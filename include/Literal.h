#ifndef LITERAL_H_5SWEKXWR
#define LITERAL_H_5SWEKXWR

#include <vector>
#include <string>

#include "Term.h"
namespace fovris {

class Literal {
    bool isTrue_;
    std::string predicateSymbol;
    std::vector<Term> terms;

  public:
    Literal(std::string p, std::vector<Term> terms);
    Literal(bool isTrue, std::string p, std::vector<Term> terms);

    bool operator==(const Literal &o) const;

    bool isTrue() const { return isTrue_; }
    const std::string &getPredicateSymbol() const { return predicateSymbol; }
    const decltype(terms) &getTerms() const { return terms; }

    std::ostream &print(std::ostream &os, bool typeInfo) const;
    friend std::ostream &operator<<(std::ostream &os, const Literal &l);
};

inline bool IsFalse(const Literal& literal){
    return !literal.isTrue();
}

} // fovris
#endif /* end of include guard: LITERAL_H_5SWEKXWR */
