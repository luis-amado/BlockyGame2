#pragma once

#include <iostream>
#include <string>
#include <mutex>

namespace {
#define RESET "\033[0m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define BLUE "\033[34m"
#define GRAY "\033[90m"

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
} // namespace

enum LogSeverity {
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
      std::lock_guard<std::mutex> lock(s_mutex);
      std::cout << getSeverityLabel(severity) << ' ';

      if (s_showLogFile) {
        std::cout << GRAY << "(" << file << ':' << line << ") " << RESET;
      }
    }
  }

  template <typename T>
  Logger& operator<<(const T& value) {
    if (m_enabled) {
      std::lock_guard<std::mutex> lock(s_mutex);
      std::cout << value;
    }
    return *this;
  }

  ~Logger() {
    if (m_enabled) {
      std::lock_guard<std::mutex> lock(s_mutex);
      std::cout << std::endl;
      if (m_severity == FATAL) abort();
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
    case INFO:  return std::string(GREEN) + "[INFO]" + RESET;
    case INFO2: return std::string(CYAN) + "[INFO]" + RESET;
    case WARN:  return std::string(YELLOW) + "[WARNING]" + RESET;
    case ERROR: return std::string(RED) + "[ERROR]" + RESET;
    case FATAL: return std::string(RED) + "[FATAL]" + RESET;
    default:    return "UNKNOWN";
    }
  }
};

#define LOG(severity) Logger(severity, __FILENAME__, __LINE__)