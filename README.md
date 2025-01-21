# Trading Application (Deribit_Trading)

This project is a trading application that connects to the Deribit cryptocurrency exchange via WebSockets. It allows users to place, cancel, modify orders, fetch order books, and view current positions. The system also supports subscribing to specific instruments and streaming market data. Optimized for performance, it leverages asynchronous network communication and threading for responsive operation.

## Features

- **WebSocket-based Trading Interface**: Establishes a WebSocket connection with Deribit for real-time data and trade execution.
- **Command-line Trading Operations**: Menu-driven CLI for managing trading tasks.
- **Support for Various Order Types**: Place, cancel, or modify market and limit orders.
- **Instrument Subscription**: Subscribe to and unsubscribe from instruments for real-time updates.
- **Market Data Streaming**: Stream live market data.
- **API Authentication**: Secure authentication using client credentials.
- **Order Book Management**: View the order book for different instruments.
- **Optimized Performance**: Asynchronous operations and threading optimizations for responsiveness.

## Project Structure

The project follows a modular structure, making it maintainable and scalable. Here's an overview of the main files:

### Main Components

1. **Main Entry Point**:
   - **`main.cpp`**: Contains the main logic to initialize the system, interact with the user, and call other modules.
   - The main menu provides options for placing, modifying, canceling orders, and more.

2. **WebSocket Handling**:
   - **`WebSocketClient.cpp`**: Manages WebSocket connections with Deribit.
   - Handles real-time data exchanges, error management, and reconnections.

3. **Trade Execution**:
   - **`TradingSystem.cpp`**: Handles authentication and executes trade orders (place, modify, cancel).
   - Fetches positions and order book data.

4. **Market Data Streaming**:
   - **`MarketDataStreaming.cpp`**: Provides real-time market data updates through WebSocket.
   - Subscribes and unsubscribes from specific instruments.


### Example Workflow

- The **main program** initializes WebSocket and API credentials, then enters a menu-driven command loop.
- The **WebSocket handler** establishes a connection and listens for data.
- **Trade execution** interacts with the API to place, modify, and cancel orders.
- **Market data streaming** subscribes to and streams live data for specific instruments.

## Prerequisites

### System Requirements

- **C++ Compiler**: A C++17 compatible compiler (e.g., GCC 7+, Clang 7+, MSVC 2017+).
- **CMake**: Version 3.x or higher.
- **Boost Libraries**: Required for networking utilities (`boost::asio`).
- **OpenSSL**: For secure WebSocket and API communication.
- **Git**: For version control and repository cloning.

### Dependencies

- `nlohmann/json` (for JSON handling)
- WebSocket library
- Boost Libraries (including `boost::asio`)

## Installation

### 1. Clone the repository:

```bash
git clone https://github.com/yourusername/deribit_trading.git
cd Deribit-Trading-System
```
### 2. Install dependencies
 - Install Boost: Follow the instructions for your operating system:
```bash
sudo apt-get install libboost-all-dev libssl-dev nlohmann-json
```

### 3. Build the cmake files

```bash
cd build
cmake ..
```

### 4. Build the project

```bash
make
```

### 4. Run the application

```bash
./trading_system
```


## Usage
Once the application is running, a menu will appear for selecting trading operations:

```bash
Trading System Menu:
1. Place Order
2. Cancel Order
3. Modify Order
4. Get Orderbook
5. View Current Positions
6. Subscribe to instrument
7. Unsubscribe from instrument
8. Stream Market Data
9. Stop Streaming Market Data
10. Exit
```

### Example workflow
1. Place Order
   - Select option 1 and input the order details (e.g., instrument name, amount, price).
   - The system will place the order asynchronously and display the response.
2. Cancel Order
   - Select option 2 and input the order ID to cancel an existing order.
3. Modify Order
    - Select option 3, then input the order ID, new amount, and new price.
4. Get Orderbook
    - Select option 4 to view the current order book for a specific instrument.
5. View Current Positions
    - Select option 5 to view your current positions.
6. Subscribe to Instrument
    - Select option 6 to subscribe to real-time data for an instrument.
7. Unsubscribe from Instrument
   - Select option 7 to unsubscribe from a previously subscribed instrument.
8. Stream Market Data
    - Select option 8 to start streaming market data in the **`build/market_data_stream.log`**
9. Stop Streaming Market Data
   - Select option 9 to stop streaming market data
10. Exit
    - Select option 10 to Exit

## Run Benchmarks 

To run the benchmarks use the following command

```bash
./build/trading_benchmarks
```


