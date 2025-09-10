#include "pch.h"
#include "Logger.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace
{
    std::mutex gLoggerMutex;
    std::ofstream gLogFile;
    bool gConsoleOutput = true;

    std::string NowString()
    {
        using namespace std::chrono;
        auto now = system_clock::now();
        std::time_t nowTime = system_clock::to_time_t(now);
        std::tm tm;
#ifdef _WIN32
        localtime_s(&tm, &nowTime);
#else
        localtime_r(&nowTime, &tm);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    const char* ToLevelString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::LOG_INFO: return "INFO";
        case LogLevel::LOG_WARN: return "WARN";
        case LogLevel::LOG_ERROR: return "ERROR";
        }
        return "";
    }
}

void Logger::Init(const std::string& filePath, bool console)
{
    std::lock_guard<std::mutex> lock(gLoggerMutex);
    gConsoleOutput = console;
    if (!filePath.empty())
    {
        gLogFile.open(filePath, std::ios::out | std::ios::app);
    }
}

void Logger::SetFilePath(const std::string& filePath)
{
    std::lock_guard<std::mutex> lock(gLoggerMutex);
    if (gLogFile.is_open())
        gLogFile.close();

    if (!filePath.empty())
        gLogFile.open(filePath, std::ios::out | std::ios::app);
}

void Logger::SetConsoleOutput(bool enable)
{
    std::lock_guard<std::mutex> lock(gLoggerMutex);
    gConsoleOutput = enable;
}

void Logger::Info(const std::string& message)
{
    Log(LogLevel::LOG_INFO, message);
}

void Logger::Warn(const std::string& message)
{
    Log(LogLevel::LOG_WARN, message);
}

void Logger::Error(const std::string& message)
{
    Log(LogLevel::LOG_ERROR, message);
}

void Logger::Log(LogLevel level, const std::string& message)
{
    std::lock_guard<std::mutex> lock(gLoggerMutex);
    std::string line = NowString() + " [" + ToLevelString(level) + "] " + message;

    if (gConsoleOutput)
        std::cout << line << std::endl;

    if (gLogFile.is_open())
        gLogFile << line << std::endl;
}
