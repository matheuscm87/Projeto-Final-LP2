#ifndef CHAT_SERVER_HPP
#define CHAT_SERVER_HPP

#include "tslog.hpp"
#include "tsqueue.hpp"
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <utility>

class ChatServer {
private:
    TSLogger &logger;
    int server_fd;
    std::vector<int> clients;
    std::unordered_map<int, std::string> client_names;
    std::mutex clients_mutex;

    // history of messages (protected by history_mutex)
    std::vector<std::string> history;
    std::mutex history_mutex;
    const size_t HISTORY_LIMIT = 50;

    // queue for messages produced by client threads and consumed by dispatcher
    ThreadSafeQueue<std::pair<int, std::string>> message_queue;

    std::atomic<bool> running;

    void handle_client(int client_socket);
    void dispatcher_loop();

    void add_history(const std::string &m);

public:
    ChatServer(TSLogger &log);
    void start(int port);
    void stop();
};

#endif
