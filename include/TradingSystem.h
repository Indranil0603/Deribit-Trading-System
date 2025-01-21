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
        
        nlohmann::json privateAPI(const std::string&, const nlohmann::json&) const;
        Auth auth;

    public:
        TradingSystem(WebSocketClient& , const std::string&, const std::string&);
        ~TradingSystem();

        void placeOrder() const;
        void cancelOrder() const;
        void modifyOrder() const;
        void getOrderbook() const;
        void viewPositions() const;
        
    };

#endif
