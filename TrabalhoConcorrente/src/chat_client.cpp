#include "chat_client.hpp"
#include "tslog.hpp"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdexcept>

ChatClient::ChatClient(TSLogger &log) : logger(log), sockfd(-1) {}

void ChatClient::connect_server(const std::string &host, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) throw std::runtime_error("Erro ao criar socket");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("Erro ao conectar ao servidor");

    logger.log("Conectado ao servidor " + host + ":" + std::to_string(port));
}

void ChatClient::register_name(const std::string &name) {
    if (sockfd < 0) throw std::runtime_error("Socket não conectado");
    send(sockfd, name.c_str(), name.size(), 0);
}

void ChatClient::send_raw(const std::string &msg) {
    if (sockfd < 0) return;
    send(sockfd, msg.c_str(), msg.size(), 0);
}

void ChatClient::start() {
    // Receiver thread
    std::thread receiver([&]() {
        char buffer[4096];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes = recv(sockfd, buffer, sizeof(buffer)-1, 0);
            if (bytes <= 0) break;
            std::cout << buffer << std::endl;
        }
    });
    receiver.detach();

    // Sender loop (stdin)
    std::string msg;
    while (true) {
        if (!std::getline(std::cin, msg)) break;
        if (msg == "/quit") break;
        send_raw(msg);
    }
    close(sockfd);
}
