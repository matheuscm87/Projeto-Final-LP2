#ifndef TSLOG_HPP
#define TSLOG_HPP

#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

class TSLogger {
private:
    std::ofstream logfile;
    std::mutex log_mutex;

    std::string timestamp() {
        auto now = std::chrono::system_clock::now();
        auto itt = std::chrono::system_clock::to_time_t(now);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &itt);
#else
        localtime_r(&itt, &tm);
#endif
        std::ostringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

public:
    TSLogger(const std::string &filename);
    ~TSLogger();

    void log(const std::string &msg);
};

#endif
