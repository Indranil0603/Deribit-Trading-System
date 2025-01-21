#ifndef TRADING_SYSTEM_H
#define TRADING_SYSTEM_H

#include <iostream>
#include <memory> // For smart pointers
#include <WebSocketClient.h>
#include <nlohmann/json.hpp>
#include <auth.h>

class TradingSystem {
    private:
        std::string client_id;
        std::string client_secret;
        WebSocketClient& websocket_client;
        
        nlohmann::json privateAPI(const std::string&, const nlohmann::json&);
        Auth auth;

    public:
        TradingSystem(WebSocketClient& , const std::string&, const std::string&);
        ~TradingSystem();

        void placeOrder();
        void cancelOrder();
        void modifyOrder();
        void getOrderbook();
        void viewPositions();
        
    };

#endif
