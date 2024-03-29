/* Generated by re2c 0.15.3 on Mon Dec 26 19:56:28 2016 */
#include "QlScanner.h"
#include "QlParse.h"
namespace fovris{

int QlScanner::scan(LexerState &state){
    state.quoteEscapes.clear();
    state.quotedString = false;

#define YYMAXFILL 19

    

    //re2c:indent:top      = 1;
    //re2c:indent:string = "  ";

    std:
        state.start = state.cursor;
    


    
{
	char yych;
	unsigned int yyaccept = 0;
	yych = *state.cursor;
	switch (yych) {
	case 0x00:	goto yy2;
	case '\t':	goto yy58;
	case '\n':	goto yy53;
	case '\r':	goto yy55;
	case ' ':	goto yy56;
	case '!':	goto yy35;
	case '"':	goto yy42;
	case '(':	goto yy21;
	case ')':	goto yy23;
	case ',':	goto yy12;
	case '-':	goto yy47;
	case '.':	goto yy49;
	case '/':	goto yy51;
	case '0':	goto yy44;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy46;
	case ':':	goto yy10;
	case '<':	goto yy31;
	case '=':	goto yy29;
	case '>':	goto yy33;
	case '?':	goto yy37;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':	goto yy40;
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'g':
	case 'h':
	case 'j':
	case 'k':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy39;
	case 'd':	goto yy6;
	case 'e':	goto yy9;
	case 'f':	goto yy8;
	case 'i':	goto yy17;
	case 'l':	goto yy19;
	case 'm':	goto yy4;
	case 'r':	goto yy7;
	case 's':	goto yy20;
	case 't':	goto yy16;
	case 'u':	goto yy18;
	case '{':	goto yy25;
	case '|':	goto yy14;
	case '}':	goto yy27;
	default:	goto yy59;
	}
yy2:
	++state.cursor;
	{ return TOK_END; }
yy4:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case 'o':	goto yy223;
	default:	goto yy117;
	}
yy5:
	{ return TOK_ID; }
yy6:
	yych = *++state.cursor;
	switch (yych) {
	case 'a':	goto yy206;
	case 'o':	goto yy207;
	default:	goto yy117;
	}
yy7:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy187;
	case 'u':	goto yy188;
	default:	goto yy117;
	}
yy8:
	yych = *++state.cursor;
	switch (yych) {
	case 'a':	goto yy177;
	default:	goto yy117;
	}
yy9:
	yych = *++state.cursor;
	switch (yych) {
	case 'n':	goto yy174;
	default:	goto yy117;
	}
yy10:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '-':	goto yy172;
	default:	goto yy11;
	}
yy11:
	{ return TOK_COLON;}
yy12:
	++state.cursor;
	{ return TOK_COMMA;}
yy14:
	++state.cursor;
	{ return TOK_OR;}
yy16:
	yych = *++state.cursor;
	switch (yych) {
	case 'r':	goto yy168;
	default:	goto yy117;
	}
yy17:
	yych = *++state.cursor;
	switch (yych) {
	case 'd':	goto yy147;
	case 'n':	goto yy149;
	default:	goto yy117;
	}
yy18:
	yych = *++state.cursor;
	switch (yych) {
	case 'n':	goto yy140;
	default:	goto yy117;
	}
yy19:
	yych = *++state.cursor;
	switch (yych) {
	case 'i':	goto yy128;
	case 'o':	goto yy129;
	default:	goto yy117;
	}
yy20:
	yych = *++state.cursor;
	switch (yych) {
	case 't':	goto yy122;
	default:	goto yy117;
	}
yy21:
	++state.cursor;
	{ return TOK_LRB;}
yy23:
	++state.cursor;
	{ return TOK_RRB;}
yy25:
	++state.cursor;
	{ return TOK_LCB;}
yy27:
	++state.cursor;
	{ return TOK_RCB;}
yy29:
	++state.cursor;
	{ return TOK_EQ;}
yy31:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '=':	goto yy120;
	default:	goto yy32;
	}
yy32:
	{ return TOK_LT;}
yy33:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '=':	goto yy118;
	default:	goto yy34;
	}
yy34:
	{ return TOK_GT;}
yy35:
	++state.cursor;
	{ return TOK_NEG;}
yy37:
	++state.cursor;
	{ return TOK_QUESTION;}
yy39:
	yych = *++state.cursor;
	goto yy117;
yy40:
	++state.cursor;
	yych = *state.cursor;
	goto yy115;
yy41:
	{ return TOK_VAR; }
yy42:
	++state.cursor;
	{ 
            state.quotedString = true;
            goto quoted_string;
        }
yy44:
	yyaccept = 0;
	yych = *(state.marker = ++state.cursor);
	switch (yych) {
	case '.':	goto yy107;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy106;
	default:	goto yy45;
	}
yy45:
	{ return TOK_INTEGER;}
yy46:
	yyaccept = 0;
	yych = *(state.marker = ++state.cursor);
	switch (yych) {
	case '.':	goto yy80;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy79;
	default:	goto yy45;
	}
yy47:
	yyaccept = 1;
	yych = *(state.marker = ++state.cursor);
	switch (yych) {
	case '0':	goto yy70;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy67;
	default:	goto yy48;
	}
yy48:
	{ return TOK_NEG;}
yy49:
	++state.cursor;
	{ return TOK_DOT;}
yy51:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '/':	goto yy65;
	default:	goto yy52;
	}
yy52:
	{
            return ErrorCode;
        }
yy53:
	++state.cursor;
yy54:
	{
                state.pos = state.cursor;
                state.line++;
                goto std;
            }
yy55:
	yych = *++state.cursor;
	switch (yych) {
	case '\n':	goto yy64;
	default:	goto yy52;
	}
yy56:
	++state.cursor;
	yych = *state.cursor;
	goto yy63;
yy57:
	{ goto std; }
yy58:
	yych = *++state.cursor;
	goto yy61;
yy59:
	yych = *++state.cursor;
	goto yy52;
yy60:
	++state.cursor;
	yych = *state.cursor;
yy61:
	switch (yych) {
	case '\t':	goto yy60;
	default:	goto yy57;
	}
yy62:
	++state.cursor;
	yych = *state.cursor;
yy63:
	switch (yych) {
	case ' ':	goto yy62;
	default:	goto yy57;
	}
yy64:
	yych = *++state.cursor;
	goto yy54;
yy65:
	++state.cursor;
	{ goto comment;}
yy67:
	yyaccept = 2;
	state.marker = ++state.cursor;
	yych = *state.cursor;
	switch (yych) {
	case '.':	goto yy76;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy67;
	default:	goto yy69;
	}
yy69:
	{ return TOK_INTEGER;}
yy70:
	yych = *++state.cursor;
	switch (yych) {
	case '.':	goto yy72;
	default:	goto yy71;
	}
yy71:
	state.cursor = state.marker;
	switch (yyaccept) {
	case 0: 	goto yy45;
	case 1: 	goto yy48;
	case 2: 	goto yy69;
	case 3: 	goto yy94;
	default:	goto yy101;
	}
yy72:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy73;
	default:	goto yy71;
	}
yy73:
	++state.cursor;
	yych = *state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy73;
	default:	goto yy75;
	}
yy75:
	{ return TOK_REAL; }
yy76:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy77;
	default:	goto yy71;
	}
yy77:
	++state.cursor;
	yych = *state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy77;
	default:	goto yy75;
	}
yy79:
	yyaccept = 0;
	yych = *(state.marker = ++state.cursor);
	switch (yych) {
	case '.':	goto yy80;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy84;
	default:	goto yy45;
	}
yy80:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy81;
	default:	goto yy71;
	}
yy81:
	++state.cursor;
	yych = *state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy81;
	default:	goto yy83;
	}
yy83:
	{ return TOK_REAL; }
yy84:
	yyaccept = 0;
	yych = *(state.marker = ++state.cursor);
	switch (yych) {
	case '.':	goto yy80;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy85;
	default:	goto yy45;
	}
yy85:
	yyaccept = 0;
	yych = *(state.marker = ++state.cursor);
	switch (yych) {
	case '-':	goto yy86;
	default:	goto yy88;
	}
yy86:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy89;
	default:	goto yy71;
	}
yy87:
	yyaccept = 0;
	state.marker = ++state.cursor;
	yych = *state.cursor;
yy88:
	switch (yych) {
	case '.':	goto yy80;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy87;
	default:	goto yy45;
	}
yy89:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy90;
	default:	goto yy71;
	}
yy90:
	yych = *++state.cursor;
	switch (yych) {
	case '-':	goto yy91;
	default:	goto yy71;
	}
yy91:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy92;
	default:	goto yy71;
	}
yy92:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy93;
	default:	goto yy71;
	}
yy93:
	yyaccept = 3;
	yych = *(state.marker = ++state.cursor);
	switch (yych) {
	case 'T':
	case 't':	goto yy95;
	default:	goto yy94;
	}
yy94:
	{ return TOK_DATE; }
yy95:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy96;
	default:	goto yy71;
	}
yy96:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy97;
	default:	goto yy71;
	}
yy97:
	yych = *++state.cursor;
	switch (yych) {
	case ':':	goto yy98;
	default:	goto yy71;
	}
yy98:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy99;
	default:	goto yy71;
	}
yy99:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy100;
	default:	goto yy71;
	}
yy100:
	yyaccept = 4;
	yych = *(state.marker = ++state.cursor);
	switch (yych) {
	case ':':	goto yy102;
	default:	goto yy101;
	}
yy101:
	{ return TOK_DATETIME; }
yy102:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy103;
	default:	goto yy71;
	}
yy103:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy104;
	default:	goto yy71;
	}
yy104:
	++state.cursor;
	{ return TOK_DATETIME; }
yy106:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy110;
	default:	goto yy45;
	}
yy107:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy108;
	default:	goto yy71;
	}
yy108:
	++state.cursor;
	yych = *state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy108;
	default:	goto yy83;
	}
yy110:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy111;
	default:	goto yy45;
	}
yy111:
	yyaccept = 0;
	yych = *(state.marker = ++state.cursor);
	switch (yych) {
	case '-':	goto yy86;
	default:	goto yy113;
	}
yy112:
	++state.cursor;
	yych = *state.cursor;
yy113:
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':	goto yy112;
	default:	goto yy45;
	}
yy114:
	++state.cursor;
	yych = *state.cursor;
yy115:
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy114;
	default:	goto yy41;
	}
yy116:
	++state.cursor;
	yych = *state.cursor;
yy117:
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy5;
	}
yy118:
	++state.cursor;
	{ return TOK_GE;}
yy120:
	++state.cursor;
	{ return TOK_LE;}
yy122:
	yych = *++state.cursor;
	switch (yych) {
	case 'r':	goto yy123;
	default:	goto yy117;
	}
yy123:
	yych = *++state.cursor;
	switch (yych) {
	case 'i':	goto yy124;
	default:	goto yy117;
	}
yy124:
	yych = *++state.cursor;
	switch (yych) {
	case 'n':	goto yy125;
	default:	goto yy117;
	}
yy125:
	yych = *++state.cursor;
	switch (yych) {
	case 'g':	goto yy126;
	default:	goto yy117;
	}
yy126:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy127;
	}
yy127:
	{ return TOK_STRING_TYPE;}
yy128:
	yych = *++state.cursor;
	switch (yych) {
	case 't':	goto yy134;
	default:	goto yy117;
	}
yy129:
	yych = *++state.cursor;
	switch (yych) {
	case 'g':	goto yy130;
	default:	goto yy117;
	}
yy130:
	yych = *++state.cursor;
	switch (yych) {
	case 'i':	goto yy131;
	default:	goto yy117;
	}
yy131:
	yych = *++state.cursor;
	switch (yych) {
	case 'c':	goto yy132;
	default:	goto yy117;
	}
yy132:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy133;
	}
yy133:
	{ return TOK_LOGIC_TYPE;}
yy134:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy135;
	default:	goto yy117;
	}
yy135:
	yych = *++state.cursor;
	switch (yych) {
	case 'r':	goto yy136;
	default:	goto yy117;
	}
yy136:
	yych = *++state.cursor;
	switch (yych) {
	case 'a':	goto yy137;
	default:	goto yy117;
	}
yy137:
	yych = *++state.cursor;
	switch (yych) {
	case 'l':	goto yy138;
	default:	goto yy117;
	}
yy138:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy139;
	}
yy139:
	{ return TOK_ID_TYPE;}
yy140:
	yych = *++state.cursor;
	switch (yych) {
	case 'k':	goto yy141;
	default:	goto yy117;
	}
yy141:
	yych = *++state.cursor;
	switch (yych) {
	case 'n':	goto yy142;
	default:	goto yy117;
	}
yy142:
	yych = *++state.cursor;
	switch (yych) {
	case 'o':	goto yy143;
	default:	goto yy117;
	}
yy143:
	yych = *++state.cursor;
	switch (yych) {
	case 'w':	goto yy144;
	default:	goto yy117;
	}
yy144:
	yych = *++state.cursor;
	switch (yych) {
	case 'n':	goto yy145;
	default:	goto yy117;
	}
yy145:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy146;
	}
yy146:
	{ return TOK_UNKNOWN;}
yy147:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy148;
	}
yy148:
	{ return TOK_ID_TYPE;}
yy149:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	case 'c':	goto yy151;
	case 't':	goto yy152;
	default:	goto yy150;
	}
yy150:
	{ return TOK_IN;}
yy151:
	yych = *++state.cursor;
	switch (yych) {
	case 'o':	goto yy158;
	default:	goto yy117;
	}
yy152:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy153;
	default:	goto yy117;
	}
yy153:
	yych = *++state.cursor;
	switch (yych) {
	case 'g':	goto yy154;
	default:	goto yy117;
	}
yy154:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy155;
	default:	goto yy117;
	}
yy155:
	yych = *++state.cursor;
	switch (yych) {
	case 'r':	goto yy156;
	default:	goto yy117;
	}
yy156:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy157;
	}
yy157:
	{ return TOK_INTEGER_TYPE;}
yy158:
	yych = *++state.cursor;
	switch (yych) {
	case 'n':	goto yy159;
	default:	goto yy117;
	}
yy159:
	yych = *++state.cursor;
	switch (yych) {
	case 's':	goto yy160;
	default:	goto yy117;
	}
yy160:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	case 'i':	goto yy162;
	default:	goto yy161;
	}
yy161:
	{ return TOK_INCONS;}
yy162:
	yych = *++state.cursor;
	switch (yych) {
	case 's':	goto yy163;
	default:	goto yy117;
	}
yy163:
	yych = *++state.cursor;
	switch (yych) {
	case 't':	goto yy164;
	default:	goto yy117;
	}
yy164:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy165;
	default:	goto yy117;
	}
yy165:
	yych = *++state.cursor;
	switch (yych) {
	case 'n':	goto yy166;
	default:	goto yy117;
	}
yy166:
	yych = *++state.cursor;
	switch (yych) {
	case 't':	goto yy167;
	default:	goto yy117;
	}
yy167:
	yych = *++state.cursor;
	switch (yych) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy161;
	}
yy168:
	yych = *++state.cursor;
	switch (yych) {
	case 'u':	goto yy169;
	default:	goto yy117;
	}
yy169:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy170;
	default:	goto yy117;
	}
yy170:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy171;
	}
yy171:
	{ return TOK_TRUE;}
yy172:
	++state.cursor;
	{ return TOK_IMPL;}
yy174:
	yych = *++state.cursor;
	switch (yych) {
	case 'd':	goto yy175;
	default:	goto yy117;
	}
yy175:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy176;
	}
yy176:
	{ return TOK_MODULE_END;}
yy177:
	yych = *++state.cursor;
	switch (yych) {
	case 'c':	goto yy178;
	case 'l':	goto yy179;
	default:	goto yy117;
	}
yy178:
	yych = *++state.cursor;
	switch (yych) {
	case 't':	goto yy183;
	default:	goto yy117;
	}
yy179:
	yych = *++state.cursor;
	switch (yych) {
	case 's':	goto yy180;
	default:	goto yy117;
	}
yy180:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy181;
	default:	goto yy117;
	}
yy181:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy182;
	}
yy182:
	{ return TOK_FALSE;}
yy183:
	yych = *++state.cursor;
	switch (yych) {
	case 's':	goto yy184;
	default:	goto yy117;
	}
yy184:
	yych = *++state.cursor;
	switch (yych) {
	case ':':	goto yy185;
	default:	goto yy117;
	}
yy185:
	++state.cursor;
	{ return TOK_FACTS;}
yy187:
	yych = *++state.cursor;
	switch (yych) {
	case 'a':	goto yy195;
	case 'l':	goto yy194;
	default:	goto yy117;
	}
yy188:
	yych = *++state.cursor;
	switch (yych) {
	case 'l':	goto yy189;
	default:	goto yy117;
	}
yy189:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy190;
	default:	goto yy117;
	}
yy190:
	yych = *++state.cursor;
	switch (yych) {
	case 's':	goto yy191;
	default:	goto yy117;
	}
yy191:
	yych = *++state.cursor;
	switch (yych) {
	case ':':	goto yy192;
	default:	goto yy117;
	}
yy192:
	++state.cursor;
	{ return TOK_RULES;}
yy194:
	yych = *++state.cursor;
	switch (yych) {
	case 'a':	goto yy198;
	default:	goto yy117;
	}
yy195:
	yych = *++state.cursor;
	switch (yych) {
	case 'l':	goto yy196;
	default:	goto yy117;
	}
yy196:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy197;
	}
yy197:
	{ return TOK_REAL_TYPE;}
yy198:
	yych = *++state.cursor;
	switch (yych) {
	case 't':	goto yy199;
	default:	goto yy117;
	}
yy199:
	yych = *++state.cursor;
	switch (yych) {
	case 'i':	goto yy200;
	default:	goto yy117;
	}
yy200:
	yych = *++state.cursor;
	switch (yych) {
	case 'o':	goto yy201;
	default:	goto yy117;
	}
yy201:
	yych = *++state.cursor;
	switch (yych) {
	case 'n':	goto yy202;
	default:	goto yy117;
	}
yy202:
	yych = *++state.cursor;
	switch (yych) {
	case 's':	goto yy203;
	default:	goto yy117;
	}
yy203:
	yych = *++state.cursor;
	switch (yych) {
	case ':':	goto yy204;
	default:	goto yy117;
	}
yy204:
	++state.cursor;
	{ return TOK_RELATIONS;}
yy206:
	yych = *++state.cursor;
	switch (yych) {
	case 't':	goto yy215;
	default:	goto yy117;
	}
yy207:
	yych = *++state.cursor;
	switch (yych) {
	case 'm':	goto yy208;
	default:	goto yy117;
	}
yy208:
	yych = *++state.cursor;
	switch (yych) {
	case 'a':	goto yy209;
	default:	goto yy117;
	}
yy209:
	yych = *++state.cursor;
	switch (yych) {
	case 'i':	goto yy210;
	default:	goto yy117;
	}
yy210:
	yych = *++state.cursor;
	switch (yych) {
	case 'n':	goto yy211;
	default:	goto yy117;
	}
yy211:
	yych = *++state.cursor;
	switch (yych) {
	case 's':	goto yy212;
	default:	goto yy117;
	}
yy212:
	yych = *++state.cursor;
	switch (yych) {
	case ':':	goto yy213;
	default:	goto yy117;
	}
yy213:
	++state.cursor;
	{ return TOK_DOMAINS; }
yy215:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy216;
	default:	goto yy117;
	}
yy216:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	case 't':	goto yy218;
	default:	goto yy217;
	}
yy217:
	{ return TOK_DATE_TYPE;}
yy218:
	yych = *++state.cursor;
	switch (yych) {
	case 'i':	goto yy219;
	default:	goto yy117;
	}
yy219:
	yych = *++state.cursor;
	switch (yych) {
	case 'm':	goto yy220;
	default:	goto yy117;
	}
yy220:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy221;
	default:	goto yy117;
	}
yy221:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy222;
	}
yy222:
	{ return TOK_DATETIME_TYPE;}
yy223:
	yych = *++state.cursor;
	switch (yych) {
	case 'd':	goto yy224;
	default:	goto yy117;
	}
yy224:
	yych = *++state.cursor;
	switch (yych) {
	case 'u':	goto yy225;
	default:	goto yy117;
	}
yy225:
	yych = *++state.cursor;
	switch (yych) {
	case 'l':	goto yy226;
	default:	goto yy117;
	}
yy226:
	yych = *++state.cursor;
	switch (yych) {
	case 'e':	goto yy227;
	default:	goto yy117;
	}
yy227:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':	goto yy116;
	default:	goto yy228;
	}
yy228:
	{ return TOK_MODULE; }
}


    quoted_string:
    
{
	char yych;
	yych = *state.cursor;
	switch (yych) {
	case '"':	goto yy233;
	case '\\':	goto yy231;
	default:	goto yy235;
	}
yy231:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '"':	goto yy238;
	default:	goto yy232;
	}
yy232:
	{ goto quoted_string;}
yy233:
	++state.cursor;
	{ return TOK_STRING;}
yy235:
	++state.cursor;
	yych = *state.cursor;
	switch (yych) {
	case '"':
	case '\\':	goto yy237;
	default:	goto yy235;
	}
yy237:
	{ goto quoted_string;}
yy238:
	++state.cursor;
	{
            state.quoteEscapes.push_back(state.cursor - 2);
            goto quoted_string;}
}


    comment:
    
{
	char yych;
	yych = *state.cursor;
	switch (yych) {
	case 0x00:	goto yy242;
	case '\n':	goto yy243;
	case '\r':	goto yy245;
	default:	goto yy247;
	}
yy242:
yy243:
	++state.cursor;
yy244:
	{
                state.pos = state.cursor;
                state.line++; goto std;
            }
yy245:
	++state.cursor;
	switch ((yych = *state.cursor)) {
	case '\n':	goto yy248;
	default:	goto yy246;
	}
yy246:
	{ goto comment; }
yy247:
	yych = *++state.cursor;
	goto yy246;
yy248:
	++state.cursor;
	yych = *state.cursor;
	goto yy244;
}

}
} // fovris

