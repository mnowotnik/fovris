#include "KB/KBHeadLiteral.h"
namespace fovris {


KBHeadLiteral::KBHeadLiteral(unsigned relId, std::vector<KBTerm> terms)
    : KBLiteral(relId, std::move(terms)) {}

KBHeadLiteral::KBHeadLiteral(bool isTrue, unsigned relId, std::vector<KBTerm> terms)
    : KBLiteral(relId, std::move(terms)), negated(!isTrue) {}

} // fovris
