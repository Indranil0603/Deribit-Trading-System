#include "TradingSystem.h"
#include "auth.h"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Initialize the trading system
TradingSystem::TradingSystem(WebSocketClient& ws_client, const std::string& client_id, const std::string& client_secret)
    : websocket_client(ws_client),
     client_id(client_id), 
     client_secret(client_secret), 
     auth(websocket_client, client_id, client_secret)  {

    websocket_client.connect("wss://test.deribit.com/ws/api/v2");
    auth.authenticate();

    std::cout << "Trading System Initialized.\n";
}

// Handle closing of Trading system
TradingSystem::~TradingSystem() {
    try{

        std::cout << "Trading System Shut Down.\n";
    } catch (const std::exception& e) {
        std::cerr << "Error during TradingSystem shutdown: " << e.what() << "\n";
    }
    
}

// Placing order
void TradingSystem::placeOrder() const {
    std::string order, instrument_name, amount, order_type, price;
    std::cout << "\nEnter order type(buy/sell): ";
    std::cin >> order;
    std::cout << "\nEnter instrument_name: ";
    std::cin >> instrument_name;
    std::cout << "\nEnter amount: ";
    std::cin >> amount;
    std::cout << "\nEnter price: ";
    std::cin >> price;
    std::cout << "\nEnter order kind(market, limit): ";
    std::cin >> order_type;

    json options = {
        {"instrument_name", instrument_name},
        {"amount", amount},
        {"price", price},
        {"type", order_type}
    };

    json res = privateAPI("private/"+order, options);

    if(res.contains("result")){
        std::cout <<"Place order result: "<<res["result"].dump(4)<<"\n";
    }
    else{
        std::cout <<"Error in placing order : "<<res["error"].dump(4)<<"\n";
    }
}

// Canceling order
void TradingSystem::cancelOrder() const {
    std::string order_id;
    std::cout << "Enter Order id: ";
    std::cin >> order_id;

    json options = {
        {"order_id", order_id}
    };

    json res = privateAPI("private/cancel", options);
    if(res.contains("result")){
        std::cout <<"Cancel order result: "<<res.dump(4)<<"\n";
    }else{
        std::cout <<"Error in canceling order : "<<res["error"].dump(4)<<"\n";
    }


}

// Modifying Order
void TradingSystem::modifyOrder() const {
    std::string order_id, amount , price;
    std::cout << "\nEnter order id: ";
    std::cin >> order_id;
    std::cout << "\nEnter amount: ";
    std::cin >> amount;
    std::cout << "\nEnter price: ";
    std::cin >> price;
    std::cout << "\nEnter order_type: ";

    json options = {
        {"order_id", order_id},
        {"amount", amount},
        {"price", price},
    };

    json res = privateAPI("private/edit", options);

    if(res.contains("result")){
        std::cout <<"Modify order result: "<<res.dump(4)<<"\n";
    }else{
        std::cout <<"Error in modifying order : "<<res["error"].dump(4)<<"\n";
    }
}

// Get the open orders
void TradingSystem::getOrderbook() const {
    json options ={};

    json res = privateAPI("private/get_open_orders", options);
    if(res.contains("result")){
        std::cout <<"Cancel order result: "<<res.dump(4)<<"\n";
    }else{
        std::cout <<"Error in placing order : "<<res["error"].dump(4)<<"\n";
    }
}

// Get Current Positions
void TradingSystem::viewPositions() const {
    json options = {};

    json res = privateAPI("private/get_positions", options);
    if(res.contains("result")){
        std::cout <<"Cancel order result: "<<res.dump(4)<<"\n";
    }else{
        std::cout <<"Error in placing order : "<<res["error"].dump(4)<<"\n";
    }
}

// Handle requests of private type
json TradingSystem::privateAPI(const std::string &method,const json &options) const {
    if(!websocket_client.isConnected()){
        throw std::runtime_error("WebSocket is not connected");
    }

    json request ={
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", method},
        {"params", options}
    };

    // auth.authenticate();
    websocket_client.sendMessage(request.dump());
    std::string response_str = websocket_client.receiveMessage();
    return json::parse(response_str);
}
