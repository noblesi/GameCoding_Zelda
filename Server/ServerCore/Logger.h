#pragma once
#include <string>

enum class LogLevel
{
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
};

class Logger
{
public:
    // 파일 위치와 콘솔 출력 여부를 설정한다.
    static void Init(const std::string& filePath = "", bool console = true);

    static void SetFilePath(const std::string& filePath);
    static void SetConsoleOutput(bool enable);

    static void Info(const std::string& message);
    static void Warn(const std::string& message);
    static void Error(const std::string& message);

private:
    static void Log(LogLevel level, const std::string& message);
};

