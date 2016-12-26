#include "Rule.h"
namespace fovris {

Literal &Rule::getHead() { return head; }
const Literal &Rule::getHead() const { return head; }
std::vector<RuleDisjunct> &Rule::getDisjuncts() { return body; }
const std::vector<RuleDisjunct> &Rule::getDisjuncts() const { return body; }

Rule::Rule(Literal head, std::vector<RuleDisjunct> body)
    : head(std::move(head)), body(std::move(body)) {}

Rule::Rule(Literal head, std::initializer_list<RuleLiteral> body)
    : head(std::move(head)), body({RuleDisjunct{body}}) {}

std::ostream &Rule::print(std::ostream &os, bool verbose) const {
    head.print(os, verbose);
    os << ":- ";
    auto it = body.begin();
    for (; it != body.end(); it++) {
        if (it != body.begin()) {
            os << '|';
        }
        it->print(os, verbose);
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const Rule &rl) {
    return rl.print(os, false);
}

} // fovris
