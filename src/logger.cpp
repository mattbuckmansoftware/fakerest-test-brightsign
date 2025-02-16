#include "logger.h"
#include <chrono>
#include <iomanip>
#include <sstream>

// Initialize static members
Logger* Logger::instance = nullptr;
std::mutex Logger::mutex;

Logger::Logger() {
    // Get current date
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d");
    std::string dateStr = ss.str();

    // Open log file
    std::string filename = "/tmp/fakerestlog" + dateStr + ".log";
    logFile.open(filename, std::ios::app);

    if (!logFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

Logger::~Logger() {
    if (logFile.is_open()) {
        logFile.close();
    }
}

Logger& Logger::getInstance() {
    std::lock_guard<std::mutex> lock(mutex);
    if (instance == nullptr) {
        instance = new Logger();
    }
    return *instance;
}

Logger& Logger::operator<<(std::ostream& (*manip)(std::ostream&)) {
    if (logFile.is_open()) {
        manip(logFile);
        logFile.flush();
    }
    return *this;
}
