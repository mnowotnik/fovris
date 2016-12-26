#ifndef BINPREDICATEFUNCTION_H_2HWB3J8C
#define BINPREDICATEFUNCTION_H_2HWB3J8C

#include "PredicateFunction.h"

#include <memory>
#include <cstdint>

namespace fovris {
enum class BuiltinPredicate : uint8_t { Lt, Le, Gt, Ge, Eq, Neq };
PredicateFunction binDispatch(BuiltinPredicate pred);
} // fovris
#endif /* end of include guard: BINPREDICATEFUNCTION_H_2HWB3J8C */
