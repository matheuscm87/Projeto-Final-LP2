#include "tslog.hpp"

TSLogger::TSLogger(const std::string &filename) {
    logfile.open(filename, std::ios::app);
    if (!logfile.is_open()) {
        throw std::runtime_error("Erro ao abrir arquivo de log");
    }
}

TSLogger::~TSLogger() {
    if (logfile.is_open()) logfile.close();
}

void TSLogger::log(const std::string &msg) {
    std::lock_guard<std::mutex> lock(log_mutex);
    logfile << "[" << timestamp() << "] " << msg << std::endl;
}
