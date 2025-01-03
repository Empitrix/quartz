#include "rules.h"
#include "types.h"
#include "utility.h"
#include "helper.h"
#include "emission.h"


void generator(Qast *asts, int start, int end);


/* generate_for: looks nodes after the current one that have a type of "type" and generate code for them */
void generate_for(Qast *asts, int *idx){
	int save = *idx;
	generator(asts, save + 1, *idx + asts[save].children + 1);
	*idx = *idx + asts[save].children;
}



// // Between a and a loads var with no address to W (if both have address load one of them to W)
// Qvar *lltw(Qvar *a, Qvar *b);
// Qvar *lftw(Qvar *a, Qvar *b);


static int main_detect = 0;

void generator(Qast *asts, int start, int end){
	int i;

	for(i = start; i < end; ++i){

		// Variable and macro
		if(asts[i].type == AST_VARIABLE_ASSIGNMENT){
			if(asts[i].snip.assigne_type == MACRO_ASSIGNMENT_ASG){
				attf("%s EQU 0x%.2X", asts[i].snip.assigned.name, asts[i].snip.assigned.addr);
			} else {
				attf("%s EQU 0x%.2X", asts[i].snip.assigned.name, asts[i].snip.assigned.addr);
				attf("MOVLW 0x%.2X", asts[i].snip.assigned.numeric_value);
				attf("MOVWF %s", asts[i].snip.assigned.name);
			}


		} else if(asts[i].type == AST_FUNCTION_ASSIGNMENT){
			attf("%s:", asts[i].snip.assigned.name);
			main_detect = strcmp(asts[i].snip.assigned.name, "main") == 0;
			generate_for(asts, &i);


		} else if(asts[i].type == AST_IF_STATEMENT){
			set_condition(&asts[i].qif.cond, 0);  // set conditions (no reverse)
			const char *if_l = get_label();
			const char *else_l = get_label();
			const char *skip_l = get_label();
			attf("\tGOTO %s", if_l);             // Goto EQUAL
			attf("\tGOTO %s", else_l);           // Goto NOT EQUAL
			attf("%s:", if_l);                   // Set "if" label
			int save = i;
			if(empty_body(&asts[i].qif.if_body)){
				attf("\tNOP ; empty \"if\" body");
			} else {
				generate_for(asts, &i);
			}

			attf("\tGOTO %s", skip_l);           // GOTO skip labe (end of equal)
			attf("%s:", else_l);                 // Set "else" label

			if(empty_body(&asts[save].qif.else_body)){
				attf("\tNOP ; empty \"else\" body");
			} else {
				++i;  // SKIP "else" keyword
				generate_for(asts, &i);
			}
			attf("%s:", skip_l);  // set "SKIP" label


		} else if(asts[i].type == AST_FOR_LOOP_ASSIGNMENT){
			const char *top_loop = get_label();
			const char *skip_loop = get_label();

			gen_assign(&asts[i].qfor.init);           // generate init part
			attf("%s:", top_loop);                    // Set top of the loop label (no init part)
			set_condition(&asts[i].qfor.cond, 0);     // set conditions
			attf("\tGOTO %s", skip_loop);             // skip the loop (if not valid)
			int save = i;
			generate_for(asts, &i);                   // Generate for body
			gen_iter(&asts[save].qfor.iter);          // Generate iter part
			attf("\tGOTO %s", top_loop);              // Goto top of the loop
			attf("%s:", skip_loop);                   // label to skip the loop


		} else if(asts[i].type == AST_WHILE_LOOP_ASSIGNMENT){
			const char *top_loop = get_label();
			const char *skip_loop = get_label();
			attf("%s:", top_loop);                      // Set a label for top of the loop
			set_condition(&asts[i].qwhile.cond, 0);     // Set conditions
			attf("\tGOTO %s", skip_loop);               // Exit the loop (test failed)
			generate_for(asts, &i);                     // Generate for "while" body
			attf("\tGOTO %s", top_loop);                // Goto top of the loop
			attf("%s:", skip_loop);                     // Set a label to skip the loop


		} else if(asts[i].type == AST_FUNCTION_CALL){
			assign_func_arg(&asts[i]);
			attf("\tCALL %s", asts[i].snip.func.name);


		} else if(asts[i].type == AST_RAW_ASM){
			attf("\t%s", asts[i].rasm);


		} else if(asts[i].type == AST_RETURN_STATEMENT){
			if(main_detect){
				attf("\tSLEEP  ; End of the program (main func)");
			} else {
				attf("\tRETLW 0x%.2X", asts[i].var.numeric_value);
			}


		} else if(asts[i].type == AST_STATEMENT){


			if(asts[i].snip.type == ITTERATIONAL_SNIP){
				// ++ and --
				gen_iter(&asts[i].snip);

			} else if(asts[i].snip.assigne_type == UPDATE_ASG){
				if(const_type(asts[i].snip.assigned.type) != 0){ continue; }

				// =
				if(asts[i].snip.op == ASSIGN_OP){
					load_var(&asts[i].snip.left);
					attf("\tMOVWF %s", asts[i].snip.assigned.name);
					// attf("\tMOVWF 0x%.2X", asts[i].snip.assigned.addr);

				// +
				} else if(asts[i].snip.op == ADD_OP){
					Qvar *lq = load_to_w(&asts[i].snip.left, &asts[i].snip.right);
					attf("\tADDWF 0x%.2X, W", lq->addr);
					attf("\tMOVWF 0x%.2X", asts[i].snip.assigned.addr);

				// &
				} else if(asts[i].snip.op == AND_OP){
					Qvar *lq = load_to_w(&asts[i].snip.left, &asts[i].snip.right);
					attf("\tANDWF 0x%.2X, W", lq->addr);
					attf("\tMOVWF 0x%.2X", asts[i].snip.assigned.addr);

				// |
				} else if(asts[i].snip.op == OR_OP){
					Qvar *lq = load_to_w(&asts[i].snip.left, &asts[i].snip.right);
					attf("\tIORWF 0x%.2X, W", lq->addr);
					attf("\tMOVWF 0x%.2X", asts[i].snip.assigned.addr);

				// ^
				} else if(asts[i].snip.op == XOR_OP){
					Qvar *lq = load_to_w(&asts[i].snip.left, &asts[i].snip.right);
					attf("\tXORWF 0x%.2X, W", lq->addr);
					attf("\tMOVWF 0x%.2X", asts[i].snip.assigned.addr);

				// -
				} else if(asts[i].snip.op == MINUS_OP){
					load_cram(&asts[i].snip.left);
					load_var(&asts[i].snip.right);
					attf("\tSUBWF 0x%.2X, W", CRAM);
					attf("\tMOVWF 0x%.2X", asts[i].snip.assigned.addr);

				} else if(asts[i].snip.op == SHIFT_LEFT_OP){
					set_shift(&asts[i].snip, "RLF");

				} else if(asts[i].snip.op == SHIFT_RIGHT_OP){
					set_shift(&asts[i].snip, "RRF");

				}


			} else if(asts[i].snip.assigne_type == ASSIGNMENT_ASG){
				if(const_type(asts[i].snip.assigned.type) == 0){ continue; }

				// +=
				if(asts[i].snip.op == ADD_ASSIGN_OP){
					load_var(&asts[i].snip.right);
					attf("\tADDWF 0x%.2X, F", asts[i].snip.left.addr);

				// -=
				} else if(asts[i].snip.op == MINUS_ASSIGN_OP){
					load_var(&asts[i].snip.right);
					attf("\tSUBWF 0x%.2X, F", asts[i].snip.left.addr);
				}


			}


		}


	}

}

