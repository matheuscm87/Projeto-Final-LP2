#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include "tslog.hpp"
#include <string>

class ChatClient {
private:
    TSLogger &logger;
public:
    ChatClient(TSLogger &log);
    void connect(const std::string &host, int port);
};

#endif
