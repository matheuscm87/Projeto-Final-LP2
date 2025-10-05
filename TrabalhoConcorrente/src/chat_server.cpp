#include "chat_server.hpp"
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <algorithm>
#include <sys/socket.h>
#include <netinet/in.h>

ChatServer::ChatServer(TSLogger &log) : logger(log), server_fd(-1), running(false) {}

void ChatServer::add_history(const std::string &m) {
    std::lock_guard<std::mutex> lk(history_mutex);
    history.push_back(m);
    if (history.size() > HISTORY_LIMIT) history.erase(history.begin());
}

void ChatServer::start(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) throw std::runtime_error("Erro ao criar socket");

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        throw std::runtime_error("Erro no bind");

    if (listen(server_fd, 10) < 0)
        throw std::runtime_error("Erro no listen");

    logger.log("Servidor iniciado na porta " + std::to_string(port));
    std::cout << "Servidor rodando na porta " << port << "..." << std::endl;

    running = true;
    // dispatcher thread: consome queue e faz broadcast
    std::thread dispatcher(&ChatServer::dispatcher_loop, this);
    dispatcher.detach();

    while (running) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket >= 0) {
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.push_back(client_socket);
            }
            logger.log("Novo cliente conectado (socket " + std::to_string(client_socket) + ")");
            std::thread(&ChatServer::handle_client, this, client_socket).detach();
        }
    }

    // cleanup
    message_queue.close();
    close(server_fd);
}

void ChatServer::stop() {
    running = false;
    // trigger accept to return (on Linux you could close the server_fd)
    if (server_fd >= 0) close(server_fd);
    message_queue.close();
}

void ChatServer::handle_client(int client_socket) {
    char buffer[4096];

    // First message expected to be the client's name (simple handshake)
    memset(buffer, 0, sizeof(buffer));
    int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);
    if (bytes <= 0) {
        close(client_socket);
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
        }
        logger.log("Cliente desconectou antes do handshake (socket " + std::to_string(client_socket) + ")");
        return;
    }
    std::string name(buffer, bytes);
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_names[client_socket] = name;
    }
    logger.log("Cliente registrou nome: " + name + " (socket " + std::to_string(client_socket) + ")");

    // Send welcome and history
    std::string welcome = "Servidor: Bem-vindo, " + name + "!\n";
    send(client_socket, welcome.c_str(), welcome.size(), 0);

    {
        std::lock_guard<std::mutex> hk(history_mutex);
        for (const auto &h : history) {
            std::string line = h + "\n";
            send(client_socket, line.c_str(), line.size(), 0);
        }
    }

    // Notify others
    std::string joinmsg = name + " entrou no chat.";
    logger.log(joinmsg);
    add_history(joinmsg);
    message_queue.push({client_socket, joinmsg});

    // Main receive loop
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(client_socket, buffer, sizeof(buffer)-1, 0);
        if (bytes <= 0) {
            // disconnected
            close(client_socket);
            std::string disc_reason = "Cliente desconectado (socket " + std::to_string(client_socket) + ")";
            logger.log(disc_reason);

            std::string the_name;
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                auto it = client_names.find(client_socket);
                if (it != client_names.end()) {
                    the_name = it->second;
                    client_names.erase(it);
                }
                clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
            }
            if (!the_name.empty()) {
                std::string leftmsg = the_name + " saiu do chat.";
                logger.log(leftmsg);
                add_history(leftmsg);
                message_queue.push({client_socket, leftmsg});
            }
            break;
        }

        std::string msg(buffer, bytes);
        std::string from_name;
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            auto it = client_names.find(client_socket);
            if (it != client_names.end()) from_name = it->second;
            else from_name = "unknown";
        }
        std::string composed = from_name + ": " + msg;
        logger.log("Mensagem recebida: " + composed);
        add_history(composed);

        // Push to queue for dispatcher to broadcast
        message_queue.push({client_socket, composed});
    }
}

void ChatServer::dispatcher_loop() {
    while (true) {
        auto opt = message_queue.pop();
        if (!opt.has_value()) break; // queue closed and empty

        int from_socket = opt->first;
        std::string msg = opt->second + "\n";

        // snapshot clients to avoid holding lock while sending
        std::vector<int> clients_snapshot;
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients_snapshot = clients;
        }

        std::vector<int> to_remove;
        for (int c : clients_snapshot) {
            if (c == from_socket) continue; // do not echo to sender
            int s = send(c, msg.c_str(), msg.size(), 0);
            if (s <= 0) {
                to_remove.push_back(c);
            }
        }

        // Remove dead sockets
        if (!to_remove.empty()) {
            std::lock_guard<std::mutex> lock(clients_mutex);
            for (int dead : to_remove) {
                clients.erase(std::remove(clients.begin(), clients.end(), dead), clients.end());
                auto it = client_names.find(dead);
                if (it != client_names.end()) {
                    std::string leftmsg = it->second + " saiu do chat (conexão perdida).";
                    add_history(leftmsg);
                    logger.log(leftmsg);
                    client_names.erase(it);
                }
                close(dead);
            }
        }
    }
}
