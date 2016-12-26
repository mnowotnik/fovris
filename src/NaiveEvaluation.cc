#include "NaiveEvaluation.h"
#include "make_unique.hpp"
#include <algorithm>
#include <unordered_map>
#include <utility>

#include "Eval/EvalRule.h"
namespace fovris {

namespace {

/*struct AddTrueResultsToRelationRemoveIncons {
    const std::map<unsigned, FactsPool> &prevIdbRelations;
    AddTrueResultsToRelationRemoveIncons(
        const std::map<unsigned, FactsPool> &prevIdbRelations)
        : prevIdbRelations(prevIdbRelations) {}

    void operator()(KnowledgeBase &knowledgeBase, unsigned relId,
                    std::vector<KBTuple> &results) {
        auto &kbRelation = knowledgeBase.getKBRelation(relId);
        for (auto &tup : results) {
            if (tup.getValue() != TruthValue::Incons) {
                kbRelation.addTuple(std::move(tup));
            }
        }
        kbRelation.clearIncons(prevIdbRelations.at(relId));
    }
};*/

struct EqualsIncons {
    bool operator()(TruthValue val) { return val == TruthValue::Incons; }
};

struct NotEqualsIncons {
    bool operator()(TruthValue val) { return val != TruthValue::Incons; }
};


template <typename ValuePred> struct AddResultsToRelation {
    ValuePred valuePred;
    AddResultsToRelation(ValuePred valuePred) : valuePred(valuePred) {}

    void operator()(KnowledgeBase &knowledgeBase, unsigned relId,
                    std::vector<KBTuple> &results) {
        auto &kbRelation = knowledgeBase.getKBRelation(relId);
        for (auto &tup : results) {
            if (valuePred(tup.getValue())) {
                kbRelation.addTuple(std::move(tup));
            }
        }
    }
};

template <typename AddToRelation>
void
    evaluateRules(KnowledgeBase &knowledgeBase,
                  const std::vector<std::unique_ptr<EvalRule>> &ruleEvaluations,
                  AddToRelation addToRelation, bool aggregate) {
    for (auto &evalRule : ruleEvaluations) {
        auto result =
            (*evalRule)(GetRelationRefs(knowledgeBase, evalRule->getRule()));
        if(aggregate){
            result = AggregateSortedRuleResult(std::move(result));
        }
        addToRelation(knowledgeBase, evalRule->getRule().getHead().getRelId(),
                      result);
    }
}

struct GenInfo {
    unsigned factsNumber = 0;
    unsigned inconsFactsNumber = 0;

    GenInfo() {}
    GenInfo(unsigned factsNumber, unsigned inconsFactsNumber)
        : factsNumber(factsNumber), inconsFactsNumber(inconsFactsNumber) {}

    bool operator!=(const GenInfo &other) {
        return factsNumber != other.factsNumber ||
               inconsFactsNumber != other.inconsFactsNumber;
    }
};

template <typename AddToRelation>
GenInfo generate(KnowledgeBase &knowledgeBase, const KBModule &kbModule,
                 const std::vector<std::unique_ptr<EvalRule>> &ruleEvaluations,
                 AddToRelation addToRelation, bool aggregate) {
    unsigned prevFactsNumber;
    unsigned prevInconsFactsNumber;
    unsigned factsNumber = kbModule.factsNumber();
    unsigned inconsFactsNumber = kbModule.inconsFactsNumber();

    do {
        prevFactsNumber = factsNumber;
        prevInconsFactsNumber = inconsFactsNumber;

        evaluateRules(knowledgeBase, ruleEvaluations, addToRelation, aggregate);

        factsNumber = kbModule.factsNumber();
        inconsFactsNumber = kbModule.inconsFactsNumber();
    } while (factsNumber != prevFactsNumber ||
             inconsFactsNumber != prevInconsFactsNumber);

    return GenInfo{factsNumber, inconsFactsNumber};
}

void evaluateKBModule(KnowledgeBase &knowledgeBase, const KBModule &kbModule) {
    std::vector<std::unique_ptr<EvalRule>> ruleEvaluations;

    for (auto &kbRule : kbModule.rules) {
        ruleEvaluations.emplace_back(
            CreateEvalRule(kbRule, knowledgeBase.getTermMapper()));
    }

    generate(knowledgeBase, kbModule, ruleEvaluations,
             AddResultsToRelation<NotEqualsIncons>(NotEqualsIncons{}), false);

    // no incons facts in module - we can end
    if (kbModule.inconsFactsNumber() == 0) {
        return;
    }

    GenInfo genTrueInfoPrev, genInconsInfoPrev;
    GenInfo genTrueInfo, genInconsInfo;

    // correct errors caused by inconsistencies if any
    do {
        genTrueInfoPrev = genTrueInfo;
        genInconsInfoPrev = genInconsInfo;
        std::map<unsigned, FactsPool> prevIdbRelations;

        // backup current idb relations
        // set current idb relations to original facts
        // remove facts which are incons in current model
        for (auto idbRelId : kbModule.idbRelationIds) {
            auto &kbRelation = knowledgeBase.getKBRelation(idbRelId);
            FactsPool &facts = kbRelation.getFacts();
            prevIdbRelations.insert(std::make_pair(idbRelId, std::move(facts)));
            kbRelation.clear();
            kbRelation.clearIncons(prevIdbRelations[idbRelId]);
        }

        genTrueInfo =
            generate(knowledgeBase, kbModule, ruleEvaluations,
                     AddResultsToRelation<NotEqualsIncons>(NotEqualsIncons{}), false);

        // add incons facts to idb relations
        for (auto idbRelId : kbModule.idbRelationIds) {
            auto &kbRelation = knowledgeBase.getKBRelation(idbRelId);
            kbRelation.addIncons(prevIdbRelations[idbRelId]);
        }

        genInconsInfo =
            generate(knowledgeBase, kbModule, ruleEvaluations,
                     AddResultsToRelation<EqualsIncons>(EqualsIncons{}), true);

    } while (genTrueInfo != genTrueInfoPrev ||
             genInconsInfo != genInconsInfoPrev);
}
}

void NaiveEvaluation::evaluateKB(KnowledgeBase &knowledgeBase) {
    for (auto const &module : knowledgeBase.getKBModules()) {
        evaluateKBModule(knowledgeBase, module);
    }
}
} // fovris
