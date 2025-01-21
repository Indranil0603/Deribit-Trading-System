#ifndef AUTH_HPP
#define AUTH_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "WebSocketClient.h"

class Auth {
private:
    std::string client_id;
    std::string client_secret;
    std::string access_token;
    std::string refresh_token;
    WebSocketClient& websocket_client;

    nlohmann::json send_auth_request(const nlohmann::json& request);


public:
    Auth(WebSocketClient& ws_client, const std::string& client_id, const std::string& client_secret);
    void authenticate();
};

#endif // AUTH_HPP
