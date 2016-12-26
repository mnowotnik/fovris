#ifndef KBHEADLITERAL_H_B3H4SAQL
#define KBHEADLITERAL_H_B3H4SAQL

#include "KB/KBLiteral.h"
namespace fovris {


class KBHeadLiteral : public KBLiteral {
    bool negated = false;

  public:
    KBHeadLiteral(unsigned relId, std::vector<KBTerm> terms);
    KBHeadLiteral(bool isTrue, unsigned relId, std::vector<KBTerm> terms);
    bool isNegated() const { return negated; }
};

} // fovris
#endif /* end of include guard: KBHEADLITERAL_H_B3H4SAQL */
