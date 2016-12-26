#include "Literal.h"
namespace fovris {

Literal::Literal(std::string p, std::vector<Term> terms)
    : Literal(true, std::move(p), std::move(terms)) {}

Literal::Literal(bool isTrue, std::string p, std::vector<Term> terms)
    : isTrue_(isTrue), predicateSymbol(std::move(p)), terms(std::move(terms)) {}

bool Literal::operator==(const Literal &o) const {
    return predicateSymbol == o.predicateSymbol && isTrue_ == o.isTrue_ &&
           terms == o.terms;
}

std::ostream &operator<<(std::ostream &os, const Literal &l) {
    return l.print(os, false);
}

std::ostream &Literal::print(std::ostream &os, bool typeInfo) const {
    if (!isTrue_) {
        os << '!';
    }
    os << predicateSymbol << '(';

    for (auto it = terms.begin(); it != terms.end(); it++) {
        if (it > terms.begin()) {
            os << ',';
        }
        it->print(os, typeInfo);
    }

    os << ')';
    return os;
}
} // fovris
