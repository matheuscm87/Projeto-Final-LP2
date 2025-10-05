#include "chat_client.hpp"
#include "tslog.hpp"
#include <iostream>

int main() {
    TSLogger logger("logs/client.log");
    ChatClient client(logger);
    try {
        client.connect_server("127.0.0.1", 5000);

        std::string name;
        std::cout << "Informe seu nome: ";
        std::getline(std::cin, name);
        if (name.empty()) name = "anon";

        client.register_name(name);
        client.start();
    } catch (const std::exception &ex) {
        std::cerr << "Erro: " << ex.what() << std::endl;
    }
    return 0;
}
