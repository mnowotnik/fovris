#ifndef QLPARSESTATE_H_I6LXWZTV
#define QLPARSESTATE_H_I6LXWZTV
#include <vector>

#include "Lexer.h"
#include "Module.h"
#include "Query.h"
namespace fovris {


struct QlParseState {
    const Lexer &lexer;
    std::vector<Module> &modules;
    std::vector<Query> queries;

    QlParseState(const Lexer &lexer, std::vector<Module> &m)
        : lexer(lexer), modules(m) {}

    unsigned getLine() const { return lexer.getLine(); }
    unsigned getPos() const { return lexer.getPos(); }
    bool errState = false;
    std::string errMsg = "";
};
} // fovris
#endif /* end of include guard: QLPARSESTATE_H_I6LXWZTV */
