#ifndef __QUARTZ_RULES_C__
#define __QUARTZ_RULES_C__
#define MAXSIZ 1024

#define MAXFILE 1024

#define MAXTOKENS 1024

#define STR_MAX 256
#define NAME_MAX 32

#define MAX_ARG 10

#define MAX_AST 100

#define MAX_RAM 15  // Total is 16 but the last address is reserved for compiler (0x19)

#define CRAM 0x19   // Compiler reserved RAM
#define CRAMS "CRAM"    // Compiler reserved RAM (STRING)

#define COMPILE_NAME "a.asm"

#include "rules.c"
#endif

