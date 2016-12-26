#include "GroundLiteral.h"
namespace fovris {


void GroundLiteral::validate() {
    for (auto &term : getTerms()) {
        if (IsVar(term)) {
            throw InvalidTypeException(
                "A fact cannot have variables as terms.");
        }
    }
}

std::ostream &operator<<(std::ostream &os, const GroundLiteral &l) {
    return l.print(os, false);
}

} // fovris
