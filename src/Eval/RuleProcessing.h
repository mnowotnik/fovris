#ifndef RULEPROCESSING_H_F1TQ9GJX
#define RULEPROCESSING_H_F1TQ9GJX

#include <vector>

#include "KB/KBRuleLiteral.h"
namespace fovris {


/** Sort literals so the rule can be evaluated safely. The safe literals must be
 * evaluated before unsafe */
std::vector<KBRuleLiteral>
    SortRuleLiterals(std::vector<KBRuleLiteral> evalOrder);

/** Creates a vector of relation ids from safe literals in a rule's body.
 * Relation ids preserve the order of corresponding literals. */
std::vector<unsigned>
    getRelIdsFromSafeLiterals(const std::vector<KBRuleLiteral> &ruleBody);


} // fovris
#endif /* end of include guard: RULEPROCESSING_H_F1TQ9GJX */
