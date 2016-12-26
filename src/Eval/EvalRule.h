#ifndef EVALRULE_H_KDUA2FKJ
#define EVALRULE_H_KDUA2FKJ
#include <vector>

#include "KB/KBRelation.h"
#include "KB/KBRule.h"
#include "Eval/EvalSubgoal.h"
namespace fovris {


class EvalRule {
  public:
    EvalRule() {}
    virtual std::vector<KBTuple> operator()(
        std::vector<std::reference_wrapper<const FactsPool>> relations) = 0;
    virtual const KBRule& getRule() const = 0;
    virtual ~EvalRule() {}
};

std::unique_ptr<EvalRule> CreateEvalRule(KBRule rule,const TermMapper& termMapper={});

} // fovris
#endif /* end of include guard: EVALRULE_H_KDUA2FKJ */
