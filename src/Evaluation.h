#ifndef EVALUATION_H_YKTLWMNO
#define EVALUATION_H_YKTLWMNO
#include <memory>

#include "KnowledgeBase.h"
namespace fovris {

class Evaluation {
  public:
    virtual void evaluateKB(KnowledgeBase &kb) = 0;
    virtual std::vector<ResultFact> queryKB(const KBQuery &query,
                                            const KnowledgeBase &kb);
};

std::vector<std::reference_wrapper<const FactsPool>>
    GetRelationRefs(KnowledgeBase &kb, const KBRule &rule);

std::vector<KBTuple> AggregateSortedRuleResult(std::vector<KBTuple> result);

} // fovris
#endif /* end of include guard: EVALUATION_H_YKTLWMNO */
