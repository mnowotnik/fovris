#ifndef KBTUPLE_H_I90GLUX1
#define KBTUPLE_H_I90GLUX1
#include "KB/KBGroundTerm.h"
#include "Array.h"
#include "TruthValue.h"

#include <iosfwd>

namespace fovris {


class KBTuple {
    Array<KBGroundTerm> terms;
    TruthValue value;

  public:
    KBTuple() : terms{0}, value(TruthValue::True) {}
    KBTuple(Array<KBGroundTerm> terms, TruthValue value)
        : terms(std::move(terms)), value(value) {}
    const Array<KBGroundTerm> &getTerms() const { return terms; }
    decltype(value) getValue() const { return value; }
    void setValue(TruthValue val) { value = val; }
    bool operator<(const KBTuple &o) const;
    bool operator>(const KBTuple &o) const;
    bool operator==(const KBTuple &o) const;
    bool operator!=(const KBTuple &o) const;
};
std::ostream &operator<<(std::ostream &os, const KBTuple &tup);
} // fovris

#endif /* end of include guard: KBTUPLE_H_I90GLUX1 */
