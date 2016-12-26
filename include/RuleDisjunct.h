#ifndef RULEDISJUNCT_H_NYZ24ZGV
#define RULEDISJUNCT_H_NYZ24ZGV
#include "RuleLiteral.h"
namespace fovris {
class RuleDisjunct {
    std::vector<RuleLiteral> literals;

  public:
    RuleDisjunct();
    RuleDisjunct(std::vector<RuleLiteral> literals);
    decltype(literals) &getBody();
    const decltype(literals) &getBody() const;
    friend std::ostream &operator<<(std::ostream &os, const RuleDisjunct &r);
    std::ostream &print(std::ostream &os, bool verbose) const;
};
} // fovris
#endif /* end of include guard: RULEDISJUNCT_H_NYZ24ZGV */
