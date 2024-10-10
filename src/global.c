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


static func_t functions[MAX_VAR_SZIE];
static int func_idx = 0;


static target_t save_target = GLOBAL_TARGET;

void save_func_global(func_t func){
	functions[func_idx++] = func;
}



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

/* check if name exits if exitst return 1 otherwise return 0 */
int name_exists(char name[], VAR variables[], int size){
	int i;
	for(i = 0; i < size; ++i){
		if(strcmp(name, variables[i].name) == 0){
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



/* ---------------------- VARIABLES ---------------------- */

int save_global_variable(VAR v){
	if(name_exists(v.name, global_var, gvar_idx)){
		return 1;
	}
	global_var[gvar_idx++] = v;
	return 0;
}


int save_scoop_variable(VAR v){
	if(name_exists(v.name, scoop_var, svar_idx)){
		return 1;
	}
	scoop_var[svar_idx++] = v;
	return 0;
}


void set_variable_target(target_t target){ save_target = target; }

int save_variable(VAR v){
	if(save_target == GLOBAL_TARGET){
		return save_global_variable(v);
	} else {
		return save_scoop_variable(v);
	}
	return 0;
}

int get_variable(char name[], target_t t, VAR *var){
	int max = t == GLOBAL_TARGET ? gvar_idx : svar_idx;
	for(int i = 0; i < max; ++i){
		if(strcmp((t == GLOBAL_TARGET ? global_var : scoop_var)[i].name, name) == 0){
			*var = (t == GLOBAL_TARGET ? global_var : scoop_var)[i];
			return 1;
		}
	}
	return 0;
}


/* for scoop and global (usable variables) */
int variable_exits(char name[], CNST_VAR *var){

	VAR v;
	
	if(get_variable(name, SCOOP_TARGET, &v)){
		convert_var_to_const(&v, var);
		return 1;
	}

	if(get_variable(name, GLOBAL_TARGET, &v)){
		convert_var_to_const(&v, var);
		return 1;
	}

	// if (name_exists(name, scoop_var, svar_idx)){
	// 	VAR v;
	// 	if(get_variable(name, SCOOP_TARGET, &v)){
	// 		convert_var_to_const(&v, var);
	// 		return 1;
	// 	}
	// }
	// if (name_exists(name, global_var, gvar_idx)){
	// 	VAR v;
	// 	if(get_variable(name, GLOBAL_TARGET, &v)){
	// 		convert_var_to_const(&v, var);
	// 		return 1;
	// 	}
	// }

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






int func_exists(const char name[], int force){
	for(int i = 0; i < func_idx; ++i){
		if(strcmp(functions[i].name, name) == 0){
			return 1;
		}
	}
	if(force){
		printf("Function '%s' not found\n", name);
		exit(0);
	}
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


void show_functions(void){
	printf("FUNCTIONS:\n");
	for(int i = 0; i < func_idx; ++i){
		printf("  %d. ", i + 1);
		printf("%s -> ", functions[i].name);
		char type[20];
		type_to_str(functions[i].return_type, type);
		printf("%s: ", type);
		for(int j = 0; j < functions[i].arg_len; ++j){
			type_to_str(functions[i].args[j].type, type);
			printf("%s:%s%s", functions[i].args[j].name, type, j == functions[i].arg_len - 1 ? "" : ", ");
		}
		putchar('\n');
	}
	putchar('\n');
}

