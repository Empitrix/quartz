#include "rules.h"
#include <limits.h>


typedef struct GFLAG {
	char output[PATH_MAX];
	char input[PATH_MAX];
} GFLAG;


typedef enum token_t {
	INT_KEYWORD,       // int
	IDENTIFIER,        // word, name, y2k, another_compiler ...
	INTEGER_VALUE,     // 255, 19, ... 
	EQUAL_SIGN,        // =
	SEMICOLON_SIGN,    // ;
	END_SIGN,          // ;
	BRAKET_OPN,        // [
	BRAKET_CLS,        // ]
	BRACE_OPN,         // {
	BRACE_CLS,         // }
	PAREN_OPN,         // (
	PAREN_CLS,         // )
	SINGLE_QUOTE,      // '
	DOUBLE_QUOTE,      // "
	WHITESPACE,        // <space>, ' '
	NEWLINE,           // <enter>, '\n'
	UNKNOWN,           // Unknown
	TYPE_KEYWORDS,     // 'int', 'char', 'string'
} token_t;


typedef struct {
	int row;
	int col;
	char word[32];
	token_t type;
} TOKEN;


typedef struct {
	TOKEN tokens[MAXTOKENS];
	int max;
	int idx;
} TKNS;


typedef struct LINES {
	char **lines;
	int len;
} LINES;


typedef enum {
	INT_TYPE,
	CHAR_TYPE,
	INVALID_TYPE
} variable_t;


typedef struct {
	int len;
	char **names;
	variable_t *types;
} ARGS;


typedef struct{
	char name[32];
	ARGS args;
	variable_t return_type;
} function_t;


typedef struct {
	char name[32];     // name
	char str[256];     // string value
	int value;         // int/char value
	int is_func;       // is function
	int is_str;        // is string
	function_t func;   // function's object
	variable_t type;   // vairlabes type
} ASGMT;

