#include "KB/KBGroundTerm.h"

namespace {
const unsigned NullVal = 0xFFFFFFFF;
}

namespace fovris {

bool KBGroundTerm::isNull() const { return val_ == NullVal; }

KBGroundNullTerm::KBGroundNullTerm() : KBGroundTerm(NullVal) {}

} // fovris
