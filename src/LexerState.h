#ifndef LEXERSTATE_H_FEIVFWA7
#define LEXERSTATE_H_FEIVFWA7
#include <vector>

namespace fovris {
struct LexerState {
    /* re2c pointers */
    const char *start = nullptr;
    const char *cursor;
    const char *limit;
    const char *marker;
    /* const char *ctxmarker; */

    // last cursor position
    const char *pos;
    unsigned int line = 1;

    bool quotedString = false;
    std::vector<const char*>quoteEscapes;
};
} // fovris
#endif /* end of include guard: LEXERSTATE_H_FEIVFWA7 */
