#include "chat_server.hpp"
#include "tslog.hpp"

int main() {
    TSLogger logger("logs/server.log");
    ChatServer server(logger);
    server.start(5000);
    return 0;
}
