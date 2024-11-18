#include "rules.h"
#include <limits.h>
#include <stdio.h>



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


typedef struct {
	char name[32];
	int addr;
} ASM_VAR;


typedef enum {
	NOT_EFFECTIVE_SNIP,  // Will be ignored by compiler (1 + 1;)
	CONDITIONAL_SNIP,    // Conditional snippet (==, !=, <=, >=, <, >)
	ITTERATIONAL_SNIP,   // itterational snippet (++, --)
	ASSIGNMENT_SNIP,     // Assigned snippet (+=, -=, =)
	ARITHMETIC_SNIP,     // Arithmetic snippet (+, -, >>, <<, ~, ^)
	FUNCTION_CALL_SNIP,  // Function call
} snip_t;

typedef enum {
	NO_ASSIGNMENT_ASG,
	FUNCTION_ASSIGNMENT_ASG,
	VARIABLE_ASSIGNMENT_ASG,
	MACRO_ASSIGNMENT_ASG,
	NEW_ASSIGNMENT_ASG,
	ASSIGNMENT_ASG,
	UPDATE_AST,
} asg_t;


typedef struct {
	char name[NAME_MAX];  // Argument's name
	qvar_t type;          // Argument's type
	int addr;             // Argument's Address (RAM)
} Qarg;

typedef struct {
	char name[NAME_MAX];  // Function name
	Qarg args[MAX_ARG];   // Arguments for function
	int arg_len;          // Number of arguments
	qvar_t return_type;   // Return type
	TKNS body;            // Body's token
} Qfunc;

typedef struct {
	snip_t type;         // Snippet type
	operator op;         // Operator that used between left and right
	Qvar left;           // Left Q-var
	Qvar right;          // Right Q-var
	asg_t assigne_type;  // Assignment type
	Qvar assigned;       // Assigned Q-var
	Qfunc func;          // Function
	Qvar args[MAX_ARG];  // Function arguments
	int arg_len;         // Argument's length
} SNIP;

SNIP empty_snip(){
	SNIP s;
	s.op = NO_OP;
	s.left = empty_qvar();
	s.right = empty_qvar();
	s.assigned = empty_qvar();
	s.arg_len = 0;
	s.assigne_type = NO_ASSIGNMENT_ASG;
	s.type = NOT_EFFECTIVE_SNIP;
	s.func.arg_len = 0;
	s.func.return_type = CONSTANT_INTEGER;
	return s;
}


typedef enum {
	LOCAL_STACK,
	GLOBAL_STACK,
	GLOBAL_LOCAL_STACK,
} Qstack;

// For
typedef struct {
	SNIP init;
	SNIP cond;
	SNIP iter;
	TKNS body;
} Qfor;


// While
typedef struct {
	SNIP cond;
	TKNS body;
} Qwhile;


// If-Else
typedef struct {
	SNIP cond;
	TKNS if_body;
	int contains_else;
	TKNS else_body;
} Qif;


typedef struct {
	SNIP snip;
	Qvar var;
	Qwhile qwhile;
	Qif qif;
	Qfor qfor;
	char rasm[124];

	ast_t type;
	ast_t refer;
	int indent;
} Qast;


static Qast qasts[500];
static int qast_idx = 0;


Qast *empty_ast(){
	Qast *ast = &qasts[qast_idx];
	qast_idx++;
	ast->snip.arg_len = 0;
	ast->var = empty_qvar();
	ast->qwhile.cond = empty_snip();
	ast->qif.cond = empty_snip();
	ast->qif.contains_else = 0;
	ast->qfor.cond = empty_snip();
	ast->type = AST_NO_STATEMENT;
	ast->refer = AST_NO_STATEMENT;
	ast->indent = 0;
	return ast;
}
