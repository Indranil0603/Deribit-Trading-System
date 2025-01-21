#include <iostream>
#include <string>
#include "TradingSystem.h"
#include "WebSocketClient.h"
#include "MarketDataStreaming.h"

int main(int argc, char *argv[]) {


    std::string client_id, client_secret;
    std::cout << "\nEnter client id: ";
    std::cin >> client_id;
    std::cout << "\nEnter client_secret: ";
    std::cin >> client_secret;
    
    //  = "EXemaHwx";
    // std::string client_secret = "XsxRJ_3M0STYwPQbNlmIwwkLyqKUO1CEI0hSHcLJV4M";

    WebSocketClient ws_market;
    MarketData market(ws_market);

    WebSocketClient ws_client;
    TradingSystem system(ws_client, client_id, client_secret);

    int choice = 0;

    while (true) {
        std::cout << "\nTrading System Menu:\n";
        std::cout << "1. Place Order\n";
        std::cout << "2. Cancel Order\n";
        std::cout << "3. Modify Order\n";
        std::cout << "4. Get Orderbook\n";
        std::cout << "5. View Current Positions\n";
        std::cout << "6. Subscribe to instrument\n";
        std::cout << "7. Unsubscribe from instrument\n";
        std::cout << "8. Stream Market Data\n";
        std::cout << "9. Stop Streaming Market Data\n";
        std::cout << "10. Exit\n";
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
                market.AddSubscription();
                break;
            case 7:
                market.RemoveSubscription();
                break;
            case 8:
                market.startStreaming();
                break;
            case 9:
                market.stopStreaming();
                break;
            case 10:
                std::cout << "Exiting...\n";
                return 0;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }

    return 0;
}

