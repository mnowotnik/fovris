#ifndef RULE_H_8VOJQLJI
#define RULE_H_8VOJQLJI
#include "RuleDisjunct.h"
#include "RuleLiteral.h"
#include <initializer_list>
namespace fovris {
class Rule {
    Literal head;
    std::vector<RuleDisjunct> body;

  public:
    Literal &getHead();
    const Literal &getHead() const;
    std::vector<RuleDisjunct> &getDisjuncts();
    const std::vector<RuleDisjunct> &getDisjuncts() const;

    Rule(Literal head, std::vector<RuleDisjunct> body);
    Rule(Literal head, std::initializer_list<RuleLiteral> body);

    friend std::ostream &operator<<(std::ostream &os, const Rule &r);
    std::ostream &print(std::ostream &os, bool verbose) const;
};
} // fovris;
#endif /* end of include guard: RULE_H_8VOJQLJI */
