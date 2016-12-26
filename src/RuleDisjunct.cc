#include "RuleDisjunct.h"
namespace fovris {

RuleDisjunct::RuleDisjunct() {}
RuleDisjunct::RuleDisjunct(std::vector<RuleLiteral> literals)
    : literals(std::move(literals)) {}

const std::vector<RuleLiteral> &RuleDisjunct::getBody() const { return literals; }
std::vector<RuleLiteral> &RuleDisjunct::getBody() { return literals; }

std::ostream &RuleDisjunct::print(std::ostream &os, bool verbose) const {
    auto it = literals.begin();
    for (; it != literals.end(); it++) {
        if (it != literals.begin()) {
            os << ',';
        }
        it->print(os, verbose);
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const RuleDisjunct &rl) {
    return rl.print(os, false);
}

} // fovris
