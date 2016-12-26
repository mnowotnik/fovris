#include "KB/KBRuleLiteral.h"
namespace fovris {


KBRuleLiteral::KBRuleLiteral(unsigned relId, std::vector<KBTerm> terms,
                             TruthValueSet lSet)
    : KBLiteral(relId, terms), logicSet(lSet) {}

KBRuleLiteral::KBRuleLiteral(std::vector<KBTerm> terms)
    : KBLiteral(0xDEADBEEF, terms), logicSet({TruthValue::True}) {}

KBRuleLiteral::KBRuleLiteral(std::vector<KBTerm> terms, TruthValueSet lSet)
    : KBLiteral(0xDEADBEEF, terms), logicSet(lSet) {}

KBRuleLiteral::KBRuleLiteral() : KBLiteral(0xDEADBEEF, {}) {}
} // fovris
