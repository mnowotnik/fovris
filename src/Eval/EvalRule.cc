#include <cassert> //TODO: move to cc

#include "boost/multi_index/identity.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/multi_index/random_access_index.hpp"
#include "boost/multi_index_container.hpp"

#include "Eval/EvalRule.h"
#include "make_unique.hpp"
#include <ostream>
namespace fovris {
namespace {
using namespace ::boost;
using namespace ::boost::multi_index;

using EvalSubgoalPtr = std::shared_ptr<EvalSubgoal>;

struct TermTag {};

class OrderedVarMapEntry {
  public:
    OrderedVarMapEntry(KBTerm term, unsigned count)
        : term(term), count(count) {}
    KBTerm term;
    unsigned count;
};

typedef multi_index_container<
    OrderedVarMapEntry,
    indexed_by<random_access<>,
               ordered_unique<tag<TermTag>, member<OrderedVarMapEntry, KBTerm,
                                                   &OrderedVarMapEntry::term>>>>
    OrderedVarMap;

std::vector<KBTerm> GetVarSet(const OrderedVarMap &varMap) {
    std::vector<KBTerm> set;
    for (auto &entry : varMap) {
        if (entry.count > 0) {
            set.push_back(entry.term);
        }
    }
    return set;
}

template <typename T> std::vector<T> ToVector(const Array<T> &array) {
    return {array.begin(), array.end()};
}

OrderedVarMap &AddToRuleVars(OrderedVarMap &ruleVars,
                             const std::vector<KBTerm> &varsSet) {
    auto &termIndex = ruleVars.get<TermTag>();
    for (auto &var : varsSet) {
        auto it = termIndex.find(var);
        if (it == termIndex.end()) {
            termIndex.insert({var, 1});
        } else {
            auto count = it->count + 1;
            termIndex.modify(it, [&count](OrderedVarMapEntry &entry) {
                entry.count = count;
            });
        }
    }
    return ruleVars;
}

OrderedVarMap &RemoveFromRuleVars(OrderedVarMap &ruleVars,
                                  const std::vector<KBTerm> &varsSet) {
    auto &termIndex = ruleVars.get<TermTag>();
    for (auto &var : varsSet) {
        auto it = termIndex.find(var);
        if (it == termIndex.end()) {
            continue;
        }

        auto count = it->count - 1;

        if (count > 0) {
            termIndex.modify(it, [&count](OrderedVarMapEntry &entry) {
                entry.count = count;
            });
        }
    }
    return ruleVars;
}

OrderedVarMap MakeVarsCounter(const std::vector<KBRuleLiteral> &literals,
                              const KBLiteral &head) {

    OrderedVarMap allRuleVars;
    AddToRuleVars(allRuleVars, head.getVarSet());
    for (auto &literal : literals) {
        AddToRuleVars(allRuleVars, literal.getVarSet());
    }

    return allRuleVars;
}

class EvaluateRuleWithoutDisjunction : public EvalRule {
    std::vector<EvalSubgoalPtr> subgoalEvals;
    KBRuleDisjunct ruleDisjunct_;
    KBHeadLiteral head_;
    KBRule rule_;
    KBRuleLiteral resultRelation_;
    std::reference_wrapper<const TermMapper> termMapper_;

  public:
    EvaluateRuleWithoutDisjunction(KBRule rule, const TermMapper &termMapper)
        : ruleDisjunct_(rule.getDisjuncts().front())
        , head_(rule.getHead())
        , rule_(rule)
        , termMapper_(termMapper) {
        auto &firstSubgoal = ruleDisjunct_.getBody().front();

        if (ruleDisjunct_.getBody().size() == 1) {
            subgoalEvals.emplace_back(CreateEvalSubgoalFirst(
                firstSubgoal, EvalDestination(!rule_.getHead().isNegated(),
                                              rule_.getHead().getTerms())));

            resultRelation_ = subgoalEvals.back()->getResultRelation();
            return;
        }

        auto varsCounter = MakeVarsCounter(ruleDisjunct_.getBody(), head_);
        RemoveFromRuleVars(varsCounter, firstSubgoal.getVarSet());

        subgoalEvals.emplace_back(CreateEvalSubgoalFirst(
            firstSubgoal, EvalDestination{GetVarSet(varsCounter)}));

        auto tmpRelation = subgoalEvals.back()->getResultRelation();

        for (auto &subgoal : Range(ruleDisjunct_.getBody(), 1,
                                   ruleDisjunct_.getBody().size() - 1)) {

            RemoveFromRuleVars(varsCounter, subgoal.getVarSet());

            auto varsSet = GetVarSet(varsCounter);

            subgoalEvals.emplace_back(
                CreateEvalSubgoalNext(tmpRelation, subgoal, varsSet));

            tmpRelation = subgoalEvals.back()->getResultRelation();
        }

        //TODO functions could evaluated ASAP to minimize number of tuples

        subgoalEvals.emplace_back(CreateEvalSubgoalLast(
            tmpRelation, ruleDisjunct_.getBody().back(),
            EvalDestination{!head_.isNegated(), head_.getTerms()},
            ruleDisjunct_.getFunctions(), termMapper_.get()));

        tmpRelation = subgoalEvals.back()->getResultRelation();

        resultRelation_ = std::move(tmpRelation);
    }

    const KBRule &getRule() const { return rule_; }

    const KBRuleLiteral &getResultRelation() { return resultRelation_; }

    std::vector<KBTuple> operator()(
        std::vector<std::reference_wrapper<const FactsPool>> relations) {
        assert(subgoalEvals.size() == relations.size());
        std::vector<KBTuple> tmpTable;
        for (size_t i = 0; i < subgoalEvals.size(); i++) {
            tmpTable = subgoalEvals[i]->evaluate(std::move(tmpTable),
                                                 relations[i].get());
        }

        return tmpTable;
    }
};

OrderedVarMap MakeVarsCounter(const KBLiteral &head,
                              const std::vector<KBRuleDisjunct> &disjuncts) {

    OrderedVarMap allRuleVars;
    AddToRuleVars(allRuleVars, head.getVarSet());
    for (auto &disjunct : disjuncts) {
        for (auto &literal : disjunct.getBody()) {
            AddToRuleVars(allRuleVars, literal.getVarSet());
        }
    }
    return allRuleVars;
}

std::vector<KBTerm> MakeVarSet(const std::vector<KBRuleLiteral> &literals) {
    std::vector<KBTerm> vars;
    std::set<KBTerm> varSet;
    using std::end;
    using std::begin;

    for (auto &literal : literals) {
        auto literalVarsIndices = GetOrderedUniqueVarIndices(literal);
        for (auto idx : literalVarsIndices) {
            auto &literalVar = literal.getTerms()[idx];
            if (!Contains(varSet, literalVar)) {
                vars.push_back(literalVar);
            } else {
                varSet.insert(literalVar);
            }
        }
    }
    return vars;
}

class EvalRuleWithDisj : public EvalRule {
    KBRule rule_;
    std::vector<EvaluateRuleWithoutDisjunction> disjunctEvaluations;
    std::vector<EvalSubgoalPtr> disjunctResultEvals;
    unsigned literalsNum_ = 0;
    std::reference_wrapper<const TermMapper> termMapper_;

  public:
    EvalRuleWithDisj(KBRule rule, const TermMapper &mapper)
        : rule_(std::move(rule)), termMapper_(mapper) {
        auto ruleVarsCounter =
            MakeVarsCounter(rule_.getHead(), rule_.getDisjuncts());


        KBRuleLiteral curTmpRel;
        auto disjIt = rule_.getDisjuncts().begin();
        for (; disjIt != rule_.getDisjuncts().end(); disjIt++) {
            auto &disjunct = (*disjIt);
            literalsNum_ += disjunct.getBody().size();

            RemoveFromRuleVars(ruleVarsCounter, MakeVarSet(disjunct.getBody()));

            disjunctEvaluations.emplace_back(
                KBRule{KBHeadLiteral{0, GetVarSet(ruleVarsCounter)},
                       {std::vector<KBRuleDisjunct>{disjunct}}},
                termMapper_.get());

            auto disjResultRel = disjunctEvaluations.back().getResultRelation();

            if (disjIt == rule_.getDisjuncts().begin()) {
                curTmpRel = disjResultRel;
                // dummy eval basically
                // skipped during evaluation
                disjunctResultEvals.push_back(CreateEvalSubgoalNextDisjunction(
                    curTmpRel, disjResultRel,
                    EvalDestination{GetVarSet(ruleVarsCounter)}));

            } else if (disjIt == rule_.getDisjuncts().end() - 1) {

                auto disjResultEval = CreateEvalSubgoalNextDisjunction(
                    curTmpRel, disjResultRel,
                    EvalDestination{!rule_.getHead().isNegated(),
                                    rule_.getHead().getTerms()});
                curTmpRel = disjResultEval->getResultRelation();
                disjunctResultEvals.push_back(disjResultEval);
            } else {
                auto disjResultEval = CreateEvalSubgoalNextDisjunction(
                    curTmpRel, disjResultRel,
                    EvalDestination{GetVarSet(ruleVarsCounter)});

                disjunctResultEvals.push_back(disjResultEval);
                curTmpRel = disjResultEval->getResultRelation();
            }
        }
    }

    const KBRule &getRule() const { return rule_; }

    /**
     * Call operator for evaluating the rule.
     *
     * @param relations vector of relations which should correspond to every literal in the rule.
     *
     * @return generated tuples
     */
    std::vector<KBTuple> operator()(
        std::vector<std::reference_wrapper<const FactsPool>> relations) {
        if (relations.size() != literalsNum_) {
            throw AssertionException(
                "The number of relations is incorrect. Should be ",
                literalsNum_);
        }


        std::vector<KBTuple> tmpResult;
        auto relationsIt = std::begin(relations);
        auto evalIt = std::begin(disjunctEvaluations);
        auto disjunctResultEvalIt = disjunctResultEvals.begin();
        for (auto &disjunct : rule_.getDisjuncts()) {
            auto disjunctLiteralsNum = disjunct.getBody().size();
            // relations to be injected into subgoal evaluator
            decltype(relations) disjunctRelations;
            disjunctRelations.reserve(disjunctLiteralsNum);
            disjunctRelations.insert(std::begin(disjunctRelations), relationsIt,
                                     relationsIt + disjunctLiteralsNum);

            auto disjunctResult = (*evalIt)(std::move(disjunctRelations));

            if (disjunctResultEvalIt == std::begin(disjunctResultEvals)) {
                tmpResult = std::move(disjunctResult);
            } else {
                tmpResult =
                    (*disjunctResultEvalIt)
                        ->evaluate(std::move(tmpResult), disjunctResult);
            }
            std::advance(relationsIt, disjunctLiteralsNum);
            evalIt++;
            disjunctResultEvalIt++;
        }
        return tmpResult;
    }
};


} // namespace

std::unique_ptr<EvalRule> CreateEvalRule(KBRule rule,
                                         const TermMapper &mapper) {

    if (rule.hasDisjunction()) {
        return std::make_unique<EvalRuleWithDisj>(rule, mapper);
    } else {
        return std::make_unique<EvaluateRuleWithoutDisjunction>(rule, mapper);
    }
}
} // fovris
