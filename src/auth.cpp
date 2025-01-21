#include "auth.h"
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

// Authenticate user
void Auth::authenticate() {
    try{
        json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "public/auth"},
            {"params", {{"grant_type", "client_credentials"},
                        {"client_id", client_id},
                        {"client_secret", client_secret}}}
        };
        json response = send_auth_request(payload);
        if (response.contains("result") && response["result"].contains("access_token")) {
            ;
            // std::cout << "Authentication Successfull\n";
        } else{
            std::cerr << "Authentication failed: No result in response.\n";
        }
    } catch (const std::exception &ex){
        std::cerr << "Authentication error: "<< ex.what() << std::endl;
    }

}

// Handle sending of auth request
json Auth::send_auth_request(const json& request){
    if( !websocket_client.isConnected() ) {
        throw std::runtime_error("WebSocket is not connected");
    }
    websocket_client.sendMessage(request.dump());
    std::string response_str = websocket_client.receiveMessage();
    return json::parse(response_str);
}


