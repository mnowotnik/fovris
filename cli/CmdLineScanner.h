#ifndef CMDLINESCANNER_H_Z5VTU78B
#define CMDLINESCANNER_H_Z5VTU78B
#include "Scanner.h"
#include <iosfwd>

namespace fovris{

class CmdLineScanner : public Scanner {
  public:
    int scan(LexerState &state);
    enum Code {
        Select = 0,
        Help,
        Exit,
        Import,
        ImportQuoted,
        End,
        String,
        Modules,
        Clear
    };

    friend std::ostream &operator<<(std::ostream &os,
                                    CmdLineScanner::Code code);
};
} // fovris

#endif /* end of include guard: CMDLINESCANNER_H_Z5VTU78B */
