#pragma once

#include <optional>
#include "Logging.h"

template <typename T>
bool AssignOrReturn(T& storage, std::optional<T> opt) {
  if (!opt.has_value()) {
    return false;
  }
  storage = std::move(*opt);
  return true;
}

#define ASSIGN_OR_RETURN(storage, value) \
  if (!AssignOrReturn(storage, opt)) {   \
    return std::nullopt;                 \
  }

template <typename T>
T AssignOrDie(std::optional<T> opt, const char* file, int line) {
  if (!opt.has_value()) {
    LOG(FATAL) << "Optional does not contain value at " << file << ": " << line;
  }
  return std::move(*opt);
}

#define ASSIGN_OR_DIE(storage, value)                 \
  storage = AssignOrDie((value), __FILE__, __LINE__);