#include "RelationDef.h"
namespace fovris {

std::ostream &operator<<(std::ostream &os, const RelationDef &r) {
    os << r.predicateSymbol << "(";

    for (auto t = r.types.begin(); t != r.types.end(); t++) {
        if (t > r.types.begin()) {
            os << ',';
        }
        os << *t;
    }
    os << ')';
    return os;
}
} // fovris
