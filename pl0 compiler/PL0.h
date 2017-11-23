#include <stdio.h>

#define NRW        19     // number of reserved words
#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       30     // maximum number of symbols in array ssym and csym
#define MAXIDLEN   10     // length of identifiers

#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array

#define MAXSYM     50     // maximum number of symbols

#define STACKSIZE  2000   // maximum storage

enum symtype
{
    SYM_NULL,
    SYM_IDENTIFIER,
    SYM_NUMBER,
    SYM_PLUS,
    SYM_MINUS,
    SYM_TIMES,
    SYM_SLASH,
    SYM_ODD,
    SYM_EQU,
    SYM_NEQ,
    SYM_LES,
    SYM_LEQ,
    SYM_GTR,
    SYM_GEQ,
    SYM_LPAREN,
    SYM_RPAREN,
    SYM_COMMA,
    SYM_SEMICOLON,
    SYM_PERIOD,
    SYM_BECOMES,
    SYM_BEGIN,
    SYM_END,
    SYM_IF,
    SYM_THEN,
    SYM_WHILE,
    SYM_DO,
    SYM_CONST,
    SYM_VAR,
    SYM_FUNCTION,

//new reserved words
    SYM_ELSE,
    SYM_EXIT,
    SYM_RETURN,
    SYM_FOR,
    SYM_RANDOM,
    SYM_PRINT,
    SYM_GOTO,
    SYM_BREAK,
    SYM_CONTINUE,
    SYM_SWITCH,

//new character symbols
    SYM_AND, //OK
    SYM_OR,  //OK
    SYM_XOR, //OK
    SYM_NOT, //OK

    SYM_BAND, //OK
    SYM_BOR, //OK
    SYM_BXOR, //OK
    SYM_BNOT, //OK

    SYM_MOD, //OK
    SYM_LSQRBRA,
    SYM_RSQRBRA,

    SYM_QUESTION,
    SYM_COLON,
    SYM_LSHIFT, //OK
    SYM_RSHIFT, //OK
    SYM_LSHBEC, //OK
    SYM_RSHBEC, //OK
    SYM_BANDBEC,//OK
    SYM_BXORBEC,//OK
    SYM_BORBEC, //OK
    SYM_ADDBEC, //OK
    SYM_SUBBEC, //OK
    SYM_MULBEC, //OK
    SYM_DIVBEC, //OK
    SYM_MODBEC, //OK

    SYM_INC,
    SYM_DEC


};

enum idtype
{
    ID_CONSTANT,
    ID_VARIABLE,
    ID_FUNCTION
};

enum opcode
{
    LIT,
    OPR,
    LOD,
    STO,
    CAL,
    INT,
    JMP,
    JPC,
    POP,
    XCHG,
    INC,
    DEC,
	LEAVE,
	RET,
	SETRET
};

enum oprcode
{
    OPR_NEG,
    OPR_ADD,
    OPR_MIN,
    OPR_MUL,
    OPR_DIV,
    OPR_ODD,
    OPR_EQU,
    OPR_NEQ,
    OPR_LES,
    OPR_LEQ,
    OPR_GTR,
    OPR_GEQ,

    OPR_NOT,
    OPR_AND,
    OPR_XOR,
    OPR_OR,

    OPR_BNOT,
    OPR_BAND,
    OPR_BXOR,
    OPR_BOR,

    OPR_MOD,
    OPR_LSHIFT,
    OPR_RSHIFT,
};

typedef struct
{
	int f; // function code
	int l; // level
	int a; // displacement address
} instruction;

//////////////////////////////////////////////////////////////////////
char *err_msg[] =
{
	/*  0 */    "",
	/*  1 */    "Found ':=' when expecting '='.",
	/*  2 */    "There must be a number to follow '='.",
	/*  3 */    "There must be an '=' to follow the identifier.",
	/*  4 */    "There must be an identifier to follow 'const', 'var', or 'FUNCTION'.",
	/*  5 */    "Missing ',' or ';'.",
	/*  6 */    "Incorrect procedure name.",
	/*  7 */    "Statement expected.",
	/*  8 */    "Follow the statement is an incorrect symbol.",
	/*  9 */    "'.' expected.",
	/* 10 */    "';' expected.",
	/* 11 */    "Undeclared identifier.",
	/* 12 */    "Illegal assignment.",
	/* 13 */    "':=' expected.",
	/* 14 */    "There must be an identifier to follow the 'call'.",
	/* 15 */    "A constant or variable can not be called.",
	/* 16 */    "'then' expected.",
	/* 17 */    "';' or 'end' expected.",
	/* 18 */    "'do' expected.",
	/* 19 */    "Incorrect symbol.",
	/* 20 */    "Relative operators expected.",
	/* 21 */    "Procedure identifier can not be in an expression.",
	/* 22 */    "Missing ')'.",
	/* 23 */    "The symbol can not be a primary expression.",
	/* 24 */    "The symbol can not be as the beginning of an expression.",
	/* 25 */    "The number is too great.",
	/* 26 */    "Operand is not l-value.",
	/* 27 */    "Missing '('.",
	/* 28 */    "Invalid identifier.",
	/* 29 */    "",
	/* 30 */    "",
	/* 31 */    "",
	/* 32 */    "There are too many levels."
};

//////////////////////////////////////////////////////////////////////
char ch;         // last character read
int  sym;        // last symbol read
char id[MAXIDLEN + 1]; // last identifier read
int  num;        // last number read
int  cc;         // character count
int  ll;         // line length
int  kk;
int  err;
int  cx;         // index of current instruction to be generated.
int  level = 0;
int  tx = 0;

int presym;
int dimDecl = 0;
int readDim = 0;

int  arrDim[1000];
int  adx = 0;

void calAdd(int i);

char line[80];

instruction code[CXMAX];

char *word[NRW + 1] =
{
	"", /* place holder */
	"begin",
	"const",
	"do",
	"end",
	"if",
	"odd",
	"function",
	"var",
	"while",

	"else",    //else
	"exit",    //exit
	"return",   //return
	"for",    //for
	"random",   //random
	"print",    //print
	"goto",    //goto
	"break",   //break
	"continue",   //continue
	"switch"   //switch
};

int wsym[NRW + 1] =
{
	SYM_NULL,
	SYM_BEGIN,
	SYM_CONST,
	SYM_DO,
	SYM_END,
	SYM_IF,
	SYM_ODD,
	SYM_FUNCTION,
	SYM_VAR,
	SYM_WHILE,

	SYM_ELSE,
	SYM_EXIT,
	SYM_RETURN,
	SYM_FOR,
	SYM_RANDOM,
	SYM_PRINT,
	SYM_GOTO,
	SYM_BREAK,
	SYM_CONTINUE,
	SYM_SWITCH
};

char csym[NSYM + 1] =
{
	' ',
	'(',
	')',
	'=',
	',',
	'.',
	';',

	'!',   //not
	'~',   //bit not
	'[',   //lsqrbra
	']',   //rsqrbra
	'?',   //question
};

int ssym[NSYM + 1] =
{
	SYM_NULL,
	SYM_LPAREN,
	SYM_RPAREN,
	SYM_EQU,
	SYM_COMMA,
	SYM_PERIOD,
	SYM_SEMICOLON,

	SYM_NOT,
	SYM_BNOT,
	SYM_LSQRBRA,
	SYM_RSQRBRA,
	SYM_QUESTION,
};

#define MAXINS   15
char *mnemonic[MAXINS] =
{
	"LIT",
	"OPR",
	"LOD",
	"STO",
	"CAL",
	"INT",
	"JMP",
	"JPC",
	"POP",
	"XCHG",
	"INC",
	"DEC",
	"LEAVE",
	"RET",
	"SETRET"
};

typedef struct
{
	char name[MAXIDLEN + 1];
	int  kind;
	int  value;
	int  arr_addr;
} comtab;

comtab table[TXMAX];

typedef struct
{
	char  name[MAXIDLEN + 1];
	int   kind;
	int   arr_addr;
	short level;
	short address;
} mask;

FILE* infile;

// EOF PL0.h
