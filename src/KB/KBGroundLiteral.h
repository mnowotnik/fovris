#ifndef KBGROUNDLITERAL_H_YBIKXGOL
#define KBGROUNDLITERAL_H_YBIKXGOL
#include "KB/KBGroundTerm.h"
#include "Array.h"
#include "TruthValue.h"
namespace fovris {

class KBGroundLiteral {

  private:
    unsigned relId;
    Array<KBGroundTerm> terms;
    TruthValue value;

  public:
    KBGroundLiteral(unsigned relId, Array<KBGroundTerm> terms, TruthValue val)
        : relId(relId), terms(std::move(terms)), value(val) {}

    decltype(terms) &getTerms() { return terms; }
    const decltype(terms) &getTerms() const { return terms; }
    unsigned getRelId() const { return relId; }
    TruthValue getValue() const { return value; }

    bool operator==(const KBGroundLiteral &o) const {

        if (relId != o.relId) {
            return false;
        }
        if (terms != o.terms) {
            return false;
        }

        return true;
    }
};

} // fovris
#endif /* end of include guard: KBGROUNDLITERAL_H_YBIKXGOL */
