#ifndef KBLITERAL_H_LJXTODM8
#define KBLITERAL_H_LJXTODM8
#include <map>
#include <set>
#include <vector>

#include "KB/KBTerm.h"
#include "Array.h"

namespace fovris {

class KBLiteral {

  private:
    unsigned relId;
    std::map<KBTerm, std::vector<unsigned>> varIndices;
    std::vector<KBTerm> varSet;
    std::vector<KBTerm> terms;

  public:
    KBLiteral(unsigned relId, std::vector<KBTerm> terms);

    const decltype(terms) &getTerms() const { return terms; }
    unsigned getRelId() const { return relId; }
    const decltype(varIndices) &getVarIndices() const { return varIndices; }
    const decltype(varSet) &getVarSet() const { return varSet; }
    std::set<KBTerm> getTermSet() const;

    bool operator==(const KBLiteral &o) const;
};

bool HaveCommonVariables(const KBLiteral &a, const KBLiteral &b);

std::vector<unsigned> GetOrderedUniqueVarIndices(const KBLiteral &lit);

} // fovris
#endif /* end of include guard: KBLITERAL_H_LJXTODM8 */
