#include "TruthValueSet.h"
#include <ostream>
namespace fovris {

TruthValueSet::TruthValueSet(const std::vector<TruthValue> &values) {
    for (TruthValue t : values) {
        set.set(static_cast<size_t>(t), 1);
    }
}

TruthValueSet::TruthValueSet(std::initializer_list<TruthValue> values) {
    for (TruthValue t : values) {
        set.set(static_cast<size_t>(t), 1);
    }
}

TruthValueSet::operator bool() const { return !isEmpty(); }

TruthValueSet TruthValueSet::getComplement() const {
    TruthValueSet copy(*this);
    for (auto val : {TruthValue::True, TruthValue::False, TruthValue::Incons,
                     TruthValue::Unknown}) {
        copy.set.set(static_cast<size_t>(val),
                     !set.test(static_cast<uint8_t>(val)));
    }
    return copy;
}

TruthValueSet TruthValueSet::getNegation() const {
    if (set.test(static_cast<uint8_t>(TruthValue::True))) {
        if (set.test(static_cast<uint8_t>(TruthValue::False))) {
            return *this;
        }
        TruthValueSet copy(*this);
        copy.set.set(static_cast<size_t>(TruthValue::True), false);
        copy.set.set(static_cast<size_t>(TruthValue::False));
        return copy;
    }

    if (set.test(static_cast<uint8_t>(TruthValue::False))) {
        TruthValueSet copy(*this);
        copy.set.set(static_cast<size_t>(TruthValue::False), false);
        copy.set.set(static_cast<size_t>(TruthValue::True));
        return copy;
    }
    return *this;
}


std::vector<TruthValue> TruthValueSet::getSet() const {
    std::vector<TruthValue> vec;
    for (uint8_t i = 0; i < 4; ++i) {
        if (set.test(i)) {
            vec.push_back(static_cast<TruthValue>(i));
        }
    }
    return vec;
}

std::ostream &operator<<(std::ostream &os, const TruthValueSet &t) {
    int printed = 0;
    for (uint8_t i = 0; i < 4; i++) {
        if (t.set.test(i)) {
            if (printed > 0) {
                os << ',';
            }
            os << static_cast<TruthValue>(i);
            printed++;
        }
    }
    return os;
}
} // fovris
