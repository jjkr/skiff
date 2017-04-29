#pragma once
#include <ostream>
#include <sstream>

#define logd sk::LogStream::debug()
#define logi sk::LogStream::info()
#define logw sk::LogStream::warn()
#define loge sk::LogStream::error()

namespace sk
{
enum class LogSeverity
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};
std::ostream& operator<<(std::ostream& os, LogSeverity severity);

void setLogSeverity(LogSeverity severity);

class LogStream
{
public:
    LogStream(LogSeverity severity);
    ~LogStream() noexcept;
    void operator=(const LogStream&) = delete;
    LogStream(const LogStream&) = delete;

    void operator=(LogStream&& ls) noexcept
    {
        swap(m_os, ls.m_os);
    }
    LogStream(LogStream&& ls) noexcept : m_os(ls.m_os)
    {
        ls.m_os = nullptr;
    }

    template <typename T>
    LogStream& operator<<(T& x)
    {
        if (m_os)
        {
            *m_os << x;
        }
        return *this;
    }

    static LogStream debug() { return LogStream(LogSeverity::DEBUG); }
    static LogStream info() { return LogStream(LogSeverity::INFO); }
    static LogStream warn() { return LogStream(LogSeverity::WARN); }
    static LogStream error() { return LogStream(LogSeverity::ERROR); }

private:
    std::ostringstream* m_os;
};
}