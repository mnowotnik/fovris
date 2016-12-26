#include "KB/KBTerm.h"

namespace {
const unsigned NullVal = 0xFFFFFFFF;
}

namespace fovris {

bool KBTerm::isNull() const { return val_ == NullVal; }

KBNullTerm::KBNullTerm() : KBTerm(NullVal) {}

} // fovris
