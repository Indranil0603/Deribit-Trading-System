#include "auth.h"
#include "TokenManager.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <boost/asio/ssl.hpp>
#include <string>


namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;


Auth::Auth(WebSocketClient& ws_client, const std::string& client_id, const std::string& client_secret)
    : websocket_client(ws_client), client_id(client_id), client_secret(client_secret) {}

void Auth::authenticate() {
    
    try{
        // std::string host = "test.deribit.com";
        // std::string path = "/api/v2/public/auth";

        json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "public/auth"},
            {"params", {{"grant_type", "client_credentials"},
                        {"client_id", client_id},
                        {"client_secret", client_secret}}}
        };
        // std::cout<<payload.dump();
        json response = send_auth_request(payload);

        // std::string payloadStr = payload.dump();

        // asio::ssl::context ctx(asio::ssl::context::tlsv12_client);
        
        // asio::io_context io_context;
        // tcp::resolver resolver(io_context);
        // tcp::socket socket(io_context);
        // ssl::stream<tcp::socket> ssl_socket(std::move(socket), ctx);

        // tcp::resolver::results_type endpoints= resolver.resolve(host, "https");
        // asio::connect(ssl_socket.lowest_layer(), endpoints);

        // ssl_socket.handshake(ssl::stream_base::client);
        // // std::cout << "Handshake successfull";
        // http::request<http::string_body> req(http::verb::post, path, 11);
        // req.set(http::field::host, host);
        // req.set(http::field::content_type, "application/json");
        // req.set(http::field::content_length, std::to_string(payloadStr.size()));
        // req.body() = payloadStr;
        
        // // std::cout << "Request:\n" << req <<std::endl;

        // http::write(ssl_socket, req);
        // beast::flat_buffer buffer;
        // http::response<http::dynamic_body> res;
        // http::read(ssl_socket, buffer, res);

        // auto response_json = json::parse(beast::buffers_to_string(res.body().data()));
        


        if (response.contains("result") && response["result"].contains("access_token")) {
            auto result = response["result"];

            AuthDetails details = {
                result["access_token"],
                result["refresh_token"],
                result["expires_in"],
                result["scope"],
            };

            TokenManager::getInstance().storeToken(details);

            // auth_data.push_back(details);
            std::cout << "Authentication Successfull\n\n";

        } else{
            std::cerr << "Authentication failed: No result in response.\n\n";
        }

    } catch (const std::exception &ex){
        std::cerr << "Authentication error: "<< ex.what() << std::endl;
    }

    
}

json Auth::send_auth_request(const json& request){
    if( !websocket_client.isConnected() ) {
        throw std::runtime_error("WebSocket is not connected");
    }
    // std::cout << "Sending auth request: " << request.dump() << "\n";
    websocket_client.sendMessage(request.dump());
    std::string response_str = websocket_client.receiveMessage();
    // std::cout << "Response string: " << json::parse(response_str) << std::endl;
    return json::parse(response_str);
}


