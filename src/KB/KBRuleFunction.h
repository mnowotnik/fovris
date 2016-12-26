#ifndef KBRULEFUNCTION_H_YI1WBXAV
#define KBRULEFUNCTION_H_YI1WBXAV
#include "PredicateFunction.h"
#include "KB/KBLiteral.h"
namespace fovris {


class KBRuleFunction : public KBLiteral {
    PredicateFunction predicateFunction;
    std::vector<TermType> termTypes;

  public:
    KBRuleFunction(bool trueResult, PredicateFunction predicateFunction,
                   std::vector<KBTerm> terms, std::vector<TermType> termTypes);
    KBRuleFunction(PredicateFunction predicateFunction, std::vector<KBTerm> terms,
                   std::vector<TermType> termTypes);

    const PredicateFunction &getFunction() const;
    const std::vector<TermType> &getTermTypes() const;
};


} // fovris
#endif /* end of include guard: KBRULEFUNCTION_H_YI1WBXAV */
