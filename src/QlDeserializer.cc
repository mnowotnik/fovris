#include <cstdlib>
#include <cstring>
#include <typeinfo>

#include "QlParse.h"
#include "QlDeserializer.h"
#include "QlParseState.h"
#include "QlScanner.h"

void *ParseAlloc(void *(*allocProc)(size_t));
void Parse(void *, int, std::string *, fovris::QlParseState *);
void ParseFree(void *, void (*freeProc)(void *));

namespace fovris {

std::vector<Query> QlDeserializer::getQueries() { return queries_; }

std::vector<Module> &&QlDeserializer::getModules() {
    return std::move(modules_);
}

void QlDeserializer::parse(const std::string &programStr) {
    void *p = ParseAlloc(malloc);

    Lexer lexer(programStr.c_str(), programStr.length(),
                std::make_unique<QlScanner>());
    Lexer::TokenCode tokenCode;

    std::vector<Module> modules;
    QlParseState ps(lexer, modules);

    do {
        tokenCode = lexer.scan();
        if (tokenCode == Lexer::ErrorCode) {
            ps.errState = true;
            break;
        }

        switch (tokenCode) {
            case TOK_END:
            case TOK_MODULE:
            case TOK_DOMAINS:
            case TOK_RELATIONS:
            case TOK_RULES:
            case TOK_FACTS:
            case TOK_MODULE_END:
            case TOK_IMPL:
            case TOK_COLON:
            case TOK_DOT:
            case TOK_COMMA:
            case TOK_OR:
            case TOK_TRUE:
            case TOK_FALSE:
            case TOK_INCONS:
            case TOK_UNKNOWN:
            case TOK_ID_TYPE:
            case TOK_STRING_TYPE:
            case TOK_INTEGER_TYPE:
            case TOK_REAL_TYPE:
            case TOK_DATE_TYPE:
            case TOK_DATETIME_TYPE:
            case TOK_LRB:
            case TOK_RRB:
            case TOK_LCB:
            case TOK_RCB:
            case TOK_IN:
            case TOK_NEG:
                Parse(p, tokenCode, nullptr, &ps);
                break;
            default:
                std::string val = lexer.getToken();
                Parse(p, tokenCode, new std::string(val), &ps);
        }
    } while (tokenCode != TOK_END && !ps.errState);

    // terminate parsing
    Parse(p, 0, NULL, &ps);

    // free memory
    ParseFree(p, free);

    if (ps.errState) {

        if (ps.errMsg.empty()) {
            ps.errMsg = "Unexpected token";
        }

        throw ParsingError(ps.errMsg, lexer.getLine(), lexer.getPos());
    }
    queries_.clear();
    queries_ = std::move(ps.queries);
    modules_ = std::move(modules);
}
} // fovris
