#include "KB/KBRuleDisjunct.h"
#include "Eval/RuleProcessing.h"
namespace fovris {

KBRuleDisjunct::KBRuleDisjunct(std::vector<KBRuleLiteral> body,
                               std::vector<KBRuleFunction> functions)
    : body(SortRuleLiterals(std::move(body)))
    , functions(std::move(functions)) {}

KBRuleDisjunct::KBRuleDisjunct(std::vector<KBRuleLiteral> body)
    : body(SortRuleLiterals(std::move(body))) {}

void KBRuleDisjunct::addFunction(KBRuleFunction function) {
    functions.push_back(std::move(function));
}

void KBRuleDisjunct::addLiteral(KBRuleLiteral literal) {
    body.push_back(std::move(literal));
    body = SortRuleLiterals(std::move(body));
}

} // fovris
