#include "QlScanner.h"
#include "QlParse.h"
namespace fovris{

int QlScanner::scan(LexerState &state){
    state.quoteEscapes.clear();
    state.quotedString = false;

    /*!max:re2c */

    /*!re2c
        re2c:define:YYCTYPE  = "char";
        re2c:define:YYCURSOR = state.cursor;
        re2c:define:YYLIMIT = state.limit;
        re2c:define:YYMARKER = state.marker;
        re2c:yyfill:enable   = 0;
    */

    //re2c:indent:top      = 1;
    //re2c:indent:string = "  ";

    std:
        state.start = state.cursor;
    /*!re2c
    any	= [\001-\377];
    digit	= [0-9];
    letter	= [a-zA-Z];
    lletter = [a-z];
    hletter = [A-Z];
    quote = [\x22\x27];
    */


    /*!re2c
        '\000'              { return TOK_END; }
        "module"            { return TOK_MODULE; }
        "domains:"          { return TOK_DOMAINS; }
        "relations:"        { return TOK_RELATIONS;}
        "rules:"            { return TOK_RULES;}
        "facts:"            { return TOK_FACTS;}
        "end"              { return TOK_MODULE_END;}
        ":-"                { return TOK_IMPL;}
        ":"                 { return TOK_COLON;}
        ","                 { return TOK_COMMA;}
        "|"                 { return TOK_OR;}
        "true"              { return TOK_TRUE;}
        "false"             { return TOK_FALSE;}
        "inconsistent" | "incons" { return TOK_INCONS;}
        "unknown"           { return TOK_UNKNOWN;}
        "literal"           { return TOK_ID_TYPE;}
        "id"                { return TOK_ID_TYPE;}
        "string"            { return TOK_STRING_TYPE;}
        "integer"           { return TOK_INTEGER_TYPE;}
        "real"              { return TOK_REAL_TYPE;}
        "date"              { return TOK_DATE_TYPE;}
        "logic"             { return TOK_LOGIC_TYPE;}
        "datetime"          { return TOK_DATETIME_TYPE;}
        "("                 { return TOK_LRB;}
        ")"                 { return TOK_RRB;}
        "{"                 { return TOK_LCB;}
        "}"                 { return TOK_RCB;}
        "in"                { return TOK_IN;}
        "="                 { return TOK_EQ;}
        "<"                 { return TOK_LT;}
        ">"                 { return TOK_GT;}
        "<="                { return TOK_LE;}
        ">="                { return TOK_GE;}
        "!"                 { return TOK_NEG;}
        "?"                 { return TOK_QUESTION;}
        [_a-z][_a-zA-Z0-9]* { return TOK_ID; }
        hletter (letter | digit) *	{ return TOK_VAR; }
        '"'               { 
            state.quotedString = true;
            goto quoted_string;
        }
        digit {4} '-' digit {2} '-' digit{2} 'T' digit{2} ':' digit{2} { return TOK_DATETIME; }
        digit {4} '-' digit {2} '-' digit{2} 'T' digit{2} ':' digit{2} ':' digit{2} { return TOK_DATETIME; }

        digit {4} '-' digit {2} '-' digit{2}          { return TOK_DATE; }
        '-' [1-9] digit *                             { return TOK_INTEGER;}
        ("0." digit+ ) | ([1-9] digit * "." digit + ) { return TOK_REAL; }
        digit +                                       { return TOK_INTEGER;}
        '-' ("0." digit+ ) | '-' ([1-9] digit * "." digit + ) { return TOK_REAL; }
        "."                 { return TOK_DOT;}
        "-"                 { return TOK_NEG;}


        "//"                { goto comment;}
        "\n" | "\r\n"
            {
                state.pos = state.cursor;
                state.line++;
                goto std;
            }

        " " + | "\t" +
            { goto std; }

        any                 {
            return ErrorCode;
        }
    */

    quoted_string:
    /*!re2c
        '\\"'                   {
            state.quoteEscapes.push_back(state.cursor - 2);
            goto quoted_string;}
        '\\'                    { goto quoted_string;}
        '"'                     { return TOK_STRING;}
        [^\\"]+                 { goto quoted_string;}
        '\\'                    {
            return ErrorCode;
        }
    */

    comment:
    /*!re2c
        "\n" | "\r\n"
            {
                state.pos = state.cursor;
                state.line++; goto std;
            }
        any			{ goto comment; }
    */
}
} // fovris

