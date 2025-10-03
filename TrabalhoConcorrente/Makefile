CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread
INCLUDES = -Iinclude

SRC_DIR = src
BIN_DIR = bin
LOG_DIR = logs

ifeq ($(OS),Windows_NT)
    EXEEXT = .exe
else
    EXEEXT =
endif

LOGGER_TEST = $(BIN_DIR)/logger_test$(EXEEXT)
SERVER = $(BIN_DIR)/server$(EXEEXT)
CLIENT = $(BIN_DIR)/client$(EXEEXT)

all: | $(BIN_DIR) $(LOG_DIR) $(LOGGER_TEST) $(SERVER) $(CLIENT)

$(LOGGER_TEST): $(SRC_DIR)/main_logger_test.cpp $(SRC_DIR)/tslog.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

$(SERVER): $(SRC_DIR)/main_server.cpp $(SRC_DIR)/chat_server.cpp $(SRC_DIR)/tslog.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

$(CLIENT): $(SRC_DIR)/main_client.cpp $(SRC_DIR)/chat_client.cpp $(SRC_DIR)/tslog.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

run-logtest: $(LOGGER_TEST)
	$(LOGGER_TEST)

run-server: $(SERVER)
	$(SERVER)

run-client: $(CLIENT)
	$(CLIENT)

$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

$(LOG_DIR):
	@mkdir -p $(LOG_DIR)

clean:
	rm -rf $(BIN_DIR) $(LOG_DIR)

rebuild: clean all
