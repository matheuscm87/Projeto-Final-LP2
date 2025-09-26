#include "chat_server.hpp"

ChatServer::ChatServer(TSLogger &log) : logger(log) {}

void ChatServer::start(int port) {
    logger.log("Servidor iniciado na porta " + std::to_string(port));
    // Etapa 2 implementará sockets e broadcast
}
