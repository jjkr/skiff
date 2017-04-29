#include "logger.hpp"
#include <atomic>
#include <chrono>
#include <cstdio>
#include <iomanip>
#include <thread>

namespace
{
thread_local std::ostringstream localBuffer;
std::atomic<sk::LogSeverity> currentSeverity(sk::LogSeverity::DEBUG);
}

namespace sk
{
std::ostream& operator<<(std::ostream& os, LogSeverity severity)
{
    switch (severity)
    {
        case LogSeverity::FATAL:
            os << "FATAL";
            break;
        case LogSeverity::ERROR:
            os << "ERROR";
            break;
        case LogSeverity::WARN:
            os << "WARN";
            break;
        case LogSeverity::INFO:
            os << "INFO";
            break;
        case LogSeverity::DEBUG:
            os << "DEBUG";
            break;
    }
    return os;
}

void setLogSeverity(LogSeverity severity)
{
    currentSeverity = severity;
}

LogStream::LogStream(LogSeverity severity)
    : m_os(severity < currentSeverity ? nullptr : &localBuffer)
{
    if (m_os)
    {
        auto now = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch())
            .count();

        *m_os << '[' << std::setw(9) << std::dec << now / 1000000 << '.' << std::setfill('0')
              << std::setw(6) << now % 1000000ll << ']';

        auto tid = std::this_thread::get_id();
        *m_os << " Tx" << std::hex << tid << std::dec << " |" << std::setfill(' ') << std::setw(5)
              << severity << "| ";
    }
}

LogStream::~LogStream() noexcept
{
    if (m_os)
    {
        *m_os << '\n';
        auto msg = m_os->str();
        std::fwrite(msg.data(), static_cast<int>(msg.size()), 1, stdout);
        m_os->str("");
    }
}
}