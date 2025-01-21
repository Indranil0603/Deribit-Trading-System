#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <unistd.h>
#include <sys/types.h> 
#include <signal.h> 
#include "MarketDataStreaming.h"
#include <fstream>
#include <thread>

using json = nlohmann::json;

// Constructor for market data
MarketData::MarketData(WebSocketClient &ws_client)
    : websocket_client(ws_client), streamProcessPID(-1), streaming_thread(), stop_flag(true), subscription_list() {
        websocket_client.connect("wss://test.deribit.com/ws/api/v2");
    }


// Subscribe to a market data channel
void MarketData::AddSubscription(){
    std::string instrumentName, interval;
    std::cout << "Enter instrument name: ";
    std::cin >> instrumentName;
    std::cout << "Enter interval: ";
    std::cin >> interval;
    std::string channelStr = "book."+instrumentName+"."+interval;
    json options = {
        {"channels", { channelStr }} 
    };

    if(!websocket_client.isConnected()){
        throw std::runtime_error("WebSocket is not connected");
    };

    json request ={
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "public/subscribe"},
        {"params", options}
    };
    std::cout << "\nRequest: "<<request.dump() << "\n";
    websocket_client.sendMessage(request.dump());
    std::string response_str = websocket_client.receiveMessage();
    if (subscription_list.addChannel(channelStr)) {
        std::cout << "Subscribed to channel: " << channelStr << std::endl;
    } else {
        std::cout << "\nError in subscription\n";
    }
    std::cout << "Response: "<< response_str<<"\n";

}

// Remove subscription from a market channel
void MarketData::RemoveSubscription(){
    std::string channel;
    std::cout << "Enter channel: ";
    std::cin >> channel;
    
    json options = {
        {"channels", { channel }}
    };

    if(!websocket_client.isConnected()){
        throw std::runtime_error("WebSocket is not connected");
    };

    json request ={
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "public/unsubscribe"},
        {"params", options}
    };

    websocket_client.sendMessage(request.dump());
    std::string response_str = websocket_client.receiveMessage();
    if (subscription_list.removeChannel(channel)) {
        std::cout << "Unsubsribed to channel: " << channel << std::endl;
    } else {
        std::cout << "\nError in unsubscribe\n";
    }
    

}

// Start streaming market data in log file
void MarketData::startStreaming() {
    if (streaming_thread.joinable()) {
        std::cerr << "Streaming thread is already running!" << std::endl;
        return;
    }

    stop_flag = false;
    streaming_thread = std::thread(&MarketData::streamData, this); 
    std::cout << "Streaming thread started." << std::endl;
}

// Stop streaming market data in log file
void MarketData::stopStreaming() {
    if (streamProcessPID != -1) {
        if (kill(streamProcessPID, SIGTERM) == 0) {
            std::cout << "Stopped streaming process (PID: " << streamProcessPID << ")" << std::endl;
        } else {
            std::cerr << "Failed to stop streaming process!" << std::endl;
        }
        streamProcessPID = -1;
    }

    terminateStreamingThread();
}

// Get data from the market data stream
json MarketData::notification(const std::string &channel){
    json request ={
        {"jsonrpc", "2.0"},
        {"method", "subscription"},
        {"params", {{"channel", channel}}}
    };

    websocket_client.sendMessage(request.dump());
    std::string response_str = websocket_client.receiveMessage();
    return json::parse(response_str);

}

// Handle streaming of market data in log file
void MarketData::streamData() {
    std::ofstream logFile("market_data_stream.log", std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file for writing!" << std::endl;
        return;
    }
    stop_flag = false;
    try {
        while (!stop_flag) {
            subscription_list.forEachChannel([&](const std::string& channel) {
                json res = notification(channel);
                if (!res.empty() && res.contains("method")) {
                    logFile << "Channel: " << channel << ", Data: " << res << std::endl;
                }
            });
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    } catch (const std::exception &e) {
        std::cerr << "Error in streamData: " << e.what() << std::endl;
        logFile << "Error in streamData: " << e.what() << std::endl;
    }

    logFile.close();
    std::cout << "Streaming stopped gracefully." << std::endl;
}

// Handle termination of streaming of market data
void MarketData::terminateStreamingThread() {
    if (!streaming_thread.joinable()) {
        std::cerr << "No streaming thread to stop!" << std::endl;
        return;
    }

    stop_flag = true;
    streaming_thread.join();
    std::cout << "Streaming thread stopped." << std::endl;
}



