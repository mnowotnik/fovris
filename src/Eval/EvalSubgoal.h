#ifndef EVALSUBGOAL_H_OTEQWYA6
#define EVALSUBGOAL_H_OTEQWYA6

#include <vector>

#include "TermMapper.h"
#include "Eval/Selection.h"
#include "Eval/Projection.h"
#include "Eval/Join.h"
#include "Eval/JoinProjection.h"
#include "KB/KBRuleFunction.h"
#include "KB/KBRelation.h"
#include "PredicateFunction.h"
namespace fovris {

class EvalSubgoal {
  public:
    virtual std::vector<KBTuple>
        evaluate(std::vector<KBTuple>&& currentRelation,
                 const FactsPool &facts) = 0;

    virtual std::vector<KBTuple>
        evaluate(std::vector<KBTuple>&& currentRelation,
                 const std::vector<KBTuple> &addedRelation) = 0;

    virtual const KBRuleLiteral &getResultRelation() = 0;
    virtual ~EvalSubgoal() {}
};


struct EvalDestination{
    bool resultIsTrue;
    std::vector<KBTerm>projectTo;
    EvalDestination(bool resultIsTrue,std::vector<KBTerm> projectTo);
    EvalDestination(std::vector<KBTerm> projectTo);
    EvalDestination(std::initializer_list<KBTerm> projectTo);
};

/* Evaluation of the first literal in a rule
 *
 * Head(X,Y) <- B1(X,Y,Z) ^ B2(X,Y,Z)
 *-----------------↑
 */
std::shared_ptr<EvalSubgoal>
    CreateEvalSubgoalFirst(const KBRuleLiteral &kbr,
                           const EvalDestination &dest);

/* Evaluation of a literal beside first in a rule
 *
 * Head(X,Y) <- B1(X,Y,Z) ^ B2(X,Y,Z) ^ ...
 *----------------------------↑
 */
std::shared_ptr<EvalSubgoal>
    CreateEvalSubgoalNext(const KBRuleLiteral &curTmpRel,
                          const KBRuleLiteral &newRel,
                          const std::vector<KBTerm> &projectTo);


/* Evaluation of a literal beside first in a rule
 *
 * Head(X,Y) <- B1(X,Y,Z) | B2(X,Y,Z)
 *----------------------------↑
 */
std::shared_ptr<EvalSubgoal>
    CreateEvalSubgoalNextDisjunction(const KBRuleLiteral &curTmpRel,
                                     const KBRuleLiteral &newRel,
                                     const EvalDestination& dest);

/* Evaluation of the last literal and remaining functions
 *
 * Head(X,Y) <- B1(X,Y,Z) ^ B2(X,Y,Z) ^ X<Y ^ Z=X
 *----------------------------↑
 */
std::shared_ptr<EvalSubgoal> CreateEvalSubgoalLast(
    const KBRuleLiteral &curTmpRel, const KBRuleLiteral &newRel,
    const EvalDestination &dest,
    const std::vector<KBRuleFunction> functions, const TermMapper &termMapper);
} // fovris
#endif /* end of include guard: EVALSUBGOAL_H_OTEQWYA6 */
