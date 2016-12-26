#ifndef SELECTION_H_NSBREW3Z
#define SELECTION_H_NSBREW3Z

#include <vector>

#include "KB/KBRuleLiteral.h"
#include "KB/KBGroundTerm.h"
#include "TruthValueSet.h"

namespace fovris {

struct Selection {
    Selection(const KBRuleLiteral &kbr)
        : attrConstraint(makeAttributeConstraint(kbr))
        , valConstraint(makeValueConstraint(kbr))
        , valueSet(kbr.getLogicSet()) {}

    const std::vector<std::vector<unsigned>> attrConstraint;
    const std::vector<std::pair<unsigned, KBGroundTerm>> valConstraint;
    const TruthValueSet valueSet;

    bool selectable(const Array<KBGroundTerm> &terms) const {
        if (checkAttrConstraints(terms) && checkValConstrains(terms)) {
            return true;
        }

        return false;
    }

    bool matchesTruthValue(TruthValue value) const {
        return checkTruthValueConstraints(value);
    }

    bool hasUnknown() const { return valueSet.hasUnknown(); }

    bool hasOnlyUnknown() const {
        return valueSet.hasUnknown() && valueSet.size() == 1;
    }

  private:
    bool checkTruthValueConstraints(TruthValue tValue) const {
        if (valueSet.isEmpty()) {
            return true;
        }

        // has only unknown, no need to check the rest
        if (valueSet.size() == 1 && valueSet.hasUnknown()) {
            return false;
        }

        return AnyOf(valueSet.getSet(),
                     [&tValue](TruthValue val) { return val == tValue; });
    }
    bool checkAttrConstraints(const Array<KBGroundTerm> &terms) const {
        for (auto const &cVec : attrConstraint) {
            if (cVec.size() <= 1) {
                continue;
            }
            auto &eqTerm = terms[cVec.front()];
            bool pass = AllOf(cVec, [&terms, &eqTerm](unsigned idx) {
                return terms[idx] == eqTerm;
            });

            if (!pass) {
                return false;
            }
        }
        return true;
    }

    bool checkValConstrains(const Array<KBGroundTerm> &terms) const {
        return AllOf(valConstraint,
                     [&terms](const std::pair<unsigned, KBGroundTerm> &valC) {
            return terms[valC.first] == valC.second;
        });
    }

    decltype(attrConstraint) makeAttributeConstraint(const KBRuleLiteral &kbr) {
        auto attrConstraint = decltype(this->attrConstraint){};
        auto &varIndices = kbr.getVarIndices();
        using MapValue = const std::pair<KBTerm, std::vector<unsigned>>;
        Transform(varIndices, attrConstraint,
                  [](MapValue &p) { return p.second; });
        return attrConstraint;
    }

    decltype(valConstraint) makeValueConstraint(const KBRuleLiteral &kbr) {
        auto valConstraint = decltype(this->valConstraint){};

        int i = 0;
        for (auto &term : kbr.getTerms()) {
            if (!term.isVariable()) {
                valConstraint.emplace_back(i, KBGroundTerm(term.get()));
            }
            i++;
        }
        return valConstraint;
    }
};
} // fovris
#endif /* end of include guard: SELECTION_H_NSBREW3Z */
