#include "WebSocketClient.h"
#include <iostream>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <nlohmann/json.hpp>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

// Initialize WebSocket Client
WebSocketClient::WebSocketClient()
   : ssl_context(boost::asio::ssl::context::sslv23),
     connected(false),
     stop(false) {
    ssl_context.set_default_verify_paths();
    // std::cout << "Websocket client Initialized.\n";
}

// Shut Down WebSocket Client
WebSocketClient::~WebSocketClient() {
    disconnect();
    if(io_thread.joinable()) {
        io_thread.join();
    }
    // std::cout << "WebSocket Client Shut Down.\n";
}

// Connect using websocket
void WebSocketClient::connect(const std::string& uri) {
    try{
        auto const pos = uri.find("://");
        if(pos == std::string::npos || uri.substr(0, pos) != "wss") {
            throw std::invalid_argument("Only wss:// URIs are supported");
        }

        std::string host = uri.substr(pos + 3);
        std::string port = "443";

        if(auto colon_pos = host.find(":"); colon_pos != std::string::npos){
            port = host.substr(colon_pos + 1);
            host = host.substr(0, colon_pos);
        }

        tcp::resolver resolver(ioc);
        boost::asio::ssl::context ssl_ctx(boost::asio::ssl::context::tlsv12_client);
        
        auto const results = resolver.resolve("test.deribit.com", "443");

        tcp::socket socket(ioc);
        boost::asio::connect(socket, results.begin(), results.end());

        boost::asio::ssl::stream<tcp::socket> ssl_stream(std::move(socket), ssl_ctx);

        ws = std::make_unique<boost::beast::websocket::stream<boost::asio::ssl::stream<tcp::socket>>>(std::move(ssl_stream));
        boost::asio::ip::tcp::no_delay option(true);
        asio::connect(ws->next_layer().next_layer(), results.begin(), results.end());

        ws->next_layer().handshake(boost::asio::ssl::stream_base::client);
        ws->handshake("test.deribit.com", "/ws/api/v2/");

        connected = true;
        stop = false;

        io_thread = std::thread([this] {this->run(); });


    } catch (const std::exception &e){
        std::cerr << "Error during connect: "<<e.what() << "\n";
    }
}

// Disconnect websocket
void WebSocketClient::disconnect() {
    if (!connected) return; // Avoid double disconnect
    
    try {
        stop = true; 

        json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "private/logout"},
        };

        sendMessage(payload.dump());
        connected = false;

        if (ws && ws->is_open()) {
            boost::system::error_code ec;
            ws->close(websocket::close_code::normal, ec);
            if (ec) {
                // std::cerr << "Error closing WebSocket: " << ec.message() << "\n";
                ;
            }
        }
        ioc.poll();
        ioc.stop();
        if (io_thread.joinable()) {
            io_thread.join();
        }
        cv.notify_all();

    } catch (const std::exception &e) {
        std::cerr << "Error during disconnect: " << e.what() << "\n";
    }
}

// Handle Sending Message over websocket
void WebSocketClient::sendMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(ws_mutex);
    if (connected) {
        try {
            ws->write(asio::buffer(message));
        } catch (const std::exception& e) {
            std::cerr << "Error while sending message: " << e.what() << "\n";
        }
    } else {
        std::cerr << "WebSocket is not connected. Cannot send message.\n";
    }
}

// Handle Receiving message over websocket
std::string WebSocketClient::receiveMessage() {
    std::unique_lock<std::mutex> lock(ws_mutex);
    cv.wait(lock, [this] { return !message_queue.empty() || stop; });

    if (stop && message_queue.empty()) {
        return "";
    }

    std::string message = std::move(message_queue.front());
    message_queue.pop();
    return message;
}

// Check for connection to websocket
bool WebSocketClient::isConnected() const {
    return connected;
}

// Asynchronously run websocket
void WebSocketClient::run() {
    try {
        while (!stop) {
            beast::flat_buffer buffer;
            boost::system::error_code ec;
            ws->read(buffer, ec);
            if (ec) {
               if (ec == websocket::error::closed || ec == boost::asio::error::eof) {
                    std::cerr << "WebSocket closed (EOF or by server).\n";
                    break;
                } else if (ec == boost::asio::error::operation_aborted) {
                    std::cerr << "WebSocket operation aborted.\n";
                    break;
                } else {
                    std::cerr << "Error reading from WebSocket: " << ec.message() << "\n";
                    break;
                }
            }
            {
                std::lock_guard<std::mutex> lock(ws_mutex);
                message_queue.push(beast::buffers_to_string(buffer.data()));
            }
            cv.notify_all();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in I/O thread: " << e.what() << "\n";
    }
}

// Join back websocket thread 
void WebSocketClient::join() {
    if (io_thread.joinable()) {
        io_thread.join();
    }
}