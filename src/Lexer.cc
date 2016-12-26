#include "Lexer.h"
#include "make_unique.hpp"
#include <algorithm>
#include <cstring>
namespace fovris {


Lexer::Lexer(std::unique_ptr<Scanner> scanner) : scanner(std::move(scanner)) {}

Lexer::Lexer(const char *s, std::unique_ptr<Scanner> scanner)
    : Lexer(s, strlen(s), std::move(scanner)) {}

Lexer::Lexer(const char *s, unsigned n, std::unique_ptr<Scanner> scanner)
    : content(s), scanner(std::move(scanner)) {
    state.cursor = content;
    state.pos = state.cursor;
    state.limit = content + n;
}

void Lexer::reset(const char *s, unsigned n) {
    content = s;
    state.cursor = content;
    state.pos = state.cursor;
    state.limit = content + n;
}

void Lexer::reset(const char *s) { reset(s, strlen(s)); }


std::string Lexer::getToken() const {
    if (state.start == nullptr) {
        return "";
    }
    if (!state.quotedString && state.quoteEscapes.empty()) {
        return std::string(state.start, state.cursor - state.start);
    }

    if (state.quotedString && state.quoteEscapes.empty()) {
        return std::string(state.start + 1, state.cursor - state.start - 2);
    }

    std::string result = "";

    const char *start = state.start;

    if (state.quotedString) {
        start++;
    }

    for (auto escapePtr : state.quoteEscapes) {
        result += std::string(start, escapePtr - start);
        start = escapePtr + 1;
    }

    if (state.quotedString) {
        result += std::string(start, state.cursor - start - 1);
    } else {
        result += std::string(start, state.cursor - start);
    }
    return result;
}

Lexer::TokenCode Lexer::getCode() const { return tokenCode; }

unsigned Lexer::getPos() const { return state.start - state.pos + 1; }

Lexer::TokenCode Lexer::scan() {
    tokenCode = scanner->scan(state);

    if (tokenCode == Scanner::ErrorCode) {
        return ErrorCode;
    }

    return tokenCode;
}
} // fovris
