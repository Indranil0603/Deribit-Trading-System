#include "TradingSystem.h"
#include "TokenManager.h"
#include "auth.h"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TradingSystem::TradingSystem(WebSocketClient& ws_client, const std::string& client_id, const std::string& client_secret)
    : websocket_client(ws_client),
     client_id(client_id), 
     client_secret(client_secret), 
     auth(websocket_client, client_id, client_secret)  {

    websocket_client.connect("wss://test.deribit.com/ws/api/v2");
    auth.authenticate();

    // token_ptr = TokenManager::getInstance().getToken();
    std::cout << "Trading System Initialized.\n";
}

TradingSystem::~TradingSystem() {
    try{
        websocket_client.disconnect();

        std::cout << "Trading System Shut Down.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error during TradingSystem shutdown: " << e.what() << "\n";
    }
    
}

void TradingSystem::placeOrder() {
    std::string instrument_name, amount, order_type;
    std::cout << "\nEnter instrument_name: ";
    std::cin >> instrument_name;
    std::cout << "\nEnter amount: ";
    std::cin >> amount;
    std::cout << "\nEnter order_type: ";
    std::cin >> order_type;

    json options = {
        {"instrument_name", instrument_name},
        {"amount", amount},
        {"type", order_type}
    };

    json res = privateAPI("private/buy", options);
    std::cout <<"Place order result: "<<res.dump()<<"\n";
}

void TradingSystem::cancelOrder() {
    std::string order_id;
    std::cout << "Enter Order id: ";
    std::cin >> order_id;

    json options = {
        {"order_id", order_id}
    };

    json res = privateAPI("private/cancel", options);
    std::cout <<"Cancel order result: "<<res.dump()<<"\n";
}

void TradingSystem::modifyOrder() {
    std::cout << "[MODIFY ORDER] Placeholder function.\n";
}

void TradingSystem::getOrderbook() {
    std::cout << "[GET ORDERBOOK] Placeholder function.\n";
}

void TradingSystem::viewPositions() {
    std::cout << "[VIEW POSITIONS] Placeholder function.\n";
}

json TradingSystem::privateAPI(const std::string &method,const json &options){
    if(!websocket_client.isConnected()){
        throw std::runtime_error("WebSocket is not connected");
    }

    json request ={
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", method},
        {"params", options}
    };

    auth.authenticate();
    websocket_client.sendMessage(request.dump());
    std::string response_str = websocket_client.receiveMessage();
    return json::parse(response_str);
}
