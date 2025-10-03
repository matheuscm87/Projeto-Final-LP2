#include "chat_client.hpp"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

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

void ChatClient::start() {
    std::thread receiver([&]() {
        char buffer[1024];
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytes = recv(sockfd, buffer, sizeof(buffer), 0);
            if (bytes <= 0) break;
            std::cout << ">> " << buffer << std::endl;
        }
    });
    receiver.detach();

    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        if (msg == "/quit") break;
        send(sockfd, msg.c_str(), msg.size(), 0);
    }
    close(sockfd);
}
