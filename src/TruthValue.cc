#include "TruthValue.h"

#include <ostream>
namespace fovris {

namespace {

// Unknown does not participate in the reasoning so it's omitted

const TruthValue F = TruthValue::False, // 0
    U = TruthValue::Unknown, // 1
    I = TruthValue::Incons, // 2
    T = TruthValue::True; // 3

const TruthValue OrArr[4][4] = {
    {F, U, I, T}, {U, U, I, T}, {I, I, I, T}, {T, T, T, T}};

const TruthValue AndArr[4][4] = {
    {F, F, F, F}, {F, U, U, U}, {F, U, I, I}, {F, U, I, T}};

const TruthValue DisjWrtOrd[4][4] = {
    {F, F, I, I}, {F, U, I, T}, {I, I, I, I}, {I, T, I, T}};

const TruthValue NegationArr[8] = {T, U, I, F};
} // namespace

TruthValue Negation(TruthValue a) {
    return NegationArr[static_cast<uint8_t>(a)];
}

TruthValue Disjunction(TruthValue a, TruthValue b) {
    return OrArr[static_cast<uint8_t>(a)][static_cast<uint8_t>(b)];
}

TruthValue Conjunction(TruthValue a, TruthValue b) {
    return AndArr[static_cast<uint8_t>(a)][static_cast<uint8_t>(b)];
}

TruthValue DisjunctionWrtOrdering(TruthValue a, TruthValue b) {
    return DisjWrtOrd[static_cast<uint8_t>(a)][static_cast<uint8_t>(b)];
}

TruthValue ResolveToTrueOrIncons(TruthValue val) {
    if (val == I) {
        return val;
    }
    return TruthValue::True;
}


std::ostream &operator<<(std::ostream &os, TruthValue t) {
    switch (t) {
        case TruthValue::False:
            os << "false";
            break;
        case TruthValue::Unknown:
            os << "unknown";
            break;
        case TruthValue::Incons:
            os << "inconsistent";
            break;
        case TruthValue::True:
            os << "true";
            break;
    }
    return os;
}
} // fovris
