#pragma once

#define DELETE_COPY(T)          \
T(const T&) = delete;           \
T& operator=(const T&) = delete

#define ALLOW_MOVE(T)                    \
T(T&& other) {*this = std::move(other);} \
T& operator=(T&& other)

#define ONLY_MOVE(T) \
DELETE_COPY(T);      \
ALLOW_MOVE(T)

// Use this macro to have a class that can only be moved and only one value matters
#define ONLY_MOVE_ID(T, id)                               \
ONLY_MOVE(T) { id = other.id; other.id = 0; return *this; }

#define ONLY_MOVE_PTR(T, ptr)                                         \
ONLY_MOVE(T) { ptr = other.ptr; other.ptr = nullptr; return *this; }