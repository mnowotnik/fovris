#include "RuleLiteral.h"
#include "Pred/BinPredicateFunction.h"

#include <map>

namespace fovris {
namespace {
const std::map<std::string, PredicateFunction> predicateFunctionMap = {
    {"=", binDispatch(BuiltinPredicate::Eq)},
    {"!=", binDispatch(BuiltinPredicate::Neq)},
    {"<", binDispatch(BuiltinPredicate::Lt)},
    {"<=", binDispatch(BuiltinPredicate::Le)},
    {">", binDispatch(BuiltinPredicate::Gt)},
    {">=", binDispatch(BuiltinPredicate::Ge)}};
}

RuleLiteral::RuleLiteral(bool isTrue, std::string predicateName,
                         PredicateFunction predicateFunction,
                         std::vector<Term> terms)
    : Literal(isTrue, predicateName, std::move(terms))
    , predicateFunction(predicateFunction) {}

RuleLiteral::RuleLiteral(bool isTrue, std::string module,
                         std::string relationName, std::vector<Term> terms,
                         const std::vector<TruthValue> &c)
    : Literal(isTrue, relationName, std::move(terms)), externalModule(module) {
    if (isTrue) {
        logicConstraint = TruthValueSet(c);
    } else {
        logicConstraint = TruthValueSet(c).getComplement();
    }
}

RuleLiteral::RuleLiteral(std::string module, std::string relationName,
                         std::vector<Term> terms,
                         const std::vector<TruthValue> &c)
    : RuleLiteral(true, module, relationName, std::move(terms), c) {}


RuleLiteral::RuleLiteral(std::string predicateName,
                         PredicateFunction predicateFunction,
                         std::vector<Term> terms)
    : RuleLiteral(true, predicateName, predicateFunction, std::move(terms)) {}

RuleLiteral::RuleLiteral(std::string predicateName, std::vector<Term> terms)
    : RuleLiteral(true, predicateName, std::move(terms)) {}

RuleLiteral::RuleLiteral(bool isTrue, std::string predicateName,
                         std::vector<Term> terms)
    : Literal(isTrue, predicateName, std::move(terms)) {
    auto it = predicateFunctionMap.find(predicateName);
    if (it != predicateFunctionMap.end()) {
        isBuiltinPredicateFunction = true;
        predicateFunction = it->second;
    }
}

RuleLiteral::RuleLiteral(const Literal &literal) : Literal(literal) {}
RuleLiteral::RuleLiteral(Literal &&literal) : Literal(std::move(literal)) {}


bool RuleLiteral::hasConstraint() const { return bool(logicConstraint); }
bool RuleLiteral::isExternal() const {
    return bool(externalModule) && !externalModule->empty();
}


bool RuleLiteral::isUnsafe() const { return hasConstraint() || isFunction(); }

bool RuleLiteral::isFunction() const { return bool(predicateFunction); }
bool RuleLiteral::isBuiltinFunction() const {
    return isBuiltinPredicateFunction;
}

bool RuleLiteral::operator==(const RuleLiteral &o) const {
    return externalModule == o.externalModule &&
           logicConstraint == o.logicConstraint;
}

std::ostream &operator<<(std::ostream &os, const RuleLiteral &l) {
    return l.print(os, false);
}

std::ostream &RuleLiteral::print(std::ostream &os, bool typeInfo) const {

    if (!isTrue()) {
        os << '!';
    }

    auto &terms = getTerms();
    if (isBuiltinPredicateFunction) {
        os << terms[0] << getPredicateSymbol() << terms[1];
        return os;
    }

    if (isExternal()) {
        os << *externalModule << '.';
    }


    os << getPredicateSymbol() << '(';

    auto begIt = terms.begin();
    for (auto it = begIt; it != terms.end(); it++) {
        if (it > begIt) {
            os << ',';
        }
        it->print(os, typeInfo);
    }

    os << ')';

    if (hasConstraint()) {
        os << " in {";
        os << logicConstraint;
        os << "}";
    }

    return os;
}
} // fovris
