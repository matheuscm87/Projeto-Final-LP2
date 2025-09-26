#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include "tslog.hpp"
#include <thread>
#include <vector>
#include <string>

class ChatServer {
private:
    TSLogger &logger;
    std::vector<std::thread> client_threads;
public:
    ChatServer(TSLogger &log);
    void start(int port);
};

#endif
