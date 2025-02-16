#pragma once

#include <fstream>
#include <iostream>
#include <mutex>

class Logger {
private:
    std::ofstream logFile;
    static Logger* instance;
    static std::mutex mutex;

    Logger();  // Private constructor

public:
    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    ~Logger();

    static Logger& getInstance();

    template<typename T>
    Logger& operator<<(const T& msg) {
        if (logFile.is_open()) {
            logFile << msg;
            logFile.flush();
        }
        return *this;
    }

    Logger& operator<<(std::ostream& (*manip)(std::ostream&));
};
