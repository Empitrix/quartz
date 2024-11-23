#include "rules.h"
#include "types.h"


void gen_assign(SNIP *snip){
	// if(snip->type != ASSIGNMENT_SNIP || snip->assigne_type == NO_ASSIGNMENT_ASG){ return; }
	if((snip->type != ASSIGNMENT_SNIP || snip->type != NOT_EFFECTIVE_SNIP) &&
		snip->assigne_type == NO_ASSIGNMENT_ASG){ return; }

	if(snip->assigne_type == UPDATE_ASG){
		
		attf("\tMOVLW 0x%.2X", snip->left.numeric_value);
		attf("\tMOVWF %s", snip->assigned.name);
	}

}

void gen_iter(SNIP *snip){
	if(snip->type != ITTERATIONAL_SNIP){ return; }

	if(snip->op == INCREMENT_OP){
		attf("\tINCF %s, F", snip->left.name);
	} else {
		attf("\tDECF %s, F", snip->left.name);
	}
}
