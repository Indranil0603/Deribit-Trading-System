#ifndef WEBSOCKET_CLIENT_H
#define WEBSOCKET_CLIENT_H

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    void connect(const std::string& uri);
    void disconnect();
    void sendMessage(const std::string& message);
    bool isConnected() const;
    std::string receiveMessage();
    void join();

private:
    void run();

    boost::asio::io_context ioc;
    boost::asio::ssl::context ssl_context;
    std::unique_ptr<boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>> ws;

    std::thread io_thread;
    std::mutex ws_mutex;
    std::queue<std::string> message_queue;
    std::condition_variable cv;
    std::atomic<bool> connected;
    std::atomic<bool> stop;

};

#endif
