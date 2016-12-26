#include <algorithm>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <functional>
#include <algorithm>

#include "Eval/EvalRule.h"
#include "Eval/Utils.h"
#include "KB/KBRelation.h"
#include "SemiNaiveEvaluation.h"
#include "make_unique.hpp"

namespace fovris {
namespace {

/*struct AddTrueResultsToRelationRemoveIncons {
    std::reference_wrapper<const std::map<unsigned, FactsPool>>
        prevIdbRelations;
    AddTrueResultsToRelationRemoveIncons(
        const std::map<unsigned, FactsPool> &prevIdbRelations)
        : prevIdbRelations(prevIdbRelations) {}
    void operator()(std::vector<KBTuple> &&result, KnowledgeBase &kb,
                    const std::unique_ptr<EvalRule> &evalRule,
                    std::map<unsigned, FactsPool> &relationDeltas) {
        unsigned headRelId = evalRule->getRule().getHead().getRelId();
        auto &deltaRelation = relationDeltas[headRelId];
        auto addToDeltaRelation = AddToDeltaRelation{deltaRelation};
        auto &relation = kb.getKBRelation(headRelId);
        for (auto &fact : result) {
            if (fact.getValue() == TruthValue::Incons) {
                continue;
            }

            auto addInfo = relation.addTuple(const_cast<const KBTuple &>(fact));
            if (addInfo.first) {
                addToDeltaRelation(std::move(fact), addInfo.second);
            }
        }
        relation.clearIncons(prevIdbRelations.get().at(headRelId));
    }
};*/

struct EqualsIncons {
    bool operator()(TruthValue val) { return val == TruthValue::Incons; }
};

struct NotEqualsIncons {
    bool operator()(TruthValue val) { return val != TruthValue::Incons; }
};

struct AddToDeltaRelation {
    FactsPool::nth_index<0>::type &randomAcessIdx;
    AddToDeltaRelation(FactsPool &deltaRel)
        : randomAcessIdx(deltaRel.get<0>()) {}

    void operator()(KBTuple &&tup, TruthValue currentVal) {
        TruthValue factVal = tup.getValue();
        auto retIt = randomAcessIdx.push_back(std::move(tup));
        if (currentVal != factVal) {
            randomAcessIdx.modify(retIt.first, [currentVal](KBTuple &tup) {
                tup.setValue(currentVal);
            });
        }
    }
};

struct ProcResultIdentity {
    std::vector<KBTuple> operator()(std::vector<KBTuple> &&result) {
        return result;
    }
};

struct ProcResultAggregate {
    std::vector<KBTuple> operator()(std::vector<KBTuple> &&result) {
        return AggregateSortedRuleResult(std::move(result));
    }
};


template <typename ValuePred, typename ProcResult> struct AddResultsToRelation {
    ValuePred valuePred;
    ProcResult procResult;
    AddResultsToRelation(ValuePred valuePred, ProcResult procResult)
        : valuePred(valuePred), procResult(procResult) {}

    void operator()(std::vector<KBTuple> &&result, KnowledgeBase &kb,
                    const std::unique_ptr<EvalRule> &evalRule,
                    std::map<unsigned, FactsPool> &relationDeltas) {
        result = procResult(std::move(result));
        unsigned headRelId = evalRule->getRule().getHead().getRelId();
        auto &deltaRelation = relationDeltas[headRelId];
        auto addToDeltaRelation = AddToDeltaRelation{deltaRelation};
        auto &relation = kb.getKBRelation(headRelId);
        for (auto &fact : result) {
            if (!valuePred(fact.getValue())) {
                continue;
            }

            auto addInfo = relation.addTuple(const_cast<const KBTuple &>(fact));
            if (addInfo.first) {
                addToDeltaRelation(std::move(fact), addInfo.second);
            }
        }
    }
};

std::map<unsigned, FactsPool> createRelationDeltas(const KBModule &module) {
    std::map<unsigned, FactsPool> relationDeltas;
    for (auto &relId : module.relationIds) {
        relationDeltas[relId] = FactsPool{};
    }
    return relationDeltas;
}

struct IdbInfo {
    bool isIdb = false;
    unsigned relId = 0;

    IdbInfo() {}
    IdbInfo(bool isIdb, unsigned relId) : isIdb(isIdb), relId(relId) {}
};

// First round of rule evaluations where deltas are populated
template <typename AddToRelation>
void firstPass(KnowledgeBase &kb, std::map<unsigned, FactsPool> &relationDeltas,
               const std::vector<std::unique_ptr<EvalRule>> &ruleEvaluations,
               AddToRelation addToRelation) {
    for (auto &evalRule : ruleEvaluations) {
        auto result = (*evalRule)(GetRelationRefs(kb, evalRule->getRule()));
        addToRelation(std::move(result), kb, evalRule, relationDeltas);
    }
}

template <typename AddToRelation>
void
    evaluateRules(KnowledgeBase &kb, const KBModule &kbm,
                  std::map<unsigned, FactsPool> &relationDeltas,
                  const std::vector<std::unique_ptr<EvalRule>> &ruleEvaluations,
                  const std::vector<std::vector<IdbInfo>> &rulesIdbInfo,
                  AddToRelation addToRelation, bool naively) {

    std::map<unsigned, FactsPool> newDeltaRelations = createRelationDeltas(kbm);
    for (size_t i = 0; i < ruleEvaluations.size(); i++) {
        auto &evalRule = ruleEvaluations[i];
        auto relationRefs = GetRelationRefs(kb, evalRule->getRule());

        // the rule contains disjunction operators, it can be only be evaluated
        // naively
        if (evalRule->getRule().getDisjuncts().size() > 1 || naively) {
            auto result = (*evalRule)(relationRefs);
            addToRelation(std::move(result), kb, evalRule, newDeltaRelations);
            continue;
        }

        std::vector<KBTuple> finalResult;
        for (size_t j = 0; j < rulesIdbInfo[i].size(); j++) {
            auto &ruleDbInfo = rulesIdbInfo[i][j];
            bool isLiteralIdb = ruleDbInfo.isIdb;
            // the literal is not in IDB or the delta is empty
            if (!isLiteralIdb || relationDeltas[ruleDbInfo.relId].empty()) {
                continue;
            }

            auto relationRefsCopy = relationRefs;

            // replace one relation ref with  delta relation
            relationRefsCopy[j] = std::cref(relationDeltas[ruleDbInfo.relId]);

            // evaluate rule with relation delta and relations
            auto result = (*evalRule)(relationRefsCopy);

            std::move(std::begin(result), std::end(result),
                      std::back_inserter(finalResult));
        }

        // add results to KB and create new relation deltas
        addToRelation(std::move(finalResult), kb, evalRule, newDeltaRelations);
    }
    relationDeltas = std::move(newDeltaRelations);
}

bool areDeltasEmpty(const std::map<unsigned, FactsPool> &relationDeltas) {
    for (auto &entry : relationDeltas) {
        if (!entry.second.empty()) {
            return false;
        }
    }
    return true;
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
                 const std::vector<std::vector<IdbInfo>> &rulesIdbInfo,
                 AddToRelation addToRelation, bool naively) {

    auto relationDeltas = createRelationDeltas(kbModule);
    firstPass(knowledgeBase, relationDeltas, ruleEvaluations, addToRelation);

    if (areDeltasEmpty(relationDeltas)) {
        return GenInfo{kbModule.factsNumber(), kbModule.inconsFactsNumber()};
    }

    do {
        evaluateRules(knowledgeBase, kbModule, relationDeltas, ruleEvaluations,
                      rulesIdbInfo, addToRelation, naively);
    } while (!areDeltasEmpty(relationDeltas));

    return GenInfo{kbModule.factsNumber(), kbModule.inconsFactsNumber()};
}


std::vector<std::vector<IdbInfo>> generateRuleIdbInfo(const KBModule &kbm) {

    std::vector<std::vector<IdbInfo>> rulesIdbInfo;
    rulesIdbInfo.reserve(kbm.rules.size());
    std::set<unsigned> idbRelIds;
    for (auto &rule : kbm.rules) {
        idbRelIds.insert(rule.getHead().getRelId());
    }

    for (auto &rule : kbm.rules) {
        std::vector<IdbInfo> ruleIdbInfo;
        for (auto &disjunct : rule.getDisjuncts()) {
            for (auto &ruleLiteral : disjunct.getBody()) {
                ruleIdbInfo.push_back(
                    IdbInfo{Contains(idbRelIds, ruleLiteral.getRelId()),
                            ruleLiteral.getRelId()});
            }
        }
        rulesIdbInfo.push_back(ruleIdbInfo);
    }
    return rulesIdbInfo;
}

void evaluateKBModule(KnowledgeBase &kb, const KBModule &kbm) {
    auto relationDeltas = createRelationDeltas(kbm);

    std::vector<std::unique_ptr<EvalRule>> ruleEvaluations;

    for (auto &kbr : kbm.rules) {
        ruleEvaluations.emplace_back(CreateEvalRule(kbr, kb.getTermMapper()));
    }

    auto rulesIdbInfo = generateRuleIdbInfo(kbm);

    generate(kb, kbm, ruleEvaluations, rulesIdbInfo,
             AddResultsToRelation<NotEqualsIncons, ProcResultIdentity>{
                 NotEqualsIncons{}, ProcResultIdentity{}},
             false);

    // no incons facts in module - we can end
    if (kbm.inconsFactsNumber() == 0) {
        return;
    }


    GenInfo genTrueInfoPrev, genInconsInfoPrev;
    GenInfo genTrueInfo, genInconsInfo;
    do {
        genTrueInfoPrev = genTrueInfo;
        genInconsInfoPrev = genInconsInfo;
        std::map<unsigned, FactsPool> prevIdbRelations;

        // backup current idb relations
        // set current idb relations to original facts
        // remove facts which are incons in current model
        for (auto idbRelId : kbm.idbRelationIds) {
            auto &kbRelation = kb.getKBRelation(idbRelId);
            FactsPool &facts = kbRelation.getFacts();
            prevIdbRelations.insert(std::make_pair(idbRelId, std::move(facts)));
            kbRelation.clear();
            kbRelation.clearIncons(prevIdbRelations[idbRelId]);
        }

        genTrueInfo =
            generate(kb, kbm, ruleEvaluations, rulesIdbInfo,
                     AddResultsToRelation<NotEqualsIncons, ProcResultIdentity>{
                         NotEqualsIncons{}, ProcResultIdentity{}},
                     false);

        // add incons facts to idb relations
        for (auto idbRelId : kbm.idbRelationIds) {
            auto &kbRelation = kb.getKBRelation(idbRelId);
            kbRelation.addIncons(prevIdbRelations[idbRelId]);
        }

        genInconsInfo =
            generate(kb, kbm, ruleEvaluations, rulesIdbInfo,
                     AddResultsToRelation<EqualsIncons, ProcResultAggregate>{
                         EqualsIncons{}, ProcResultAggregate{}},
                     true);

    } while (genTrueInfo != genTrueInfoPrev ||
             genInconsInfo != genInconsInfoPrev);
}
}
void SemiNaiveEvaluation::evaluateKB(KnowledgeBase &kb) {
    for (auto &module : kb.getKBModules()) {
        evaluateKBModule(kb, module);
    }
}
} // fovris
