#include "KB/KBTuple.h"
#include <ostream>

namespace fovris {


bool KBTuple::operator<(const KBTuple &o) const {
    return std::tie(terms, value) < std::tie(o.terms, o.value);
}

bool KBTuple::operator>(const KBTuple &o) const {
    return std::tie(terms, value) > std::tie(o.terms, o.value);
}
bool KBTuple::operator==(const KBTuple &o) const {
    return std::tie(terms, value) == std::tie(o.terms, o.value);
}
bool KBTuple::operator!=(const KBTuple &o) const {
    return std::tie(terms, value) != std::tie(o.terms, o.value);
}


std::ostream &operator<<(std::ostream &os, const KBTuple &tup) {
    os << "KBTuple:";
    for (auto &term : tup.getTerms()) {
        os << term.get() << ' ';
    }
    os << tup.getValue();
    return os;
}
}
