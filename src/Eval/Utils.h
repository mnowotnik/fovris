#ifndef UTILS_H_LAIZ4TDQ
#define UTILS_H_LAIZ4TDQ
#include "KB/KBTerm.h"
#include "KB/KBGroundTerm.h"
#include "KB/KBTuple.h"
#include "TruthValue.h"
#include "Array.h"

#include <algorithm>
namespace fovris {

unsigned inline countVariables(const Array<KBTerm> &terms) {
    return std::count_if(terms.begin(), terms.end(),
                         [](const KBTerm &t) { return t.isVariable(); });
}


} // fovris
#endif /* end of include guard: UTILS_H_LAIZ4TDQ */
