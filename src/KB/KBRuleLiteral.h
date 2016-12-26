#ifndef KBRULELITERAL_H_8RL97ASE
#define KBRULELITERAL_H_8RL97ASE
#include <initializer_list>

#include "KB/KBLiteral.h"
#include "TruthValueSet.h"
#include "Utils.h"
namespace fovris {


class KBRuleLiteral : public KBLiteral {
    TruthValueSet logicSet;

  public:
    KBRuleLiteral(unsigned relId, std::vector<KBTerm> terms, TruthValueSet lSet);

    // convenience
    KBRuleLiteral(unsigned relId, std::initializer_list<KBTerm> terms,
                  TruthValueSet lSet)
        : KBRuleLiteral(relId, {terms}, lSet) {}

    // convenience
    KBRuleLiteral(unsigned relId, std::initializer_list<KBTerm> terms,
                  TruthValue tVal)
        : KBRuleLiteral(relId, {terms}, TruthValueSet{tVal}) {}

    // convenience
    explicit KBRuleLiteral(std::vector<KBTerm> terms);

    // convenience
    explicit KBRuleLiteral(std::vector<KBTerm> terms,TruthValueSet lSet);

    explicit KBRuleLiteral();

    const decltype(logicSet) &getLogicSet() const { return logicSet; }

    bool hasUnknown() const { return logicSet.hasUnknown(); }
    bool isUnsafe() const { return hasUnknown(); }
};

} // fovris
#endif /* end of include guard: KBRULELITERAL_H_8RL97ASE */
