#include "types.h"
#include <string.h>

#define MAX_VAR_SZIE 100



/* ****************************** QVAR ****************************** */
static Qvar global_qvar[MAX_VAR_SZIE];  // Global Qvar
static int global_q_idx = 0;

static Qvar local_qvar[MAX_VAR_SZIE];  // Local Qvar
static int local_q_idx = 0;

static Qfunc global_qfunc[MAX_VAR_SZIE];  // Global Qfunc
static int qfunc_idx = 0;



int qvar_exists(char name[], Qstack stack){
	if(stack == GLOBAL_STACK){
		for(int i = 0; i < global_q_idx; ++i){
			if(strcmp(name, global_qvar[i].name) == 0){
				return 1;
			}
		}
	} else if (stack == LOCAL_STACK){
		for(int i = 0; i < local_q_idx; ++i){
			if(strcmp(name, local_qvar[i].name) == 0){
				return 1;
			}
		}
	} else {
		// Check for Both Global/Local by recursive calling
		int result = 0;
		result = qvar_exists(name, GLOBAL_STACK);

		if(result){
			return result;
		} else {
			return qvar_exists(name, LOCAL_STACK);
		}

	}
	return 0;
}

/* save_qvar: save Q-var return 0 if everything goes fine (not exitst) otherwise return 1 */
int save_qvar(Qvar q, Qstack stack){
	if(qvar_exists(q.name, GLOBAL_LOCAL_STACK)){ return 1; }

	if(stack == GLOBAL_STACK || stack == GLOBAL_LOCAL_STACK){
		global_qvar[global_q_idx++] = q;
	} else {
		local_qvar[local_q_idx++] = q;
	}

	return 0;
}


/* get_qvar: return 0 if Q-var exists otherwise return 1 (save qvar to *q)*/
int get_qvar(char name[], Qvar *q){
	int i;

	for(i = 0; i < global_q_idx; ++i){
		if(strcmp(global_qvar[i].name, name) == 0){
			*q = global_qvar[i];
			return 0;
		}
	}

	for(i = 0; i < local_q_idx; ++i){
		if(strcmp(local_qvar[i].name, name) == 0){
			*q = local_qvar[i];
			return 0;
		}
	}

	return 1;
}


int qfunc_exists(char name[]){
	for(int i = 0; i < qfunc_idx; ++i){
		if(strcmp(global_qfunc[i].name, name) == 0){
			return 1;
		}
	}

	if(qvar_exists(name, GLOBAL_LOCAL_STACK)){
		return 2;
	}

	return 0;
}

int save_qfunc(Qfunc qf){
	int result = 0;
	if((result = qfunc_exists(qf.name))){ return result; }
	global_qfunc[qfunc_idx++] = qf;
	return 0;
}

int get_qfunc(char name[], Qfunc *qf){
	if(qfunc_exists(name) == 0){ return 1; }  // Func not exists
	for(int i = 0; i < qfunc_idx; ++i){
		if(strcmp(global_qfunc[i].name, name) == 0){
			*qf = global_qfunc[i];
			return 0;
		}
	}
	return 1;
}
