#include "types.h"
#include "utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_VAR_SZIE 100

static VAR global_var[MAX_VAR_SZIE];
static int gvar_idx = 0;

static VAR scoop_var[MAX_VAR_SZIE];
static int svar_idx = 0;

static VAR macro_var[MAX_VAR_SZIE];
static int mvar_idx = 0;



/* ---------------------- ALL OF THE NAMES ---------------------- */

static char saved_names[256][NAME_MAX];
static int names_idx = 0;

static char saved_scoop_names[256][NAME_MAX];
static int scoop_names_idx = 0;



int is_name_used(char name[], int scoop){
	int max = scoop == 1 ? scoop_names_idx : names_idx;
	for(int i = 0; i < max; ++i){
		if(scoop){
			if(strcmp(saved_scoop_names[i], name) == 0){
				return 1;
			}
		} else {
			if(strcmp(saved_names[i], name) == 0){
				return 1;
			}
		}
	}
	return 0;
}


int name_avilable(char name[]){
	return is_name_used(name, 0);
}

void check_name(TKNS *tkns, char name[], int back){
	if(is_name_used(name, 0) == 0){
		tkns->idx = tkns->idx - back;
		throw_err(tkns, "variable not found", NULL);
		exit(0);
	}
}


int save_name(char name[]){
	if(is_name_used(name, 0) == 0){
		strcpy(saved_names[names_idx++], name);
		return 0;
	}
	return 1;
}

int save_scoop_name(char name[]){
	if(is_name_used(name, 1) == 0){
		strcpy(saved_scoop_names[scoop_names_idx++], name);
		return 0;
	}
	return 1;
}



void update_name_state(TKNS *tkns, char name[], int scoop){
	int used = scoop ? save_scoop_name(name) : save_name(name);
	if(used){
		throw_err(tkns, "This name alraedy been used", NULL);
		exit(0);
	}
}


void clear_scoop_names(void){
	scoop_names_idx = 0;
}


/* ---------------------- ALL OF THE NAMES ---------------------- */

int name_exists(char name[], VAR scoop[], int size){
	int i;
	for(i = 0; i < size; ++i){
		if(strcmp(name, scoop[i].name) == 0){
			return 1;
		}
	}
	return 0;
}


int identifier_exists(char name[], id_type id){
	switch (id) {
		case ID_VARIABLE_NAME:
			return name_exists(name, global_var, gvar_idx);

		case ID_MACRO_NAME:
			return name_exists(name, macro_var, mvar_idx);
			break;

		case ID_SCOOP_NAME:
			return 0;
			break;

		case ID_FUNCTION_NAME:
			return 0;
			break;

		default:
			return 1;
	}
	return 1;
}




int save_identifier(VAR v, id_type id){
	// save name
	if(save_name(v.name)){ return 1; }

	switch (id) {
		case ID_VARIABLE_NAME:
			break;

		case ID_MACRO_NAME:
			global_var[gvar_idx++] = v;
			break;

		case ID_SCOOP_NAME:
			break;

		case ID_FUNCTION_NAME:
			break;

		default:
			return 1;
	}
	return 1;
}



/* ---------------------- GLOBAL VARIABLES ---------------------- */

int save_global_variable(VAR v){
	if(name_exists(v.name, global_var, gvar_idx)){
		return 1;
	}
	global_var[gvar_idx++] = v;
	return 0;
}


/* ---------------------- MACRO ---------------------- */

int save_macro_variable(VAR v){
	if(name_exists(v.name, global_var, gvar_idx)){
		return 1;
	}
	global_var[gvar_idx++] = v;
	return 0;
}


/* ---------------------- SCROOP ---------------------- */

int save_scoop_variable(VAR v){
	if(name_exists(v.name, scoop_var, svar_idx)){
		return 1;
	}
	scoop_var[svar_idx++] = v;
	return 0;
}





/* ---------------------- DEBUG ---------------------- */

void show_global_variables(void){
	printf("GLOBAL VARIABLES:\n");
	for(int i = 0; i < gvar_idx; ++i){
		printf("  %d. %s:", i + 1, global_var[i].name);
		switch (global_var[i].type) {
			case INT_VAR:
				printf(" <INT> ");
				printf("%7s%d\n", "", global_var[i].value);
				break;

			case CHAR_VAR:
				printf(" <CHAR> ");
				printf("%6s\'%c\'\n", "", global_var[i].value);
				break;

			case STR_VAR:
				printf(" <STRING> ");
				printf("%4s\"%s\"\n", "", global_var[i].str_value);
				break;
		}
	}
	putchar('\n');
}

