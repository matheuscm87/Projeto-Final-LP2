#include "chat_client.hpp"
#include "tslog.hpp"

int main() {
    TSLogger logger("logs/client.log");
    ChatClient client(logger);
    client.connect_server("127.0.0.1", 5000);
    client.start();
    return 0;
}
