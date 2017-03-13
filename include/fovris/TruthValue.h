#ifndef TRUTHVALUE_H_QLBG67U1
#define TRUTHVALUE_H_QLBG67U1
#include <cstdint>
#include <iosfwd>
namespace fovris {
enum class TruthValue : uint8_t { False = 0, Unknown, Incons, True, COUNT };

TruthValue Conjunction(TruthValue a, TruthValue b);
TruthValue Disjunction(TruthValue a, TruthValue b);
TruthValue DisjunctionWrtOrdering(TruthValue a, TruthValue b);
TruthValue Negation(TruthValue a);

std::ostream &operator<<(std::ostream &os, TruthValue t);

constexpr TruthValue mapBoolToTruthValue(bool val) {
    return val ? TruthValue::True : TruthValue::False;
}

inline TruthValue ResolveTruthValue(TruthValue val, bool isTrue) {
    if (isTrue) {
        return val;
    }
    return Negation(val);
}

TruthValue ResolveToTrueOrIncons(TruthValue val);

} // fovris
#endif /* end of include guard: TRUTHVALUE_H_QLBG67U1 */
