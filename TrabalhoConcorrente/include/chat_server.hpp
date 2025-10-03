#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include "tslog.hpp"
#include <thread>
#include <vector>
#include <string>
#include <mutex>

class ChatServer {
private:
    TSLogger &logger;
    int server_fd;
    std::vector<int> clients;
    std::mutex clients_mutex;

    void handle_client(int client_socket);

public:
    ChatServer(TSLogger &log);
    void start(int port);
};

#endif
