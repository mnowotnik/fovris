#include "KB/KBRuleFunction.h"
namespace fovris {


KBRuleFunction::KBRuleFunction(bool trueResult,
                               PredicateFunction predicateFunction,
                               std::vector<KBTerm> terms,
                               std::vector<TermType> termTypes)
    : KBLiteral(0xDEADBEEF, std::move(terms)), termTypes(std::move(termTypes)) {
    if (trueResult) {
        this->predicateFunction = predicateFunction;
    } else {
        this->predicateFunction =
            [predicateFunction](const Array<Term> &arr)
                -> bool { return !predicateFunction(arr); };
    }
}


KBRuleFunction::KBRuleFunction(PredicateFunction predicateFunction,
                               std::vector<KBTerm> terms,
                               std::vector<TermType> termTypes)
    : KBRuleFunction(true, predicateFunction, std::move(terms),
                     std::move(termTypes)) {}

const PredicateFunction &KBRuleFunction::getFunction() const {
    return predicateFunction;
}

const std::vector<TermType> &KBRuleFunction::getTermTypes() const {
    return termTypes;
}
} // fovris
