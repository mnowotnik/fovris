#include "CmdLineScanner.h"
#include "QlScanner.h"
#include <ostream>

namespace fovris {

std::ostream &operator<<(std::ostream& os,CmdLineScanner::Code code){
    typedef CmdLineScanner::Code Code;
    switch(code){
        case Code::Select:
            os << "Select" << std::endl; break;
        case Code::Help:
            os << "Help" << std::endl; break;
        case Code::Exit:
            os << "Exit" << std::endl; break;
        case Code::Import:
            os << "Import" << std::endl; break;
        case Code::ImportQuoted:
            os << "ImportQuoted" << std::endl; break;
        case Code::End:
            os << "End" << std::endl; break;
        case Code::String:
            os << "String" << std::endl; break;
        case Code::Clear:
            os << "Clear" << std::endl; break;
        case Code::Modules:
            os << "Modules" << std::endl; break;
    }
    return os;
}

int CmdLineScanner::scan(LexerState &state){

    /*!max:re2c */

    /*!re2c
        re2c:define:YYCTYPE  = "char";
        re2c:define:YYCURSOR = state.cursor;
        re2c:define:YYLIMIT = state.limit;
        re2c:define:YYMARKER = state.marker;
        re2c:yyfill:enable   = 0;
    */

    std:
        state.start = state.cursor;
    /*!re2c
    any = [\001-\377];
    digit   = [0-9];
    letter  = [a-zA-Z];
    lletter = [a-z];
    hletter = [A-Z];
    quote = [\x22\x27];
    id = [_a-z][_a-zA-Z0-9]*;
    spaces = " ";
    */

    /*!re2c
        '\000'              { return Code::End; }
        id "." id any +     { return Code::Select; }
        "import"            {
                               state.start= state.cursor;
                               goto import; }
        "exit"              { return Code::Exit; }
        "clear"             { return Code::Clear; }
        "?"                 { return Code::Help; }
        "help"              { return Code::Help; }
        "modules"           { return Code::Modules; }

        " " +               { goto std; }
        any                 { return ErrorCode;}
    */

    import:
    /*!re2c
        ' ' +               {
                                state.start = state.cursor;
                                goto import; }
        '\000'              { return Code::End; }
        '"' any + '"'       { return Code::ImportQuoted; }
        [^" ] any +         { return Code::Import; }
        any                 { return ErrorCode;}
    */
}
} // fovris
