#ifndef SCANNER_H_JXZ3QIM5
#define SCANNER_H_JXZ3QIM5
#include "LexerState.h"
namespace fovris {


class Scanner {
  public:
    const static int ErrorCode = -1;
    virtual int scan(LexerState &state) = 0;
};


} // fovris
#endif /* end of include guard: SCANNER_H_JXZ3QIM5 */
