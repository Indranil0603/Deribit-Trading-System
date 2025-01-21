#ifndef MARKET_DATA_HPP
#define MARKET_DATA_HPP

#include "WebSocketClient.h"
#include <unistd.h>      
#include <sys/types.h>  
#include <signal.h>      
#include <thread>        
#include <atomic>
#include <nlohmann/json.hpp>
#include <SubscriptionList.h>

class MarketData {
    private:
        WebSocketClient& websocket_client;
        SubscriptionList subscription_list;
        std::atomic<bool> stop_flag;  
        std::thread streaming_thread; 
        pid_t streamProcessPID;

    
    public:
        MarketData(WebSocketClient& ws_client);
        void AddSubscription();
        void RemoveSubscription();
        void startStreaming();
        void stopStreaming();
        void streamData();
        void terminateStreamingThread();
        nlohmann::json notification(const std::string &);
};

#endif //  MARKET_DATA_HPP
