#include "chat_server.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <algorithm>

ChatServer::ChatServer(TSLogger &log) : logger(log), server_fd(-1) {}

void ChatServer::start(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) throw std::runtime_error("Erro ao criar socket");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("Erro no bind");

    if (listen(server_fd, 10) < 0)
        throw std::runtime_error("Erro no listen");

    logger.log("Servidor iniciado na porta " + std::to_string(port));
    std::cout << "Servidor rodando na porta " << port << "..." << std::endl;

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket >= 0) {
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.push_back(client_socket);
            }
            logger.log("Novo cliente conectado");
            std::thread(&ChatServer::handle_client, this, client_socket).detach();
        }
    }
}

void ChatServer::handle_client(int client_socket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            close(client_socket);
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            }
            logger.log("Cliente desconectado");
            break;
        }
        std::string msg(buffer);
        logger.log("Mensagem recebida: " + msg);

        // Broadcast
        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int client : clients) {
            if (client != client_socket) {
                send(client, msg.c_str(), msg.size(), 0);
            }
        }
    }
}
