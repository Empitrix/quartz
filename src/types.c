typedef struct GFLAG {
	char *output;
	char *input;
} GFLAG;


typedef enum diagnostics {
	TERMINATE_DIAG,
	WARNING_DIAG
} diags;


typedef enum token_t {
	INT_KEYWORD,       // int
	IDENTIFIER,        // word, name, y2k, ...
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
	char *word;
	token_t type;
} TOKEN;


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
	char *name;
	ARGS args;
	variable_t return_type;
} function_t;

typedef struct {
	char *name;
	char *str;
	int value;
	int is_func;
	int is_str;
	function_t func;
	variable_t type;
} ASGMT;


// typedef struct {
// 	token_t *rules;
// 	int len;
// } RULE;
// 
// 
// RULE get_rule(EXPR expr){
// 	RULE rule;
// 
// 	switch(expr){
// 		case VARIABLE_ASSIGNEMNT:
// 			rule.len = 5;
// 			rule.rules = (token_t *)calloc(rule.len, sizeof(token_t));
// 			rule.rules[0] = INT_KEYWORD;
// 			rule.rules[0] = TYPE_KEYWORDS;
// 			rule.rules[1] = IDENTIFIER;
// 			rule.rules[2] = EQUAL_SIGN;
// 			rule.rules[3] = INTEGER_VALUE;
// 			rule.rules[4] = END_SIGN;
// 			return rule;
// 		default:
// 			break;
// 	}
// 
// 	return rule;
// }

