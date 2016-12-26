#ifndef TRUTHVALUESET_H_BHTYIQ8L
#define TRUTHVALUESET_H_BHTYIQ8L
#include <bitset>
#include <vector>
#include <initializer_list>

#include "TruthValue.h"
namespace fovris {

class TruthValueSet {
    std::bitset<static_cast<int>(TruthValue::COUNT)> set;

  public:
    TruthValueSet() {}
    explicit TruthValueSet(const std::vector<TruthValue> &values);
    TruthValueSet(std::initializer_list<TruthValue> values);

    std::vector<TruthValue> getSet() const;

    friend std::ostream &operator<<(std::ostream &os, const TruthValueSet &t);

    size_t size() const { return set.count(); }
    bool isEmpty() const { return !set.any(); }
    bool isFalse() const {
        return set.test(static_cast<uint8_t>(TruthValue::False));
    }
    bool hasUnknown() const {
        return set.test(static_cast<uint8_t>(TruthValue::Unknown));
    }

    TruthValueSet getNegation() const;
    TruthValueSet getComplement() const;

    operator bool() const;

    bool operator==(const TruthValueSet &o) const { return set == o.set; }
};
} // fovris
#endif /* end of include guard: TRUTHVALUESET_H_BHTYIQ8L */
