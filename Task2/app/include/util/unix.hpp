#pragma once

#include <cerrno>
#include <cstring>

#define ERROR_LOG ", " << std::strerror(errno) << " [" << errno << "]"
