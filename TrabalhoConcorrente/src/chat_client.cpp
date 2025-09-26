#include "chat_client.hpp"

ChatClient::ChatClient(TSLogger &log) : logger(log) {}

void ChatClient::connect(const std::string &host, int port) {
    logger.log("Cliente conectando em " + host + ":" + std::to_string(port));
    // Etapa 2 implementará a conexão real via sockets
}
