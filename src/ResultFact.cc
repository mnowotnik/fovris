#include <iomanip>

#include "ResultFact.h"
namespace fovris {

std::ostream &operator<<(std::ostream &os, const ResultFact &q) {
    std::stringstream ss;
    ss << '(';
    const auto &terms = q.terms;

    for (auto it = terms.begin(); it != terms.end(); it++) {
        if (it > terms.begin()) {
            ss << ',';
        }
        it->print(ss, false);
    }

    ss << ")";
    os << std::left << std::setw(15) << ss.str();
    os << std::right << ": " << q.value;
    return os;
}

} // fovris
