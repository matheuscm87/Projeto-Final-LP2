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

    // envia o nome/username para o servidor (handshake inicial)
    void register_name(const std::string &name);

    // envia uma mensagem bruta
    void send_raw(const std::string &msg);

    // loop principal: leitura do stdin e envio das mensagens
    void start();
};

#endif
