#include "Evaluation.h"
#include "KB/KBRelation.h"

namespace fovris {
std::vector<std::reference_wrapper<const FactsPool>>
    GetRelationRefs(KnowledgeBase &kb, const KBRule &rule) {
    std::vector<std::reference_wrapper<const FactsPool>> result;
    for (auto &disjunct : rule.getDisjuncts()) {
        for (auto &literal : disjunct.getBody()) {
            result.emplace_back(kb.getKBRelation(literal.getRelId()).getFacts());
        }
    }
    return result;
}

// TODO remove
template <typename L>
static bool canBeUnified(const L &ql, const Array<KBGroundTerm> &tuple) {

    // check if for same vars in literal terms in ground literal are equal
    for (auto &pair : ql.getVarIndices()) {
        auto const &indices = pair.second;
        if (indices.size() <= 1) {
            continue;
        }
        unsigned zeroIndice = indices.front();
        bool varsEqualCond =
            AllOf(indices, [&ql, &tuple, zeroIndice](unsigned indice) {
                return tuple[indice].get() == ql.getTerms()[zeroIndice].get();
            });
        if (!varsEqualCond) {
            return false;
        }
    }

    // terms should be equal or be variables
    auto qlIt = ql.getTerms().begin();
    auto tupIt = tuple.begin();

    for (; qlIt != ql.getTerms().end() && tupIt != tuple.end();
         qlIt++, tupIt++) {
        if (qlIt->get() != tupIt->get()) {
            if (!qlIt->isVariable()) {
                return false;
            }
        }
    }

    return true;
}

// result is sorted cause of sort merge join
std::vector<KBTuple> AggregateSortedRuleResult(std::vector<KBTuple> result) {
    std::vector<KBTuple> aggregated;
    if (result.empty()) {
        return aggregated;
    }

    auto tupleToInsertIt = result.begin();
    TruthValue bestValue = tupleToInsertIt->getValue();
    for (auto it = result.begin() + 1; it != result.end(); it++) {
        if (it->getTerms() != tupleToInsertIt->getTerms()) {
            tupleToInsertIt->setValue(bestValue);
            aggregated.push_back(std::move(*tupleToInsertIt));
            tupleToInsertIt = it;
        } else if (it->getValue() != bestValue &&
                   it->getValue() != TruthValue::Incons) {
            bestValue = TruthValue::Incons;
        }
    }
    aggregated.push_back(std::move(*tupleToInsertIt));
    return aggregated;
}



std::vector<ResultFact> Evaluation::queryKB(const KBQuery &query,
                                                 const KnowledgeBase &kb) {
    const std::vector<TermType> colDomains =
        kb.getKBRelation(query.getRelId()).colDomains;
    std::vector<ResultFact> result;

    const std::string relName = query.getRelationName();
    for (auto &kbTup : kb.getKBRelation(query.getRelId()).getFacts()) {
        if (canBeUnified(query, kbTup.getTerms())) {
            result.push_back(
                kb.makeResultFact(kbTup.getTerms(), kbTup.getValue(), colDomains));
        }
    }

    return result;
}


} // fovris
