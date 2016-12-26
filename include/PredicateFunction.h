#ifndef PREDICATE_H_1QKBYAJ3
#define PREDICATE_H_1QKBYAJ3
#include <functional>
#include "Array.h"
#include "Term.h"

namespace fovris {
typedef std::function<bool(const Array<Term> &)> PredicateFunction;
} // fovris
#endif /* end of include guard: PREDICATE_H_1QKBYAJ3 */
