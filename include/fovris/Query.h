#ifndef QUERY_H_2UGKWPIJ
#define QUERY_H_2UGKWPIJ
#include <string>
#include <vector>

#include "Term.h"
namespace fovris {

struct Query {
    const std::string moduleSymbol;
    const std::string predicateSymbol;
    const std::vector<Term> terms;

    Query(std::string m, std::string p, std::vector<Term> terms)
        : moduleSymbol(std::move(m))
        , predicateSymbol(std::move(p))
        , terms(std::move(terms)) {}

    explicit Query(const std::string &rawQuery);

    bool operator==(const Query &o) const;

    std::ostream &print(std::ostream &os, bool typeInfo) const;

    friend std::ostream &operator<<(std::ostream &os, const Query &l);
};

} // fovris
#endif /* end of include guard: QUERY_H_2UGKWPIJ */
