#ifndef LEXER_H_93OERDU8
#define LEXER_H_93OERDU8

#include <string>
#include <memory>
#include "Scanner.h"
#include "LexerState.h"
namespace fovris {


class Lexer {
  public:
    static const int ErrorCode = -1;
    Lexer(std::unique_ptr<Scanner> scanner);
    Lexer(const char *s, std::unique_ptr<Scanner> scanner);
    Lexer(const char *s, unsigned n, std::unique_ptr<Scanner> scanner);
    typedef int TokenCode;

    TokenCode scan();

    std::string getToken() const;
    unsigned getLine() const { return state.line; }
    unsigned getPos() const;
    TokenCode getCode() const;

    void reset(const char *s, unsigned n);
    void reset(const char *s);

  private:
    TokenCode tokenCode = ErrorCode;
    const char *content;
    LexerState state;
    std::unique_ptr<Scanner> scanner;
};

} // fovris
#endif /* end of include guard: LEXER_H_93OERDU8 */
