#ifndef TESTUTILS_H_CTU0KYNE
#define TESTUTILS_H_CTU0KYNE
#include <set>
#include "Array.h"
#include "KB/KBTerm.h"
#include "KB/KBGroundTerm.h"
inline fovris::Array<fovris::KBTerm>
    makeVarTerms(std::initializer_list<unsigned> args) {
    using fovris::Array;
    using fovris::KBTerm;
    Array<KBTerm> arr(args.size());
    size_t i = 0;
    for (auto arg : args) {
        arr[i] = KBTerm{arg, true};
        i++;
    }
    return arr;
}

inline std::set<fovris::KBTerm>
    makeVarTermsSet(std::initializer_list<unsigned> args) {
    using fovris::KBTerm;
    std::set<KBTerm> set{};
    size_t i = 0;
    for (auto arg : args) {
        set.insert(KBTerm{arg, true});
        i++;
    }
    return set;
}

inline fovris::Array<fovris::KBGroundTerm>
    makeGroundTerms(std::initializer_list<unsigned> args) {
    using fovris::Array;
    using fovris::KBGroundTerm;
    Array<KBGroundTerm> arr(args.size());
    size_t i = 0;
    for (auto arg : args) {
        arr[i] = fovris::KBGroundTerm{arg};
        i++;
    }
    return arr;
}
#endif /* end of include guard: TESTUTILS_H_CTU0KYNE */
