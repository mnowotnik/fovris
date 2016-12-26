#include "QueryResult.h"
namespace fovris {


std::ostream &operator<<(std::ostream &os, const QueryResult &q) {
    return q.print(os, 0);
}

std::ostream &QueryResult::print(std::ostream &os, int indent) const {
    for (const auto &f : facts) {
        os << std::string(indent, ' ') << query.predicateSymbol << f << std::endl;
    }
    return os;
}

} // fovris

