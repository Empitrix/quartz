typedef struct GFLAG {
	char *output;
	char *input;
} GFLAG;


typedef enum diagnostics {
	TERMINATE_DIAG,
	WARNING_DIAG
} diags;


typedef struct Token {
	int row;
	int col;
	char *word;
} Token;


typedef struct LINES {
	char **lines;
	int len;
} LINES;

