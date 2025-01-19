#include <iostream>
#include <string>
#include "TradingSystem.h"
#include "WebSocketClient.h"

int main() {
    // std::string client_id = "EXemaHwx";
    // std::string client_secret = "XsxRJ_3M0STYwPQbNlmIwwkLyqKUO1CEI0hSHcLJV4M";

    // WebSocketClient client;

    // client.connect("wss://test.deribit.com/ws/api/v2");

    
    // Auth auth(client, client_id, client_secret);
    // auth.authenticate();

    // if (client.isConnected()) {
    //     client.join();  // Wait for the thread to finish execution
    // }

    // auto token = TokenManager::getInstance().getToken();
    // if (token) {
    //     std::cout << "Main Program: Access Token: " << token->access_token << "\n";
    // } else {
    //     std::cerr << "Main Program: Authentication failed.\n";
    // }

    WebSocketClient ws_client;

    std::string client_id = "";
    std::string client_secret = "";

    TradingSystem system(ws_client, client_id, client_secret);

    int choice = 0;

    while (true) {
        std::cout << "Trading System Menu:\n";
        std::cout << "1. Place Order\n";
        std::cout << "2. Cancel Order\n";
        std::cout << "3. Modify Order\n";
        std::cout << "4. Get Orderbook\n";
        std::cout << "5. View Current Positions\n";
        std::cout << "6. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                system.placeOrder();
                break;
            case 2:
                system.cancelOrder();
                break;
            case 3:
                system.modifyOrder();
                break;
            case 4:
                system.getOrderbook();
                break;
            case 5:
                system.viewPositions();
                break;
            case 6:
                std::cout << "Exiting...\n";
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }


    // client.disconnect();
    return 0;
}

