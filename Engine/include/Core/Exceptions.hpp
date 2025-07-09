#pragma once

#include <stdexcept>
#include <string>

namespace Core {

    class Exception : public std::runtime_error {
    public:
        explicit Exception(const std::string& message)
            : std::runtime_error(message) {}
    };

    class WindowException : public Exception {
    public:
        explicit WindowException(const std::string& message)
            : Exception(message) {
        }
    };
}
