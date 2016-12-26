#ifndef KBRULE_H_NDFAQSBE
#define KBRULE_H_NDFAQSBE

#include "KB/KBHeadLiteral.h"
#include "KB/KBRuleDisjunct.h"
namespace fovris {


class KBRule {
    KBHeadLiteral head;
    std::vector<KBRuleDisjunct> disjuncts;

  public:
    KBRule(KBHeadLiteral head, std::vector<KBRuleDisjunct> disjuncts)
        : head(std::move(head)), disjuncts(std::move(disjuncts)) {}

    KBRule(KBHeadLiteral head, std::vector<KBRuleLiteral> literals)
        : head(std::move(head)), disjuncts({KBRuleDisjunct{std::move(literals)}}) {}

    bool hasDisjunction() const { return disjuncts.size() > 1; }

    decltype(head) &getHead() { return head; }
    decltype(disjuncts) &getDisjuncts() { return disjuncts; }
    const decltype(head) &getHead() const { return head; }
    const decltype(disjuncts) &getDisjuncts() const { return disjuncts; }
};
} // fovris
#endif /* end of include guard: KBRULE_H_NDFAQSBE */
