#pragma once

#include <cassert>
#include "util/Logging.h"

#ifdef DEBUG
#define DEBUG_ASSERT(statement) \
  if (!(statement)) LOG(FATAL) << "Assertion failed: (" << TERM_YELLOW << #statement << TERM_RESET << ") at " << __FILE_NAME__ << ":" << __LINE__ << TERM_RED << ' '
#define DEBUG_LOG(severity) LOG(severity)
#define DEBUG_LOG_STATEMENT(severity, statement) LOG_STATEMENT(severity, statement)

#else
#define DEBUG_ASSERT(statement) if (true) ; else std::cerr
#define DEBUG_LOG(severity) if (true) ; else std::cerr
#define DEBUG_LOG_STATEMENT(severity, statement) if (true); else std::cerr
#endif