#include "Eval/EvalSubgoal.h"
#include "KB/KBLiteral.h"

#include "Utils.h"

#if defined(_OPENMP)
#include "Eval/omp_mergesort.h"
#include <omp.h>
#endif

#include <algorithm>

namespace fovris {


namespace {
namespace ph = std::placeholders;

const unsigned MtFactNumThreshold = 512;
#if defined(_OPENMP)
const unsigned MaxThreadsNumber = omp_get_max_threads();
#endif

template <class Container, class V = typename Container::value_type>
std::set<V> ConvertToSet(const Container &container) {
    return std::set<V>{container.begin(), container.end()};
}

using EvalSubgoalPtr = std::shared_ptr<EvalSubgoal>;

typedef TruthValue (*LogicBinOp)(TruthValue a, TruthValue b);

struct KBTupleRef {
    const Array<KBGroundTerm> *terms;
    TruthValue value;

  public:
    KBTupleRef() : terms(nullptr) {}
    KBTupleRef(const Array<KBGroundTerm> &terms, TruthValue value)
        : terms(&terms), value(value) {}
    const Array<KBGroundTerm> &getTerms() const { return *terms; }
    decltype(value) getValue() const { return value; }

    bool operator<(const KBTupleRef &o) const {
        return std::tie(*terms, value) < std::tie(*o.terms, o.value);
    }

    bool operator>(const KBTupleRef &o) const {
        return std::tie(*terms, value) > std::tie(*o.terms, o.value);
    }
    bool operator==(const KBTupleRef &o) const {
        return std::tie(*terms, value) == std::tie(*o.terms, o.value);
    }
    bool operator!=(const KBTupleRef &o) const {
        return std::tie(*terms, value) != std::tie(*o.terms, o.value);
    }
};

bool AllOfPredicateFunctions(
    const std::vector<JoinProjection> &functionJoinProjections,
    const std::vector<KBRuleFunction> &functions, const TermMapper &termMapper,
    const Array<KBGroundTerm> &tmpFact, const Array<KBGroundTerm> &selFact,
    std::vector<Array<Term>> &preallocatedFuncProjections) {

    auto funJoinProjIt = functionJoinProjections.begin();
    auto funTermsIt = preallocatedFuncProjections.begin();

    for (auto &kbfunction : functions) {
        auto &termTypes = kbfunction.getTermTypes();
        auto &function = kbfunction.getFunction();
        auto funProjected = funJoinProjIt->project(tmpFact, selFact);
        funJoinProjIt++;

        auto &funTerms = *funTermsIt;
        funTermsIt++;

        auto termTypeIt = termTypes.begin();
        auto i = 0;
        for (auto &kbTerm : funProjected) {
            funTerms[i] = termMapper.queryTerm(kbTerm.get(), *termTypeIt);
            termTypeIt++;
            i++;
        }

        if (!function(funTerms)) {
            return false;
        }
    }
    return true;
}


struct SelectPredicate {
    Selection &selection;

    SelectPredicate(Selection &selection) : selection(selection) {}
    bool operator()(const KBTuple &fact) {
        return fact.getValue() != TruthValue::Unknown &&
               selection.selectable(fact.getTerms());
    }
};

struct SelectPredicateWithValueMatch {
    Selection &selection;

    SelectPredicateWithValueMatch(Selection &selection)
        : selection(selection) {}
    bool operator()(const KBTuple &fact) {
        return fact.getValue() != TruthValue::Unknown &&
               selection.selectable(fact.getTerms()) &&
               selection.matchesTruthValue(fact.getValue());
    }
};

template <typename Container, typename Predicate>
std::vector<std::pair<KBTupleRef, bool>>
    selectKBTupleRefAndMatch(Container &facts, Selection &selection) {
    std::vector<std::pair<KBTupleRef, bool>> curTmpTable;
    Predicate pred(selection);

#if defined(_OPENMP)
    if (facts.size() < MtFactNumThreshold || MaxThreadsNumber <= 2) {
#endif
        // selection
        for (auto &fact : facts) {
            if (pred(fact)) {
                curTmpTable.emplace_back(
                    KBTupleRef{fact.getTerms(),
                               ResolveToTrueOrIncons(fact.getValue())},
                    selection.matchesTruthValue(fact.getValue()));
            }
        }

        return curTmpTable;
#if defined(_OPENMP)
    }
    std::vector<int> startIndexes(MaxThreadsNumber);
#pragma omp parallel for schedule(static) default(shared)
    for (unsigned i = 0; i < facts.size(); i++) {
        if (pred(facts[i])) {
            startIndexes[omp_get_thread_num()] += 1;
        }
    }
    size_t rsize = 0;
    for (size_t i = 0; i < MaxThreadsNumber; i++) {
        rsize += startIndexes[i];
        if (i > 0) {
            startIndexes[i] += startIndexes[i - 1];
        }
    }

    std::vector<KBTuple> rTable(rsize);
    curTmpTable.resize(rsize);

#pragma omp parallel default(shared)
    {
        unsigned j = omp_get_thread_num() > 0
                         ? startIndexes[omp_get_thread_num() - 1]
                         : 0;
#pragma omp for schedule(static)
        for (unsigned i = 0; i < facts.size(); i++) {
            if (pred(facts[i])) {
                curTmpTable[j] = std::make_pair(
                    KBTupleRef{facts[i].getTerms(),
                               ResolveToTrueOrIncons(facts[i].getValue())},
                    selection.matchesTruthValue(facts[i].getValue()));
                j++;
            }
        }
    }
#endif
    return curTmpTable;
}

template <typename Container, typename Predicate>
std::vector<KBTupleRef> selectKBTupleRef(Container &facts,
                                         Selection &selection) {
    std::vector<KBTupleRef> curTmpTable;
    Predicate pred(selection);

#if defined(_OPENMP)
    if (facts.size() < MtFactNumThreshold || MaxThreadsNumber <= 2) {
#endif
        // selection
        for (auto &fact : facts) {
            if (pred(fact)) {
                curTmpTable.emplace_back(KBTupleRef{
                    fact.getTerms(), ResolveToTrueOrIncons(fact.getValue())});
            }
        }

        return curTmpTable;
#if defined(_OPENMP)
    }
    std::vector<size_t> startIndexes(MaxThreadsNumber);
#pragma omp parallel for schedule(static) default(shared)
    for (unsigned i = 0; i < facts.size(); i++) {
        if (pred(facts[i])) {
            startIndexes[omp_get_thread_num()] += 1;
        }
    }
    size_t rsize = 0;
    for (size_t i = 0; i < MaxThreadsNumber; i++) {
        rsize += startIndexes[i];
        if (i > 0) {
            startIndexes[i] += startIndexes[i - 1];
        }
    }

    std::vector<KBTuple> rTable(rsize);
    curTmpTable.resize(rsize);

#pragma omp parallel default(shared)
    {
        unsigned j = omp_get_thread_num() > 0
                         ? startIndexes[omp_get_thread_num() - 1]
                         : 0;
#pragma omp for schedule(static)
        for (unsigned i = 0; i < facts.size(); i++) {
            if (pred(facts[i])) {
                curTmpTable[j] =
                    KBTupleRef{facts[i].getTerms(),
                               ResolveToTrueOrIncons(facts[i].getValue())};
                j++;
            }
        }
    }
#endif
    return curTmpTable;
}

template <typename T> struct TermsMapper {
    const Array<KBGroundTerm> &operator()(const T &t) { return t.getTerms(); }
};
template <> struct TermsMapper<std::pair<KBTupleRef, bool>> {
    const Array<KBGroundTerm> &
        operator()(const std::pair<KBTupleRef, bool> &t) {
        return t.first.getTerms();
    }
};

template <> struct TermsMapper<KBTuple *> {
    const Array<KBGroundTerm> &operator()(KBTuple *t) { return t->getTerms(); }
};

template <> struct TermsMapper<KBTupleRef *> {
    const Array<KBGroundTerm> &operator()(KBTupleRef *t) {
        return t->getTerms();
    }
};

template <> struct TermsMapper<std::pair<KBTupleRef, bool> *> {
    const Array<KBGroundTerm> &operator()(std::pair<KBTupleRef, bool> *t) {
        return t->first.getTerms();
    }
};

template <typename T> struct TupleCompareFunctor {
    TermsMapper<T> mapper;
    std::reference_wrapper<const std::vector<unsigned>> indices;

    TupleCompareFunctor(const std::vector<unsigned> &indices)
        : indices(std::cref(indices)) {}

    bool operator()(const T &tupA, const T &tupB) {
        auto it = indices.get().begin();
        auto &termsA = mapper(tupA);
        auto &termsB = mapper(tupB);
        for (; it != indices.get().end(); it++) {
            if (termsA[*it] != termsB[*it]) {
                break;
            }
        }

        if (it == indices.get().end()) {
            return false;
        }

        if (termsA[*it] < termsB[*it]) {
            return true;
        }

        return false;
    }
};

template <typename T>
void sortRelation(std::vector<T> &relation,
                  const std::vector<unsigned> &indices) {
#if not defined(_OPENMP)
    std::sort(relation.begin(), relation.end(),
              TupleCompareFunctor<T>(indices));
#else
    if (relation.size() < MtFactNumThreshold) {
        std::sort(relation.begin(), relation.end(),
                  TupleCompareFunctor<T>(indices));
    } else {
        std::vector<T *> relationToSort;
        relationToSort.reserve(relation.size());
        auto relationData = relation.data();
        for (size_t i = 0; i < relation.size(); i++) {
            relationToSort.emplace_back(&relationData[i]);
        }

        std::vector<T *> tmpTable(relation.size());
        mergesort<T *, TupleCompareFunctor<T *>>(
            relationToSort.data(), relationToSort.size(), tmpTable.data(),
            MaxThreadsNumber, TupleCompareFunctor<T *>(indices));

        std::vector<T> newRel;
        newRel.reserve(relation.size());
        auto ptrIt = relationToSort.begin();
        while (ptrIt != relationToSort.end()) {
            newRel.push_back(std::move(**ptrIt));
            ptrIt++;
        }
        relation = std::move(newRel);
    }
#endif
}
} // namespace

EvalDestination::EvalDestination(bool isTrue, std::vector<KBTerm> projectTo)
    : resultIsTrue(isTrue), projectTo(std::move(projectTo)) {}
EvalDestination::EvalDestination(std::vector<KBTerm> projectTo)
    : resultIsTrue(true), projectTo(std::move(projectTo)) {}

EvalDestination::EvalDestination(std::initializer_list<KBTerm> projectTo)
    : EvalDestination(std::vector<KBTerm>(projectTo)) {}

/* Evaluation of the first literal in a rule
 *
 * Head(X,Y) <- B1(X,Y,Z) ^ B2(X,Y,Z)
 *-----------------↑
 */
class EvalSubgoalFirst : public EvalSubgoal {
    Selection selection;
    Projection projection;

    KBRuleLiteral resultRelation;
    bool resultIsTrue;

  public:
    EvalSubgoalFirst(const KBRuleLiteral &kbr, const EvalDestination &dest)
        : selection(kbr)
        , projection(kbr, dest.projectTo)
        , resultRelation(projection.project(kbr.getTerms()),
                         {TruthValue::True, TruthValue::Incons})
        , resultIsTrue(dest.resultIsTrue) {}

    const decltype(resultRelation) &getResultRelation() {
        return resultRelation;
    }

    using EvalSubgoal::evaluate;

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&,
                                  const FactsPool &facts) {
        return doEvaluate(facts);
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&,
                                  const std::vector<KBTuple> &relation) {
        return doEvaluate(relation);
    }

  private:
    template <typename Container>
    std::vector<KBTuple> doEvaluate(Container &facts) {

#if defined(_OPENMP)
        if (facts.size() < MtFactNumThreshold || MaxThreadsNumber <= 2) {
#endif
            std::vector<KBTuple> rTable;

            // select
            for (auto const &fact : facts) {
                if (selection.selectable(fact.getTerms()) &&
                    selection.matchesTruthValue(fact.getValue()) &&
                    fact.getValue() != TruthValue::Unknown) {
                    rTable.emplace_back(
                        projection.project(fact.getTerms()),
                        ResolveTruthValue(fact.getValue(), resultIsTrue));
                }
            }
            return rTable;

#if defined(_OPENMP)
        }

        std::vector<size_t> startIndexes(MaxThreadsNumber);
#pragma omp parallel for schedule(static) default(shared)
        for (size_t i = 0; i < facts.size(); i++) {
            if (selection.selectable(facts[i].getTerms()) &&
                selection.matchesTruthValue(facts[i].getValue()) &&
                facts[i].getValue() != TruthValue::Unknown) {
                startIndexes[omp_get_thread_num()] += 1;
            }
        }

        size_t rsize = 0;
        for (size_t i = 0; i < MaxThreadsNumber; i++) {
            rsize += startIndexes[i];
            if (i > 0) {
                startIndexes[i] += startIndexes[i - 1];
            }
        }
        std::vector<KBTuple> rTable(rsize);
        assert(rsize <= facts.size());
#pragma omp parallel default(shared)
        {
            size_t j = omp_get_thread_num() > 0
                           ? startIndexes[omp_get_thread_num() - 1]
                           : 0;
#pragma omp for schedule(static)
            for (size_t i = 0; i < facts.size(); i++) {
                if (selection.selectable(facts[i].getTerms()) &&
                    selection.matchesTruthValue(facts[i].getValue()) &&
                    facts[i].getValue() != TruthValue::Unknown) {
                    rTable[j] = KBTuple(
                        projection.project(facts[i].getTerms()),
                        ResolveTruthValue(facts[i].getValue(), resultIsTrue));
                    j++;
                }
            }
        }
        return rTable;
#endif
    }
};

/* Evaluation of a literal beside first in a rule
 * with functions in a rule
 *
 * Ex.:
 *
 * Head(X,Y) <- B1(X,Y,Z) ^ B2(X,Y,Z) ^ ... ^ X<Y ^ Z>X
 *----------------------------↑
 */
class EvalSubgoalNextWithFunctions : public EvalSubgoal {
    Selection selection;
    Join join;
    JoinProjection joinProjection;
    const TermMapper termMapper;
    std::vector<JoinProjection> functionJoinProjections;
    std::vector<KBRuleFunction> functions;
    std::vector<Array<Term>> preallocatedFuncProjections;

    std::function<bool(const Array<KBGroundTerm> &, const Array<KBGroundTerm> &,
                       std::vector<Array<Term>> &)> allFunctionsMatch;

    bool resultIsTrue;
    KBRuleLiteral resultRelation;

  public:
    EvalSubgoalNextWithFunctions(const KBRuleLiteral &curTmpRel,
                                 const KBRuleLiteral &newRel,
                                 const EvalDestination &dest,
                                 std::vector<KBRuleFunction> functions,
                                 const TermMapper &termMapper)
        : selection(newRel)
        , join(curTmpRel,
               {newRel, ConvertToSet(GetOrderedUniqueVarIndices(newRel))})
        , joinProjection(curTmpRel, newRel, dest.projectTo)
        , termMapper(termMapper)
        , functions(std::move(functions))
        , resultIsTrue(dest.resultIsTrue)
        , resultRelation(
              joinProjection.project(curTmpRel.getTerms(), newRel.getTerms()),
              {mapBoolToTruthValue(dest.resultIsTrue), TruthValue::Incons}) {
        for (auto &function : this->functions) {
            functionJoinProjections.emplace_back(curTmpRel, newRel,
                                                 function.getTerms());
            preallocatedFuncProjections.emplace_back(
                function.getTerms().size());
        }

        allFunctionsMatch =
            std::bind(AllOfPredicateFunctions, functionJoinProjections,
                      this->functions, termMapper, ph::_1, ph::_2, ph::_3);
    }

    const decltype(resultRelation) &getResultRelation() {
        return resultRelation;
    }

    using EvalSubgoal::evaluate;

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const FactsPool &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const std::vector<KBTuple> &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

  private:
    template <typename Container>
    std::vector<KBTuple> doEvaluate(std::vector<KBTuple> &&tmpTable,
                                    Container &relation) {
        // Select facts matching column constraints
        // In the second argument of the std::pair place true if fact matches
        // truth value constraint
        //
        std::vector<std::pair<KBTupleRef, bool>> curTmpTable =
            selectKBTupleRefAndMatch<Container, SelectPredicate>(relation,
                                                                 selection);

        sortRelation(tmpTable, join.getLeftRelIndices());
        sortRelation(curTmpTable, join.getRightRelIndices());

        std::vector<KBTuple> newTmpTable;
        // merge join
        auto tmpBeg = curTmpTable.begin();
        auto tmpIt = tmpTable.begin();
        auto tmpEnd = tmpTable.end();
        auto curIt = curTmpTable.begin();
        auto curEnd = curTmpTable.end();

        while (tmpIt != tmpEnd && curIt != curEnd) {
            auto result = join.compareWithoutNull(tmpIt->getTerms(),
                                                  curIt->first.getTerms());
            // tmp is greater
            if (result == 1) {
                // next cur tuple
                curIt++;
                // cur is greater
            } else if (result == -1) {
                // tmpFact was not joined and there is unknown in the logic set
                // constraint of the current literal
                if (selection.hasUnknown() &&
                    (curIt != tmpBeg &&
                     !join.joinableWithoutNull(
                         tmpIt->getTerms(), (curIt - 1)->first.getTerms())) &&
                    allFunctionsMatch(tmpIt->getTerms(), {},
                                      preallocatedFuncProjections)) {

                    // perform projection for NOT join
                    newTmpTable.emplace_back(
                        joinProjection.leftComplement(tmpIt->getTerms()),
                        ResolveTruthValue(tmpIt->getValue(), resultIsTrue));
                }

                // next tmp tuple
                tmpIt++;
                // equal
            } else {
                if (selection.hasOnlyUnknown()) {
                    curIt++;
                    tmpIt++;
                    continue;
                }

                if (allFunctionsMatch(tmpIt->getTerms(),
                                      curIt->first.getTerms(),
                                      preallocatedFuncProjections) &&
                    curIt->second) {
                    newTmpTable.emplace_back(
                        joinProjection.project(tmpIt->getTerms(),
                                               curIt->first.getTerms()),
                        ResolveTruthValue(Conjunction(tmpIt->getValue(),
                                                      curIt->first.getValue()),
                                          resultIsTrue));
                }

                auto tmpItNext = tmpIt + 1;
                while (tmpItNext != tmpEnd &&
                       join.joinable(tmpItNext->getTerms(),
                                     curIt->first.getTerms())) {
                    if (allFunctionsMatch(tmpItNext->getTerms(),
                                          curIt->first.getTerms(),
                                          preallocatedFuncProjections) &&
                        curIt->second) {
                        newTmpTable.emplace_back(
                            joinProjection.project(tmpItNext->getTerms(),
                                                   curIt->first.getTerms()),
                            ResolveTruthValue(
                                Conjunction(tmpItNext->getValue(),
                                            curIt->first.getValue()),
                                resultIsTrue));
                    }

                    tmpItNext++;
                }

                auto curItNext = curIt + 1;
                while (curItNext != curEnd &&
                       join.joinable(tmpIt->getTerms(),
                                     curItNext->first.getTerms())) {

                    if (allFunctionsMatch(tmpIt->getTerms(),
                                          curItNext->first.getTerms(),
                                          preallocatedFuncProjections) &&
                        curItNext->second) {
                        newTmpTable.emplace_back(
                            joinProjection.project(tmpIt->getTerms(),
                                                   curItNext->first.getTerms()),
                            ResolveTruthValue(
                                Conjunction(tmpIt->getValue(),
                                            curItNext->first.getValue()),
                                resultIsTrue));
                    }

                    curItNext++;
                }

                curIt++;
                tmpIt++;
            }
        }

        if (tmpIt == tmpEnd) {
            return newTmpTable;
        }

        if (!selection.hasUnknown()) {
            return newTmpTable;
        }

        // tmp relation still has not joined tuples and rule literal has
        // 'unknown' logical constraint

        if (curIt != tmpBeg) {
            // find first tmp tuple for which none tuples it can be joined with
            // exists in the cur table
            while (tmpIt != tmpEnd) {
                if ((join.joinableWithoutNull(tmpIt->getTerms(),
                                              (curIt - 1)->first.getTerms()))) {
                    tmpIt++;
                } else {
                    break;
                }
            }
        }

        for (; tmpIt != tmpEnd; tmpIt++) {
            if (allFunctionsMatch(tmpIt->getTerms(), {},
                                  preallocatedFuncProjections)) {
                newTmpTable.emplace_back(
                    joinProjection.leftComplement(tmpIt->getTerms()),
                    ResolveTruthValue(tmpIt->getValue(), resultIsTrue));
            }
        }
        return newTmpTable;
    }
};

/* Evaluation of a literal beside first in a rule
 *
 * Ex.:
 *
 * Head(X,Y) <- B1(X,Y,Z) ^ B2(X,Y,Z)
 *----------------------------↑
 */
class EvalSubgoalNext : public EvalSubgoal {
    Selection selection;
    Join join;
    JoinProjection joinProjection;
    bool resultIsTrue;
    KBRuleLiteral resultRelation;

  public:
    EvalSubgoalNext(const KBRuleLiteral &curTmpRel, const KBRuleLiteral &newRel,
                    const EvalDestination &dest)
        : selection(newRel)
        , join(curTmpRel,
               {newRel, ConvertToSet(GetOrderedUniqueVarIndices(newRel))})
        , joinProjection(curTmpRel, newRel, dest.projectTo)
        , resultIsTrue(dest.resultIsTrue)
        , resultRelation(
              joinProjection.project(curTmpRel.getTerms(), newRel.getTerms()),
              {mapBoolToTruthValue(dest.resultIsTrue), TruthValue::Incons}) {}

    const decltype(resultRelation) &getResultRelation() {
        return resultRelation;
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const FactsPool &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const std::vector<KBTuple> &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

  private:
    template <typename Container>
    std::vector<KBTuple> doEvaluate(std::vector<KBTuple> &&tmpTable,
                                    Container &relation) {

        std::vector<std::pair<KBTupleRef, bool>> curTmpTable =
            selectKBTupleRefAndMatch<Container, SelectPredicate>(relation,
                                                                 selection);

        sortRelation(tmpTable, join.getLeftRelIndices());
        sortRelation(curTmpTable, join.getRightRelIndices());

        std::vector<KBTuple> newTmpTable;

        auto tmpBeg = curTmpTable.begin();
        auto tmpIt = tmpTable.begin();
        auto tmpEnd = tmpTable.end();
        auto curIt = curTmpTable.begin();
        auto curEnd = curTmpTable.end();

        while (tmpIt != tmpEnd && curIt != curEnd) {
            auto result = join.compareWithoutNull(tmpIt->getTerms(),
                                                  curIt->first.getTerms());

            // tmp is greater
            if (result == 1) {
                // next cur tuple
                curIt++;
                // cur is greater
            } else if (result == -1) {
                // tmpFact was not joined and there is unknown in the logic set
                // constraint of the current literal
                if (selection.hasUnknown() &&
                    (curIt != tmpBeg &&
                     !join.joinableWithoutNull(
                         tmpIt->getTerms(), (curIt - 1)->first.getTerms()))) {

                    // perform projection for NOT join
                    newTmpTable.emplace_back(
                        joinProjection.leftComplement(tmpIt->getTerms()),
                        ResolveTruthValue(tmpIt->getValue(), resultIsTrue));
                }
                // next tmp tuple
                tmpIt++;

                // equal
            } else {
                if (selection.hasOnlyUnknown()) {
                    curIt++;
                    tmpIt++;
                    continue;
                }

                if (curIt->second) {
                    newTmpTable.emplace_back(
                        joinProjection.project(tmpIt->getTerms(),
                                               curIt->first.getTerms()),
                        ResolveTruthValue(Conjunction(tmpIt->getValue(),
                                                      curIt->first.getValue()),
                                          resultIsTrue));
                }
                auto tmpItNext = tmpIt + 1;
                while (tmpItNext != tmpEnd &&
                       join.joinable(tmpItNext->getTerms(),
                                     curIt->first.getTerms())) {
                    if (curIt->second) {
                        newTmpTable.emplace_back(
                            joinProjection.project(tmpItNext->getTerms(),
                                                   curIt->first.getTerms()),
                            ResolveTruthValue(
                                Conjunction(tmpItNext->getValue(),
                                            curIt->first.getValue()),
                                resultIsTrue));
                    }

                    tmpItNext++;
                }

                auto curItNext = curIt + 1;
                while (curItNext != curEnd &&
                       join.joinable(tmpIt->getTerms(),
                                     curItNext->first.getTerms())) {

                    if (curItNext->second) {
                        newTmpTable.emplace_back(
                            joinProjection.project(tmpIt->getTerms(),
                                                   curItNext->first.getTerms()),
                            ResolveTruthValue(
                                Conjunction(tmpIt->getValue(),
                                            curItNext->first.getValue()),
                                resultIsTrue));
                    }

                    curItNext++;
                }

                curIt++;
                tmpIt++;
            }
        }
        if (tmpIt == tmpEnd) {
            return newTmpTable;
        }

        if (!selection.hasUnknown()) {
            return newTmpTable;
        }

        // tmp relation still has not joined tuples and rule literal has
        // 'unknown' logical constraint

        if (curIt != tmpBeg) {
            // find first tmp tuple for which none tuples it can be joined with
            // exists in the cur table
            while (tmpIt != tmpEnd) {
                if ((join.joinableWithoutNull(tmpIt->getTerms(),
                                              (curIt - 1)->first.getTerms()))) {
                    tmpIt++;
                } else {
                    break;
                }
            }
        }

        for (; tmpIt != tmpEnd; tmpIt++) {
            newTmpTable.emplace_back(
                joinProjection.leftComplement(tmpIt->getTerms()),
                ResolveTruthValue(tmpIt->getValue(), resultIsTrue));
        }
        return newTmpTable;
    }
};

/* Evaluation of a literal beside first in a rule
 *
 * Optimized version for literals without the 'in {unknown}' clause.
 *
 * Head(X,Y) <- B1(X,Y,Z) ^ B2(X,Y,Z)
 *----------------------------↑
 */
class EvalSubgoalNextWithoutUnknown : public EvalSubgoal {
    Selection selection;
    Join join;
    JoinProjection joinProjection;

    bool resultIsTrue;
    KBRuleLiteral resultRelation;

  public:
    EvalSubgoalNextWithoutUnknown(const KBRuleLiteral &curTmpRel,
                                  const KBRuleLiteral &newRel,
                                  const EvalDestination &dest)
        : selection(newRel)
        , join(curTmpRel,
               {newRel, ConvertToSet(GetOrderedUniqueVarIndices(newRel))})
        , joinProjection(curTmpRel, newRel, dest.projectTo)
        , resultIsTrue(dest.resultIsTrue)
        , resultRelation(
              joinProjection.project(curTmpRel.getTerms(), newRel.getTerms()),
              {mapBoolToTruthValue(dest.resultIsTrue), TruthValue::Incons}) {}

    const decltype(resultRelation) &getResultRelation() {
        return resultRelation;
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const FactsPool &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const std::vector<KBTuple> &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

    template <typename Container>
    std::vector<KBTuple> doEvaluate(std::vector<KBTuple> &&tmpTable,
                                    Container &relation) {
        std::vector<KBTupleRef> curTmpTable =
            selectKBTupleRef<Container, SelectPredicateWithValueMatch>(
                relation, selection);

        sortRelation(tmpTable, join.getLeftRelIndices());
        sortRelation(curTmpTable, join.getRightRelIndices());

        std::vector<KBTuple> newTmpTable;
        // merge join
        auto tmpIt = tmpTable.begin();
        auto tmpEnd = tmpTable.end();
        auto curIt = curTmpTable.begin();
        auto curEnd = curTmpTable.end();

        while (tmpIt != tmpEnd && curIt != curEnd) {
            auto result = join.compare(tmpIt->getTerms(), curIt->getTerms());
            if (result == 1) {
                curIt++;
            } else if (result == -1) {
                tmpIt++;
            } else {

                newTmpTable.emplace_back(
                    joinProjection.project(tmpIt->getTerms(),
                                           curIt->getTerms()),
                    ResolveTruthValue(
                        Conjunction(tmpIt->getValue(), curIt->getValue()),
                        resultIsTrue));

                auto tmpItNext = tmpIt + 1;
                while (tmpItNext != tmpEnd &&
                       join.joinableWithoutNull(tmpItNext->getTerms(),
                                                curIt->getTerms())) {

                    newTmpTable.emplace_back(
                        joinProjection.project(tmpItNext->getTerms(),
                                               curIt->getTerms()),
                        ResolveTruthValue(Conjunction(tmpItNext->getValue(),
                                                      curIt->getValue()),
                                          resultIsTrue));
                    tmpItNext++;
                }

                auto curItNext = curIt + 1;
                while (curItNext != curEnd &&
                       join.joinableWithoutNull(tmpIt->getTerms(),
                                                curItNext->getTerms())) {

                    newTmpTable.emplace_back(
                        joinProjection.project(tmpIt->getTerms(),
                                               curItNext->getTerms()),
                        ResolveTruthValue(Conjunction(tmpIt->getValue(),
                                                      curItNext->getValue()),
                                          resultIsTrue));
                    curItNext++;
                }

                curIt++;
                tmpIt++;
            }
        }
        return newTmpTable;
    }
};

class EvalSubgoalNextWithoutUnknownWithFunctions : public EvalSubgoal {
    Selection selection;
    Join join;
    JoinProjection joinProjection;
    const TermMapper termMapper;
    std::vector<JoinProjection> functionJoinProjections;
    std::vector<KBRuleFunction> functions;
    std::vector<Array<Term>> preallocatedFuncProjections;

    std::function<bool(const Array<KBGroundTerm> &, const Array<KBGroundTerm> &,
                       std::vector<Array<Term>> &)> allFunctionsMatch;

    bool resultIsTrue;
    KBRuleLiteral resultRelation;

  public:
    EvalSubgoalNextWithoutUnknownWithFunctions(
        const KBRuleLiteral &curTmpRel, const KBRuleLiteral &newRel,
        const EvalDestination &dest, std::vector<KBRuleFunction> functions,
        const TermMapper &termMapper)
        : selection(newRel)
        , join(curTmpRel,
               {newRel, ConvertToSet(GetOrderedUniqueVarIndices(newRel))})
        , joinProjection(curTmpRel, newRel, dest.projectTo)
        , termMapper(termMapper)
        , functions(std::move(functions))
        , resultIsTrue(dest.resultIsTrue)
        , resultRelation(
              joinProjection.project(curTmpRel.getTerms(), newRel.getTerms()),
              {mapBoolToTruthValue(dest.resultIsTrue), TruthValue::Incons}) {
        for (auto &function : this->functions) {
            functionJoinProjections.emplace_back(curTmpRel, newRel,
                                                 function.getTerms());
            preallocatedFuncProjections.emplace_back(
                function.getTerms().size());
        }
        allFunctionsMatch =
            std::bind(AllOfPredicateFunctions, functionJoinProjections,
                      this->functions, termMapper, ph::_1, ph::_2, ph::_3);
    }

    const decltype(resultRelation) &getResultRelation() {
        return resultRelation;
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const FactsPool &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const std::vector<KBTuple> &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

  private:
    template <typename Container>
    std::vector<KBTuple> doEvaluate(std::vector<KBTuple> &&tmpTable,
                                    Container &relation) {
        std::vector<KBTupleRef> curTmpTable =
            selectKBTupleRef<Container, SelectPredicateWithValueMatch>(
                relation, selection);

        sortRelation(tmpTable, join.getLeftRelIndices());
        sortRelation(curTmpTable, join.getRightRelIndices());

        std::vector<KBTuple> newTmpTable;
        // merge join
        auto tmpIt = tmpTable.begin();
        auto tmpEnd = tmpTable.end();
        auto curIt = curTmpTable.begin();
        auto curEnd = curTmpTable.end();

        while (tmpIt != tmpEnd && curIt != curEnd) {
            auto result = join.compare(tmpIt->getTerms(), curIt->getTerms());
            if (result == 1) {
                curIt++;
            } else if (result == -1) {
                tmpIt++;
            } else {

                if (allFunctionsMatch(tmpIt->getTerms(), curIt->getTerms(),
                                      preallocatedFuncProjections)) {

                    newTmpTable.emplace_back(
                        joinProjection.project(tmpIt->getTerms(),
                                               curIt->getTerms()),
                        ResolveTruthValue(
                            Conjunction(tmpIt->getValue(), curIt->getValue()),
                            resultIsTrue));
                }


                auto tmpItNext = tmpIt + 1;
                while (tmpItNext != tmpEnd &&
                       join.joinableWithoutNull(tmpItNext->getTerms(),
                                                curIt->getTerms())) {

                    if (allFunctionsMatch(tmpItNext->getTerms(),
                                          curIt->getTerms(),
                                          preallocatedFuncProjections)) {
                        newTmpTable.emplace_back(
                            joinProjection.project(tmpItNext->getTerms(),
                                                   curIt->getTerms()),
                            ResolveTruthValue(Conjunction(tmpItNext->getValue(),
                                                          curIt->getValue()),
                                              resultIsTrue));
                    }

                    tmpItNext++;
                }

                auto curItNext = curIt + 1;
                while (curItNext != curEnd &&
                       join.joinableWithoutNull(tmpIt->getTerms(),
                                                curItNext->getTerms())) {

                    if (allFunctionsMatch(tmpIt->getTerms(),
                                          curItNext->getTerms(),
                                          preallocatedFuncProjections)) {
                        newTmpTable.emplace_back(
                            joinProjection.project(tmpIt->getTerms(),
                                                   curItNext->getTerms()),
                            ResolveTruthValue(
                                Conjunction(tmpIt->getValue(),
                                            curItNext->getValue()),
                                resultIsTrue));
                    }


                    curItNext++;
                }

                curIt++;
                tmpIt++;
            }
        }
        return newTmpTable;
    }
};

class EvalSubgoalNextDisjunction : public EvalSubgoal {
    Selection selection;
    Join join;
    JoinProjection joinProjection;

    bool resultIsTrue;
    KBRuleLiteral resultRelation;

  public:
    EvalSubgoalNextDisjunction(const KBRuleLiteral &curTmpRel,
                               const KBRuleLiteral &newRel,
                               const EvalDestination &dest)
        : selection(newRel)
        , join(curTmpRel,
               {newRel, ConvertToSet(GetOrderedUniqueVarIndices(newRel))})
        , joinProjection(curTmpRel, newRel, dest.projectTo)
        , resultIsTrue(dest.resultIsTrue)
        , resultRelation(
              joinProjection.project(curTmpRel.getTerms(), newRel.getTerms()),
              {mapBoolToTruthValue(resultIsTrue), TruthValue::Incons}) {}

    const decltype(resultRelation) &getResultRelation() {
        return resultRelation;
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const FactsPool &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

    std::vector<KBTuple> evaluate(std::vector<KBTuple> &&tmpTable,
                                  const std::vector<KBTuple> &relation) {
        return doEvaluate(std::move(tmpTable), relation);
    }

    template <typename Container>
    std::vector<KBTuple> doEvaluate(std::vector<KBTuple> &&tmpTable,
                                    Container &relation) {
        std::vector<KBTupleRef> curTmpTable =
            selectKBTupleRef<Container, SelectPredicateWithValueMatch>(
                relation, selection);
        sortRelation(tmpTable, join.getLeftRelIndices());
        sortRelation(curTmpTable, join.getRightRelIndices());
        std::vector<KBTuple> newTmpTable;
        std::vector<bool> curTmpTableAnyJoined(curTmpTable.size());

        // tmp is a temporary relation, used to aggregate results
        // cur is the currently processed relation from the KB
        // tmp is left relation , cur is right
        auto tmpIt = tmpTable.begin();
        auto tmpEnd = tmpTable.end();
        auto curIt = curTmpTable.begin();
        auto curEnd = curTmpTable.end();

        unsigned curIdx = 0;
        // merge join
        while (tmpIt != tmpEnd && curIt != curEnd) {
            auto result = join.compare(tmpIt->getTerms(), curIt->getTerms());
            // tmp is greater
            if (result == 1) {
                // next cur tuple
                curIt++;
                curIdx++;
                // cur is greater
            } else if (result == -1) {
                // perform left join
                newTmpTable.emplace_back(
                    joinProjection.leftComplement(tmpIt->getTerms()),
                    tmpIt->getValue());
                // next tmp tuple
                tmpIt++;
                // equal
            } else {
                newTmpTable.emplace_back(
                    joinProjection.project(tmpIt->getTerms(),
                                           curIt->getTerms()),
                    ResolveTruthValue(
                        Disjunction(tmpIt->getValue(), curIt->getValue()),
                        resultIsTrue));

                // mark the tuple from cur relation as joined
                curTmpTableAnyJoined[curIdx] = true;

                auto tmpItNext = tmpIt + 1;
                while (
                    tmpItNext != tmpEnd &&
                    join.joinable(tmpItNext->getTerms(), curIt->getTerms())) {
                    newTmpTable.emplace_back(
                        joinProjection.project(tmpItNext->getTerms(),
                                               curIt->getTerms()),
                        ResolveTruthValue(Disjunction(tmpItNext->getValue(),
                                                      curIt->getValue()),
                                          resultIsTrue));

                    tmpItNext++;
                }

                auto curItNext = curIt + 1;
                while (
                    curItNext != curEnd &&
                    join.joinable(tmpIt->getTerms(), curItNext->getTerms())) {
                    newTmpTable.emplace_back(
                        joinProjection.project(tmpIt->getTerms(),
                                               curItNext->getTerms()),
                        ResolveTruthValue(Disjunction(tmpIt->getValue(),
                                                      curItNext->getValue()),
                                          resultIsTrue));

                    curItNext++;
                }

                curIt++;
                curIdx++;
                tmpIt++;
            }
        }

        if (curTmpTable.empty()) {
            for (auto &tup : tmpTable) {

                newTmpTable.emplace_back(
                    joinProjection.leftComplement(tup.getTerms()),
                    tup.getValue());
            }
        }

        unsigned i = 0;
        for (auto curJoined : curTmpTableAnyJoined) {
            if (!curJoined) {
                newTmpTable.emplace_back(
                    joinProjection.righComplement(curTmpTable[i].getTerms()),
                    ResolveTruthValue(curTmpTable[i].getValue(), resultIsTrue));
            }
            i++;
        }

        return newTmpTable;
    }
};

EvalSubgoalPtr CreateEvalSubgoalFirst(const KBRuleLiteral &kbr,
                                      const EvalDestination &dest) {
    if (kbr.isUnsafe()) {
        throw AssertionException("First evaluation cannot be unsafe!");
    }
    return std::make_unique<EvalSubgoalFirst>(kbr, dest);
}

EvalSubgoalPtr CreateEvalSubgoalNext(const KBRuleLiteral &curTmpRel,
                                     const KBRuleLiteral &newRel,
                                     const std::vector<KBTerm> &projectTo) {
    if (curTmpRel.isUnsafe()) {
        throw AssertionException("Current tmp relation cannot be unsafe!");
    }

    if (newRel.isUnsafe()) {
        return std::make_shared<EvalSubgoalNext>(curTmpRel, newRel,
                                                 EvalDestination{projectTo});
    } else {
        return std::make_shared<EvalSubgoalNextWithoutUnknown>(
            curTmpRel, newRel, EvalDestination{projectTo});
    }
}

EvalSubgoalPtr CreateEvalSubgoalNextDisjunction(const KBRuleLiteral &curTmpRel,
                                                const KBRuleLiteral &newRel,
                                                const EvalDestination &dest) {
    if (curTmpRel.isUnsafe()) {
        throw AssertionException("Current tmp relation cannot be unsafe!");
    }
    return std::make_unique<EvalSubgoalNextDisjunction>(curTmpRel, newRel,
                                                        dest);
}


EvalSubgoalPtr CreateEvalSubgoalLast(const KBRuleLiteral &curTmpRel,
                                     const KBRuleLiteral &newRel,
                                     const EvalDestination &dest,
                                     std::vector<KBRuleFunction> functions,
                                     const TermMapper &termMapper) {
    if (functions.empty() && newRel.isUnsafe()) {
        return std::make_unique<EvalSubgoalNext>(curTmpRel, newRel, dest);
    } else if (functions.empty()) {
        return std::make_unique<EvalSubgoalNextWithoutUnknown>(curTmpRel,
                                                               newRel, dest);
    } else if (newRel.isUnsafe()) {
        return std::make_unique<EvalSubgoalNextWithFunctions>(
            curTmpRel, newRel, dest, std::move(functions), termMapper);
    } else {
        return std::make_unique<EvalSubgoalNextWithoutUnknownWithFunctions>(
            curTmpRel, newRel, dest, std::move(functions), termMapper);
    }
}
} // fovris
