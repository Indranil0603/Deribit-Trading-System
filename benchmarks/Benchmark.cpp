#include <benchmark/benchmark.h>
#include <sstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "TradingSystem.h"
#include "WebSocketClient.h"
#include "MarketDataStreaming.h"

using json = nlohmann::json;

std::ostream nullStream(nullptr);

std::string client_id = "client_id here";
std::string client_secret = "client_secret here";

WebSocketClient ws_client;
TradingSystem trading_system(ws_client, client_id, client_secret);

WebSocketClient ws_market;
MarketData market(ws_market);


// Benchmark Placing order Latency
static void BenchmarkPlaceOrder(benchmark::State& state) {
    std::streambuf* originalCinStream = std::cin.rdbuf();
    std::streambuf* originalCoutStream = std::cout.rdbuf();
    std::streambuf* originalCerrStream = std::cerr.rdbuf();

    std::istringstream mockPlaceOrder("buy\nETH-PERPETUAL\n1\n50000\nlimit\n");
    std::cin.rdbuf(mockPlaceOrder.rdbuf());
    std::cout.rdbuf(nullStream.rdbuf()); 
    std::cerr.rdbuf(nullStream.rdbuf()); 

    for (auto _ : state) {
        trading_system.placeOrder(); 
    }

    std::cin.rdbuf(originalCinStream);
    std::cout.rdbuf(originalCoutStream);
    std::cerr.rdbuf(originalCerrStream);
}

// Benchmark Market Data Latency
static void BenchmarkMarketData(benchmark::State &state) {
    
    std::streambuf* originalCinStream = std::cin.rdbuf();
    std::streambuf* originalCoutStream = std::cout.rdbuf();
    std::streambuf* originalCerrStream = std::cerr.rdbuf();

    std::istringstream mockSubscribe("ETH-PERPETUAL\n100ms\n");
    std::cin.rdbuf(mockSubscribe.rdbuf());
    std::cout.rdbuf(nullStream.rdbuf());
    std::cerr.rdbuf(nullStream.rdbuf());

    // Subscribe to market
    market.AddSubscription();

    // Benchmark the market data fetch process
    std::string channel = "book.ETH-PERPETUAL.100ms\n";
    for (auto _ : state) {
        json res = market.notification(channel);
    }


    std::istringstream mockUnsubscribe("ETH-PERPETUAL\n1\n100ms");
    std::cin.rdbuf(mockUnsubscribe.rdbuf()); 
    std::cout.rdbuf(nullStream.rdbuf());
    std::cerr.rdbuf(nullStream.rdbuf());

    // Remove subscription after benchmark
    market.RemoveSubscription();

    std::cin.rdbuf(originalCinStream);
    std::cout.rdbuf(originalCoutStream);
    std::cerr.rdbuf(originalCerrStream);
}

// Benchmark Websocket propagation delay
static void BenchmarkWebSocketPropagation(benchmark::State &state) {
    json request = {
        {"jsonrpc", "2.0"},
        {"id", 35},
        {"method", "public/test"},
        {"params", {}}
    };

    // Benchmark sending and receiving of message from websocket
    for (auto _ : state) {
        ws_client.sendMessage(request.dump());
        std::string response_str = ws_client.receiveMessage();
    };
}

// Benchmark full trading loop delay
static void BenchmarkFullTradingLoop(benchmark::State& state) {
    std::streambuf* originalCinStream = std::cin.rdbuf();
    std::streambuf* originalCoutStream = std::cout.rdbuf();
    std::streambuf* originalCerrStream = std::cerr.rdbuf();

    std::istringstream mockBuyOrder("buy\nETH-PERPETUAL\n1\n50000\nlimit\n");
    std::cin.rdbuf(mockBuyOrder.rdbuf());
    std::cout.rdbuf(nullStream.rdbuf());  
    std::cerr.rdbuf(nullStream.rdbuf());

    // Benchmark complete trading loop
    for (auto _ : state) {
        // Place buy order and measure the latency
        trading_system.placeOrder(); 
        
        // Now place a sell order in the same iteration
        std::istringstream mockSellOrder("sell\nETH-PERPETUAL\n1\n50000\nlimit\n");
        std::cin.rdbuf(mockSellOrder.rdbuf());
        trading_system.placeOrder();
    }

    std::cin.rdbuf(originalCinStream);
    std::cout.rdbuf(originalCoutStream);
    std::cerr.rdbuf(originalCerrStream);
}


// Register the benchmark function
BENCHMARK(BenchmarkPlaceOrder);
BENCHMARK(BenchmarkMarketData);
BENCHMARK(BenchmarkWebSocketPropagation);
BENCHMARK(BenchmarkFullTradingLoop);

BENCHMARK_MAIN();
