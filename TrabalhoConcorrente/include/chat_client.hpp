#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include "tslog.hpp"
#include <string>

class ChatClient {
private:
    TSLogger &logger;
    int sockfd;

public:
    ChatClient(TSLogger &log);
    void connect_server(const std::string &host, int port);
    void start();
};

#endif
