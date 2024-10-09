#include "rules.h"
#include <limits.h>


typedef struct GFLAG {
	char output[PATH_MAX];
	char input[PATH_MAX];
} GFLAG;


typedef enum token_t {
	INT_KEYWORD,       // int
	CHAR_KEYWORD,      // char
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
	HASHTAG,           // #
	WHITESPACE,        // <space>, ' '
	NEWLINE,           // <enter>, '\n'
	TYPE_KEYWORDS,     // 'int', 'char', 'string'
	INCLUDE_KEWORD,    // include keyword
	DEFINE_KEWORD,     // define keyword
	IF_KEWORD,         // if keyword
	ELSE_KEWORD,       // else keyword
	RETURN_KEWORD,       // return keyword
	GOTO_KEWORD,       // return keyword
	FOR_KEWORD,        // for keyword
	WHILE_KEWORD,      // while keyword
	COMMENT_TOK,       // comment token
	LEFT_SIGN,         // <
	RIGHT_SIGN,        // >
	PLUS_SIGN,         // +
	MINUS_SIGN,        // -
	TILDE_SIGN,        // ~
	COMMA_SIGN,        // ,
	UNKNOWN,           // Unknown
} token_t;


typedef struct {
	int row;
	int col;
	char word[NAME_MAX];
	token_t type;
} TOKEN;


typedef struct {
	TOKEN tokens[MAXTOKENS];
	int max;
	int idx;
} TKNS;



// typedef enum {
// 	INT_TYPE,
// 	CHAR_TYPE,
// 	INVALID_TYPE
// } variable_t;




typedef enum {
	INT_VAR,
	CHAR_VAR,
	STR_VAR,
	// UNRECOGNIZED_VAR,
} var_t;


typedef struct {
	char name[NAME_MAX];
	var_t type;
} ARG;


typedef struct{
	ARG args[MAX_ARG];
	int arg_len;
	TKNS body;
	var_t return_type;
} func_t;


typedef struct {
	char name[NAME_MAX];   // name
	char str[STR_MAX];         // string value
	int value;             // int/char value
	int is_func;           // is function
	int is_str;            // is string
	func_t func;       // function's object
	var_t type;       // vairlabes type
} ASGMT;




typedef enum {
	INT_CONST,
	CHAR_CONST,
	STRING_CONST,
} cvar_t;


typedef struct {
	char str_value[STR_MAX];
	int int_value;
	char char_value;
	cvar_t type;
} CNST_VAR;


typedef enum {
	INCLUDE_MACRO,
	DEFINE_MACRO,
} mcro_t;

typedef struct {
	char name[NAME_MAX];
	CNST_VAR value;
	mcro_t type;
} MACRO;


// expression action
typedef enum {
	EQUAL_OP,           // ==
	ASSIGN_OP,          // =
	SMALLER_OP,         // <
	GREATOR_OP,         // >
	SMALLER_EQ_OP,      // <=
	GREATOR_EQ_OP,      // >=
	ADD_OP,             // +
	MINUS_OP,           // -
	INCREMENT_OP,       // ++
	DECREMENT_OP,       // --
	SHIFT_RIGHT_OP,     // >>
	SHIFT_LEFT_OP,      // <<
	COMPLEMENT_OP,      // ~
	NO_OP,              // No operator
	INVALID_OP          // invalid(err) operator
} operator;

typedef struct {
	char left[STR_MAX];
	char right[STR_MAX];
	operator op;
} STMT;


typedef struct{
	STMT init;
	STMT cond;
	STMT iter;
	TKNS body;
} FOR_ASGMT;

typedef enum {
	IF_BODY,
	ELSE_BODY,
	WHILE_BODY,
} body_t;

typedef struct{
	STMT cond;
	TKNS body;
	body_t type;
} BODY_ASGMT;


typedef struct {
	char name[NAME_MAX];
	int value;
	char str_value[STR_MAX];
	var_t type;
} VAR;


typedef enum {
	ID_FUNCTION_NAME,
	ID_VARIABLE_NAME,
	ID_SCOOP_NAME,
	ID_MACRO_NAME,
} id_type;

