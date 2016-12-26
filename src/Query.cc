#include "Query.h"
#include "Module.h"
#include "Lexer.h"
#include "QlScanner.h"
#include "QlParse.h"
#include "UnsafeTerm.h"

namespace fovris {

namespace {

/* Parse string and create a Query.
 *
 * Example input: m.parent(X,Y).
 *
 */
Query makeQueryFromRawString(const std::string &rawQuery) {
    Lexer lexer(rawQuery.c_str(), rawQuery.length(),
                std::make_unique<QlScanner>());
    Lexer::TokenCode tokenCode;

    std::vector<Module> modules;

    Lexer::TokenCode tokens[] = {TOK_ID, TOK_DOT, TOK_ID, TOK_LRB};
    int tokensLen = sizeof(tokens) / sizeof(tokens[0]);


    std::string module;
    std::string predicate;
    std::vector<Term> terms;

    int tokIdx = 0;

    do {
        tokenCode = lexer.scan();
        if (tokenCode == Lexer::ErrorCode) {
            throw ParsingError("Unrecognized pattern", lexer.getLine(),
                               lexer.getPos());
        }


        if ((tokIdx < tokensLen && tokenCode != tokens[tokIdx]) ||
            (tokIdx >= tokensLen && tokIdx % 2 == 1 &&
             tokenCode != TOK_COMMA)) {
            if (tokenCode == TOK_RRB && tokIdx > tokensLen) {
                break;
            }

            throw ParsingError("Invalid syntax", lexer.getLine(),
                               lexer.getPos());
        }

        switch (tokenCode) {
            case TOK_ID:
                if (tokIdx == 0) {
                    module = lexer.getToken();
                } else if (tokIdx == 2) {
                    predicate = lexer.getToken();
                } else if (tokIdx >= tokensLen) {
                    terms.emplace_back(UnsafeTerm(lexer.getToken(), TermType::Id));
                }
                break;
            case TOK_VAR:
                terms.emplace_back(Var(lexer.getToken()));
                break;
            case TOK_STRING:
                terms.emplace_back(lexer.getToken(), TermType::String);
                break;
            case TOK_DATE:
                terms.emplace_back(Date(lexer.getToken()));
                break;
            case TOK_DATETIME:
                terms.emplace_back(DateTime(lexer.getToken()));
                break;
            case TOK_INTEGER:
                terms.emplace_back(std::stoi(lexer.getToken()));
                break;
            case TOK_REAL:
                terms.emplace_back(std::stod(lexer.getToken()));
                break;
            case TOK_TRUE:
                terms.emplace_back(TruthValue::True);
                break;
            case TOK_FALSE:
                terms.emplace_back(TruthValue::False);
                break;
            case TOK_INCONS:
                terms.emplace_back(TruthValue::Incons);
                break;
            case TOK_UNKNOWN:
                terms.emplace_back(TruthValue::Unknown);
                break;
            case TOK_DOT:
            case TOK_LRB:
            case TOK_COMMA:
                break;
            default:
                throw ParsingError("Invalid syntax", lexer.getLine(),
                                   lexer.getPos());
        }
        tokIdx++;
    } while (tokenCode != TOK_END);

    if (tokenCode == TOK_END) {
        throw ParsingError("Invalid syntax", lexer.getLine(), lexer.getPos());
    }
    return Query(module, predicate, std::move(terms));
}
}

std::ostream &operator<<(std::ostream &os, const Query &q) {
    return q.print(os, false);
}

bool Query::operator==(const Query &o) const {
    return predicateSymbol == o.predicateSymbol &&
           moduleSymbol == o.moduleSymbol && terms == o.terms;
}

std::ostream &Query::print(std::ostream &os, bool typeInfo) const {
    os << moduleSymbol << '.' << predicateSymbol << '(';

    for (auto it = terms.begin(); it != terms.end(); it++) {
        if (it > terms.begin()) {
            os << ',';
        }
        it->print(os, typeInfo);
    }

    os << ')';
    return os;
}

Query::Query(const std::string &rawQuery)
    : Query(makeQueryFromRawString(rawQuery)) {}
} // fovris
