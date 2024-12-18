#pragma once

#include <iostream>
#include <string>

namespace {
#define RESET "\033[0m"
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define BLUE "\033[34m"
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
  static void setLogLevel(LogSeverity level) {
    currentLogLevel = level;
  }

  explicit Logger(LogSeverity severity)
    : m_severity(severity), m_enabled(severity >= currentLogLevel) {
    if (m_enabled) {
      std::cout << getSeverityLabel(severity) << ' ';
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
    }
  }

private:
  LogSeverity m_severity;
  bool m_enabled;
  static LogSeverity currentLogLevel;

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

#define LOG(severity) Logger(severity)