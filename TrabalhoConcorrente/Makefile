# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread
INCLUDES = -Iinclude

# Diretórios
SRC_DIR = src
BIN_DIR = bin
INC_DIR = include

# Detecta sistema (Windows usa .exe)
ifeq ($(OS),Windows_NT)
    EXEEXT = .exe
else
    EXEEXT =
endif

# Arquivos-fonte
LOGGER_SRC = $(SRC_DIR)/tslog.cpp
LOGGER_TEST_SRC = $(SRC_DIR)/main_logger_test.cpp

# Executáveis
LOGGER_TEST = $(BIN_DIR)/logger_test$(EXEEXT)

# Regra padrão
all: $(LOGGER_TEST)

# Compila o logger_test
$(LOGGER_TEST): $(LOGGER_TEST_SRC) $(LOGGER_SRC)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Limpeza
clean:
	rm -rf $(BIN_DIR)
