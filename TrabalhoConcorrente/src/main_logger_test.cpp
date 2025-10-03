#include "tslog.hpp"
#include <thread>
#include <vector>
#include <iostream>

void worker(TSLogger &logger, int id) {
    for (int i = 0; i < 70; i++) {
        logger.log("Thread " + std::to_string(id) + " registrou mensagem " + std::to_string(i));
    }
}

int main() {
    TSLogger logger("logs/logs.txt");

    std::vector<std::thread> threads;
    for (int i = 0; i < 5; i++) {
        threads.emplace_back(worker, std::ref(logger), i);
    }

    for (auto &t : threads) {
        t.join();
    }

    std::cout << "Teste de logging concluído! Veja o arquivo logs.txt" << std::endl;
    return 0;
}
