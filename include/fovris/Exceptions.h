#ifndef EXCEPTIONS_H_IMZVXPAP
#define EXCEPTIONS_H_IMZVXPAP
#include <stdexcept>
#include <utility>

#include "Utils/Print.h"

namespace fovris {

class IncorrectValueException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class InvalidTypeException : public std::runtime_error {
    using std::runtime_error::runtime_error;

  public:
    template <typename... Args>
    InvalidTypeException(Args &&... args)
        : runtime_error(print(std::forward<Args>(args)...)) {}
};

class AssertionException : public std::runtime_error {
    using std::runtime_error::runtime_error;

  public:
    template <typename... Args>
    AssertionException(Args &&... args)
        : runtime_error(print(std::forward<Args>(args)...)) {}

};

class InvalidInputException : public std::runtime_error {
    using std::runtime_error::runtime_error;

  public:
    template <typename... Args>
    InvalidInputException(Args &&... args)
        : runtime_error(print(std::forward<Args>(args)...)) {}
};

class FileAccessException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class InvalidArgumentException : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};

/**
 * Parsing error class
 */
class ParsingError : public std::runtime_error {
    unsigned line;
    unsigned pos;

  public:
    unsigned getLine() { return line; }
    unsigned getPos() { return pos; }

    ParsingError(const std::string &msg, unsigned line, unsigned pos)
        : runtime_error(msg), line(line), pos(pos) {}
};

} // fovris
#endif /* end of include guard: EXCEPTIONS_H_IMZVXPAP */
