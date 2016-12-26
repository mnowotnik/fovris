#ifndef QLSCANNER_H_NML6GPSB
#define QLSCANNER_H_NML6GPSB
#include "Scanner.h"
namespace fovris {


class QlScanner : public Scanner {
  public:
    int scan(LexerState &state);
};

} // fovris
#endif /* end of include guard: QLSCANNER_H_NML6GPSB */
