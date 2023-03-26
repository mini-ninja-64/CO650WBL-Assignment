#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <unordered_set>
#include <functional>

typedef std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> ConnectionHandler;

class SocketServer {
private:
    const ConnectionHandler connectionHandler;
    std::thread newConnectionListener;
    std::thread currentConnectionHandler;

    int socketDescriptor;
    sockaddr_in socketAddress;

    bool initialised = false;
    bool running = false;
    std::mutex connectionListMutex;
    std::unordered_set<int> connectionList;

    int initialise();


public:
//    Add Handler for new connections and disconnects
    SocketServer(int port, ConnectionHandler connectionHandler);
    ~SocketServer();

    [[nodiscard]] int start();
    [[nodiscard]] int stop();

    void block();
};
