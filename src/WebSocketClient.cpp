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

WebSocketClient::WebSocketClient()
   : ssl_context(boost::asio::ssl::context::sslv23),
     connected(false),
     stop(false) {
    ssl_context.set_default_verify_paths();
    // std::cout << "Websocket client Initialized.\n";
}

WebSocketClient::~WebSocketClient() {
    disconnect();
    if(io_thread.joinable()) {
        io_thread.join();
    }
    // std::cout << "WebSocket Client Shut Down.\n";
}

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

void WebSocketClient::disconnect() {
    if (!connected) return; // Avoid double disconnect
    

    try {
        stop = true; // Signal other threads to stop

        // Logout payload (if needed)
        json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "private/logout"},
        };

        sendMessage(payload.dump());
        connected = false;

        if (ws && ws->is_open()) {
            boost::system::error_code ec;

            // Close WebSocket gracefully
            ws->close(websocket::close_code::normal, ec);
            if (ec) {
                std::cerr << "Error closing WebSocket: " << ec.message() << "\n";
            }
        }

        // Allow pending handlers to complete
        ioc.poll();

        // Stop the I/O context
        ioc.stop();

        // Join the I/O thread if it's still running
        if (io_thread.joinable()) {
            io_thread.join();
        }

        // Notify waiting threads
        cv.notify_all();

    } catch (const std::exception &e) {
        std::cerr << "Error during disconnect: " << e.what() << "\n";
    }
}


void WebSocketClient::sendMessage(const std::string& message) {
    std::lock_guard<std::mutex> lock(ws_mutex);
    if (connected) {
        try {
            // std::cout << "Sending message: " << message << "\n"; // Log message for debugging
            ws->write(asio::buffer(message)); // Attempt to send the message
        } catch (const std::exception& e) {
            std::cerr << "Error while sending message: " << e.what() << "\n";
        }
    } else {
        std::cerr << "WebSocket is not connected. Cannot send message.\n";
    }
}
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

bool WebSocketClient::isConnected() const {
    return connected;
}

void WebSocketClient::run() {
    try {
        while (!stop) {
            beast::flat_buffer buffer;

            // Perform a read operation and handle errors gracefully
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

            // Process received message
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


void WebSocketClient::join() {
    if (io_thread.joinable()) {
        io_thread.join(); // Join the thread to wait for completion
    }
}