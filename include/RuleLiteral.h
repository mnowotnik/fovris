#ifndef RULELITERAL_H_D4WLOK8L
#define RULELITERAL_H_D4WLOK8L
#include <cstdint>
#include "boost/optional.hpp"

#include "TruthValueSet.h"
#include "PredicateFunction.h"
#include "make_unique.hpp"
#include "Literal.h"

namespace fovris {

class RuleLiteral : public Literal {
    PredicateFunction predicateFunction;
    bool isBuiltinPredicateFunction = false;
    boost::optional<std::string> externalModule;

    TruthValueSet logicConstraint;

  public:
    RuleLiteral(const RuleLiteral &copy) = default;
    RuleLiteral(RuleLiteral &&move) = default;

    RuleLiteral(std::string predicateName, PredicateFunction predicateFunction,
                std::vector<Term> terms);

    RuleLiteral(bool isTrue, std::string predicateName,
                PredicateFunction predicateFunction, std::vector<Term> terms);

    RuleLiteral(bool isTrue, std::string module, std::string relationName,
                std::vector<Term> terms, const std::vector<TruthValue> &c = {});

    RuleLiteral(std::string module, std::string relationName,
                std::vector<Term> terms, const std::vector<TruthValue> &c = {});

    RuleLiteral(bool isTrue, std::string predicateName,
                std::vector<Term> terms);

    RuleLiteral(std::string predicateName,std::vector<Term> terms);

    RuleLiteral(const Literal &literal);
    RuleLiteral(Literal &&literal);

    const decltype(externalModule) &getExternalModule() const {
        return externalModule;
    }
    const decltype(logicConstraint) &getLogicConstraint() const {
        return logicConstraint;
    }
    const decltype(predicateFunction) &getPredicateFunction() const {
        return predicateFunction;
    }


    bool hasConstraint() const;
    bool isExternal() const;
    bool isFunction() const;
    bool isBuiltinFunction() const;
    bool isUnsafe() const;

    bool operator==(const RuleLiteral &o) const;

    std::ostream &print(std::ostream &os, bool typeInfo) const;
    friend std::ostream &operator<<(std::ostream &os, const RuleLiteral &l);
};
} // fovris

#endif /* end of include guard: RULELITERAL_H_D4WLOK8L */
