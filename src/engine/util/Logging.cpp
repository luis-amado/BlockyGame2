#include "Logging.h"

LogSeverity Logger::s_currentLogLevel = INFO;
bool Logger::s_showLogFile = false;
std::mutex Logger::s_mutex;
