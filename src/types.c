#include "rules.h"
#include <limits.h>


typedef struct GFLAG {
	char output[PATH_MAX];
	char input[PATH_MAX];
	char target[100];
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
	INCLUDE_KEYWORD,   // include keyword
	DEFINE_KEYWORD,    // define keyword
	IF_KEYWORD,        // if keyword
	ELSE_KEYWORD,      // else keyword
	RETURN_KEYWORD,    // return keyword
	VOID_KEYWORD,      // void keyword
	GOTO_KEYWORD,      // goto keyword
	FOR_KEYWORD,       // for keyword
	WHILE_KEYWORD,     // while keyword
	COMMENT_TOK,       // comment token
	LEFT_SIGN,         // <
	RIGHT_SIGN,        // >
	PLUS_SIGN,         // +
	MINUS_SIGN,        // -
	TILDE_SIGN,        // ~
	CARET_SIGN,        // ^
	AND_SIGN,          // &
	OR_SIGN,           // |
	EXCLAMATION_SIGN,  // !
	COMMA_SIGN,        // ,
	BACKTICK_SIGN,     // `
	UNKNOWN,           // Unknown `, ! ...
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


typedef enum {
	INT_VAR,
	CHAR_VAR,
	STR_VAR,
	MACRO_VAR,
} var_t;


typedef struct {
	char name[NAME_MAX];
	var_t type;
	int addr;
} ARG;


typedef struct{
	ARG args[MAX_ARG];
	char name[NAME_MAX];
	int arg_len;
	TKNS body;
	var_t return_type;
} func_t;


typedef struct {
	char name[NAME_MAX];   // name
	char str[STR_MAX];     // string value
	int value;             // int/char value
	int is_func;           // is function
	int is_str;            // is string
	int address;           // RAM's Address
	func_t func;           // function's object
	var_t type;            // vairlabes type
} ASGMT;




// typedef enum {
// 	INT_CONST,
// 	CHAR_CONST,
// 	STRING_CONST,
// } cvar_t;

typedef enum {
	CONSTANT_STRING,    // Str "Something..."
	CONSTANT_INTEGER,   // Int 123...
	CONSTANT_CHAR,      // Char 'A'
	QVAR_INT,           // Int
	QVAR_CHAR,          // Char
	QVAR_DEFINE,        // Define 
} qvar_t;


typedef struct {
	char name[NAME_MAX];      // Name
	int addr;                 // RAM's address
	qvar_t type;              // Q-Var type
	int numeric_value;        // Any kind of numeric value
	char const_str[STR_MAX];  // Constant string
} Qvar;


Qvar empty_qvar(void){
	Qvar q;
	q.addr = 0;
	strcpy(q.const_str, "");
	strcpy(q.name, "");
	q.type = CONSTANT_INTEGER;
	q.numeric_value = 0;
	return q;
}


void copy_qvar(Qvar dst, Qvar src){
	dst.addr = src.addr;
	strcpy(dst.const_str, src.const_str);
	strcpy(dst.name, src.name);
	dst.type = src.type;
	dst.numeric_value = src.numeric_value;
}


typedef struct {
	char name[NAME_MAX];
	char str_value[STR_MAX];
	int int_value;
	char char_value;
	var_t type;
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


// expressions
typedef enum {
	EQUAL_OP,           // ==
	NOT_EQUAL_OP,       // !=
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
	AND_OP,             // &
	OR_OP,              // |
	XOR_OP,             // ^
	ADD_ASSIGN_OP,      // +=
	MINUS_ASSIGN_OP,    // -=
	NO_OP,              // No operator
	INVALID_OP          // invalid(err) operator
} operator;

typedef struct {
	char left[STR_MAX];
	char right[STR_MAX];
	int literal;
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


typedef enum {
	GLOBAL_TARGET,
	SCOOP_TARGET,
} target_t;


typedef struct {
	int complement;   // is complemented (contains '~')
	int arithmetic;   // -1: decrement, 0: no arithmetic, 1: increment
	int value;
	CNST_VAR var;
} side_t;


typedef enum {
	EXPR_EMPTY,
	EXPR_ASSIGNABLE,
	EXPR_FUNCTION_CALL,
} expr_t;

typedef struct {
	side_t left;
	side_t right;
	operator op;
	int mono_side;
	char assign_name[NAME_MAX];
	// int is_assign;
	// int is_call;
	expr_t type;
	func_t caller;
	CNST_VAR args[10];
	int args_len;
} EXPR;



/* Abstract Syntax Tree (AST) type */
typedef enum {
	AST_NO_STATEMENT,             // no statement <empty>
	AST_VARIABLE_ASSIGNMENT,      // variable
	AST_FOR_LOOP_ASSIGNMENT,      // for loop
	AST_WHILE_LOOP_ASSIGNMENT,    // while loop
	AST_IF_STATEMENT,             // if
	AST_ELSE_STATEMENT,           // else
	AST_FUNCTION_ASSIGNMENT,      // function
	AST_RETURN_STATEMENT,         // return
	AST_FUNCTION_CALL,            // function call
	AST_STATEMENT,                // statement
	AST_RAW_ASM,                  // raw assembly
	AST_MACRO,                    // macro (define)
} ast_t;


typedef struct AST{
	func_t func;
	ASGMT asgmt;
	ast_t type;
	EXPR expr;
	char raw_asm[128];
	CNST_VAR value;
	ast_t refer;
	STMT cond;
	STMT init;
	FOR_ASGMT for_asgmt;
	MACRO macro;
} AST;


typedef struct {
	char name[32];
	int addr;
} ASM_VAR;


typedef enum {
	NOT_EFFECTIVE_SNIP,  // Will be ignored by compiler (1 + 1;)
	CONDITIONAL_SNIP,
	ITTERATIONAL_SNIP,
	ASSIGNMENT_SNIP,
	ARITHMETIC_SNIP,
} snip_t;

typedef enum {
	NO_ASSIGNMENT_ASG,
	FUNCTION_ASSIGNMENT_ASG,
	VARIABLE_ASSIGNMENT_ASG,
	MACRO_ASSIGNMENT_ASG,
	NEW_ASSIGNMENT_ASG,
	ASSIGNMENT_ASG,
	UPDATE_AST, // Define 
} asg_t;


typedef struct {
	char name[NAME_MAX];
	qvar_t type;
	int addr;
} Qarg;

typedef struct {
	Qarg args[MAX_ARG];
	int arg_len;
	qvar_t return_type;
	char name[NAME_MAX];
	TKNS body;
} Qfunc;

typedef struct {
	snip_t type;
	operator op;
	Qvar left;
	Qvar right;
	asg_t assigne_type;
	Qvar assigned;

	Qfunc func;
} SNIP;

/*
int a = 12;
char a = 'A';
char buff[] = "Quartz...";
a = 20;
a = 'B';
a = a | b;
a = a + 1;

a << 1;
i++;
i--;

i < 10;

a = 'A' + i;

*/


typedef enum {
	LOCAL_STACK,
	GLOBAL_STACK,
	GLOBAL_LOCAL_STACK,
} Qstack;

typedef struct {
	SNIP init;
	SNIP cond;
	SNIP iter;
	TKNS body;
} Qfor;


typedef struct {
	SNIP cond;
	TKNS body;
} Qwhile;


typedef struct {
	SNIP cond;
	TKNS if_body;
	int contains_else;
	TKNS else_body;
} Qif;

