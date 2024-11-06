#include "global.h"
#include "emission.h"


void generator(void){
	int i;
	int length = 0;

	int if_detected = 0;

	for(i = 0; i < ast_len(); ++i){
		char tmpc[256] = { 0 };  // Temp Code
		int l = 0;

		// printf("AST %d\n", i + 1);

		if(if_detected != 1 && asts[i].type != AST_ELSE_STATEMENT){ if_detected = 0; }

		switch (asts[i].type) {

			case AST_VARIABLE_ASSIGNMENT:
			case AST_RAW_ASM:
			case AST_FUNCTION_ASSIGNMENT:
			case AST_RETURN_STATEMENT:
				code_emission(asts[i], tmpc, &l);
				add_to_tree(tmpc);
				length += l;
				break;

			case AST_IF_STATEMENT:
				code_emission(asts[i], tmpc, &l);
				add_to_tree(tmpc);
				memset(tmpc, '\0', sizeof(tmpc));
				length += l;

				int if_idx = i + 1;
				int if_lenght = 0;
				char if_part[100][128] = { 0 };
				int if_part_idx = 0;
				int total_if = 0;

				while(asts[if_idx].refer == AST_IF_STATEMENT){
					char inner[256] = { 0 };
					code_emission(asts[if_idx], inner, &if_lenght);
					// add_to_tree(inner);
					strcpy(if_part[if_part_idx++], inner);
					total_if += if_lenght;
					++if_idx;
				}

				// int added_elements = if_idx - 1;
				if(total_if == 0){
					strcatf(tmpc, "NOP ; if body is empty");
					// add_to_tree(tmpc);
					strcpy(if_part[if_part_idx++], tmpc);
				}


				// printf("%s\n", total_if == 0 ? "NO IF ELEMENT" : "SOME IF EMELEMNTS");

				i = if_idx - 1;


				int else_idx = 0;
				int else_lenght = 0;
				char else_part[100][128] = { 0 };
				int else_part_idx = 0;
				int total_else = 0;

				int else_exists = asts[if_idx].refer == AST_ELSE_STATEMENT;

				if(else_exists){
					else_idx = i + 1;
					while(asts[else_idx].refer == AST_ELSE_STATEMENT){
						if(asts[else_idx].type == AST_ELSE_STATEMENT){ else_idx++; continue; }
						char inner[256] = { 0 };
						code_emission(asts[else_idx], inner, &else_lenght);
						strcpy(else_part[else_part_idx++], inner);
						total_else += else_lenght;
						++else_idx;
					}
				}

				if(total_else == 0){
					strcatf(tmpc, "NOP ; else body is empty");
					strcpy(else_part[else_part_idx++], tmpc);
					total_else = 1;
				}
				// memset(tmpc, '\0', sizeof(tmpc));


				// printf("LENGTH : %d\n", length);

				// if(else_exists){
					memset(tmpc, '\0', sizeof(tmpc));
					strcatf(tmpc, "\tGOTO 0x%x", length + total_else);
					add_to_tree(tmpc);

					for(int j = 0; j < else_part_idx; ++j){
						add_to_tree(else_part[j]);
					}

				// }


				memset(tmpc, '\0', sizeof(tmpc));
				strcatf(tmpc, "\tGOTO 0x%x", length + total_else + total_if);
				add_to_tree(tmpc);

				for(int j = 0; j < if_part_idx; ++j){
					add_to_tree(if_part[j]);
				}
				

				break;

			default: break;
		};
	}

	printf("LENGTH: %d\n", length);
}

