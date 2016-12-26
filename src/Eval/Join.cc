#include <numeric>

#include "Eval/Join.h"

namespace fovris {

namespace {

std::set<unsigned> ToIndices(const KBRuleLiteral &rel) {
    using std::begin;
    using std::end;
    std::vector<unsigned> indices(rel.getTerms().size());
    std::iota(begin(indices), end(indices), 0);
    return {begin(indices), end(indices)};
}
}

void Join::makeEqualIndices(const JoinInfo &infoA, const JoinInfo &infoB) {
    auto const &varsIndicesA = infoA.rel.getVarIndices();
    auto const &varsIndicesB = infoB.rel.getVarIndices();

    for (auto itVarsA = varsIndicesA.begin(); itVarsA != varsIndicesA.end();
         itVarsA++) {
        auto itVarsB = varsIndicesB.find(itVarsA->first);
        if (itVarsB == varsIndicesB.end()) {
            continue;
        }

        for (unsigned aIdx : itVarsA->second) {
            for (unsigned bIdx : itVarsB->second) {
                if (Contains(infoA.includedIndices, aIdx) &&
                    Contains(infoB.includedIndices, bIdx)) {
                    equalIndices.push_back({aIdx, bIdx});
                    leftIndices.push_back(aIdx);
                    rightIndices.push_back(bIdx);
                }
            }
        }
    }
}

JoinInfo::JoinInfo(const KBRuleLiteral &rel, std::set<unsigned> includedIndices)
    : rel(rel), includedIndices(includedIndices) {}

JoinInfo::JoinInfo(const KBRuleLiteral &rel)
    : rel(rel), includedIndices(ToIndices(rel)) {}
} // fovris
