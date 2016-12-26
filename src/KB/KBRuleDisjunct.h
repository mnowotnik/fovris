#ifndef KBRULECONJUNCTION_H_ND2KXC1G
#define KBRULECONJUNCTION_H_ND2KXC1G
#include <initializer_list>

#include "KB/KBRuleLiteral.h"
#include "KB/KBRuleFunction.h"
namespace fovris {


class KBRuleDisjunct {
    std::vector<KBRuleLiteral> body;
    std::vector<KBRuleFunction> functions;

  public:
    explicit KBRuleDisjunct(std::vector<KBRuleLiteral> body);

    explicit KBRuleDisjunct(std::vector<KBRuleLiteral> body,
                   std::vector<KBRuleFunction> functions);

    const decltype(body) &getBody() const { return body; }
    const decltype(functions) &getFunctions() const { return functions; }

    void addFunction(KBRuleFunction function);
    void addLiteral(KBRuleLiteral literal);
};
} // fovris
#endif /* end of include guard: KBRULECONJUNCTION_H_ND2KXC1G */
