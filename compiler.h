#ifndef __QUARTZ_COMPILER__
#define __QUARTZ_COMPILER__


#include "src/types.h"
#include "src/utility.h"
#include "src/lexer.h"
#include "src/generator.h"
#include "src/parser.h"


void tokenizer(char *input, TKNS *tkns);
void add_tree(const char *inpt);
void qparser(TKNS *tkns, int allow_expr, ast_t refer);
void update_children(void);
int qfunc_exists(char *name);
void generator(Qast *asts, int start, int end);
void reorder(void);


void strcatf(char* dst, const char * frmt, ...);

void compiler_clean(void);

#endif
