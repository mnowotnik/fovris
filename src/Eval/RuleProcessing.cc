#include "Eval/RuleProcessing.h"
namespace fovris {

std::vector<KBRuleLiteral>
    SortRuleLiterals(std::vector<KBRuleLiteral> evalOrder) {

    auto it =
        std::find_if(evalOrder.begin(), evalOrder.end(),
                     [](const KBRuleLiteral &rl) { return rl.isUnsafe(); });
    // no unsafe return as is
    if (it == evalOrder.end()) {
        return evalOrder;
    }

    std::sort(evalOrder.begin(), evalOrder.end(),
              [](const KBRuleLiteral &a, const KBRuleLiteral &b) {
        if (a.isUnsafe() && b.isUnsafe()) {
            return a.getVarSet().size() > b.getVarSet().size();
        }

        if (a.isUnsafe()) {
            if (HaveCommonVariables(a, b)) {
                return false;
            } else {
                return true;
            }
        }

        if (b.isUnsafe()) {
            if (HaveCommonVariables(a, b)) {
                return true;
            } else {
                return false;
            }
        }

        return a.getVarSet().size() > b.getVarSet().size();
    });
    return evalOrder;
}

std::vector<unsigned>
    getRelIdsFromSafeLiterals(const std::vector<KBRuleLiteral> &ruleBody) {
    std::vector<unsigned> relationIds;
    for (auto &rl : ruleBody) {
        if (!rl.isUnsafe()) {
            relationIds.push_back(rl.getRelId());
        }
    }
    return relationIds;
}
} // fovris
