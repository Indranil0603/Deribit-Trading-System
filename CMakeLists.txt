cmake_minimum_required(VERSION 3.10)
project(DeribitTradingSystem)

set(CMAKE_CXX_STANDARD 17)

# Add source files
set(SOURCES
    src/main.cpp
    src/TradingSystem.cpp
    src/auth.cpp
    src/WebSocketClient.cpp
    src/MarketDataStreaming.cpp
    src/SubscriptionList.cpp
)

# Include directories
include_directories(include)

# Find required packages
find_package(Boost REQUIRED COMPONENTS system thread regex)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)

# Add the executable
add_executable(trading_system ${SOURCES})

# Link the libraries
target_link_libraries(trading_system 
    Boost::system 
    Boost::thread
    nlohmann_json::nlohmann_json 
    OpenSSL::SSL 
    OpenSSL::Crypto
)

option(ENABLE_BENCHMARKS "Enable building benchmarks" ON)
if (ENABLE_BENCHMARKS)
    # Find Google Benchmark
    find_package(benchmark REQUIRED)

    # Add benchmark source files
    set(BENCHMARK_SOURCES
        benchmarks/Benchmark.cpp
        src/TradingSystem.cpp
        src/auth.cpp
        src/WebSocketClient.cpp
        src/MarketDataStreaming.cpp
        src/SubscriptionList.cpp
    )

    # Add the benchmark executable
    add_executable(trading_benchmarks ${BENCHMARK_SOURCES})

    # Link the libraries
    target_link_libraries(trading_benchmarks 
        Boost::system 
        Boost::thread
        nlohmann_json::nlohmann_json 
        OpenSSL::SSL 
        OpenSSL::Crypto 
        benchmark::benchmark 
        pthread
    )
endif()
