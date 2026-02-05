#pragma once

#include <iostream>
#include <string>
#include <mutex>

namespace {
#define TERM_RESET "\033[0m"
#define TERM_RED "\033[31m"
#define TERM_YELLOW "\033[33m"
#define TERM_GREEN "\033[32m"
#define TERM_CYAN "\033[36m"
#define TERM_BLUE "\033[34m"
#define TERM_GRAY "\033[90m"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
} // namespace

enum LogSeverity {
  EXTRA,
  INFO,
  INFO2,
  WARN,
  ERROR,
  FATAL
};

class Logger {
public:
  static void showFiles(bool value = true) {
    s_showLogFile = value;
  }

  static void setLogLevel(LogSeverity level) {
    s_currentLogLevel = level;
  }

  explicit Logger(LogSeverity severity, const char* file, int line)
    : m_severity(severity), m_enabled(severity >= s_currentLogLevel) {
    if (m_enabled) {
      s_mutex.lock();
      std::cout << getSeverityLabel(severity) << ' ';

      if (s_showLogFile) {
        std::cout << TERM_GRAY << "(" << file << ':' << line << ") " << TERM_RESET;
      }
    }
  }

  template <typename T>
  Logger& operator<<(const T& value) {
    if (m_enabled) {
      std::cout << value;
    }
    return *this;
  }

  ~Logger() {
    if (m_enabled) {
      std::cout << std::endl;
      if (m_severity == FATAL) abort();
      s_mutex.unlock();
    }
  }

private:
  LogSeverity m_severity;
  bool m_enabled;
  static LogSeverity s_currentLogLevel;
  static bool s_showLogFile;
  static std::mutex s_mutex;

  inline std::string getSeverityLabel(LogSeverity severity) {
    switch (severity) {
    case EXTRA: return std::string(TERM_GRAY) + "[EXTRA]" + TERM_RESET;
    case INFO:  return std::string(TERM_GREEN) + "[INFO]" + TERM_RESET;
    case INFO2: return std::string(TERM_CYAN) + "[INFO]" + TERM_RESET;
    case WARN:  return std::string(TERM_YELLOW) + "[WARNING]" + TERM_RESET;
    case ERROR: return std::string(TERM_RED) + "[ERROR]" + TERM_RESET;
    case FATAL: return std::string(TERM_RED) + "[FATAL]" + TERM_RESET;
    default:    return "UNKNOWN";
    }
  }
};

#define LOG(severity) Logger(severity, __FILENAME__, __LINE__)

#define LOG_STATEMENT(severity, statement) LOG(severity) << #statement << " = " << statement << " "