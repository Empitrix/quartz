#include "global.h"
#include "emission.h"
#include "rules.h"
#include <stdio.h>
#include <string.h>


int extract_to(char[], ast_t, int *);


int get_scoop_length(int idx){
	int i;
	for(i = idx; i > 0; --i){
		if(asts[i].type == AST_FUNCTION_ASSIGNMENT){ break; }
	}
	// printf("IDX: %d\nI: %d\n", idx, i);
	return idx - i;
}


void generator(void){
	int i;
	int length = 0;
	int s_length = 0;

	int if_detected = 0;

	for(i = 0; i < ast_len(); ++i){
		char tmpc[256] = { 0 };  // Temp Code
		int l = 0;

		// printf("AST %d\n", i + 1);

		if(if_detected != 1 && asts[i].type != AST_ELSE_STATEMENT){ if_detected = 0; }

		if (asts[i].type == AST_VARIABLE_ASSIGNMENT ||
			asts[i].type == AST_RAW_ASM ||
			asts[i].type == AST_FUNCTION_ASSIGNMENT) {
				code_emission(asts[i], tmpc, &l);
				add_to_tree(tmpc);
				length += l;

		} else if(asts[i].type == AST_RETURN_STATEMENT){
				code_emission(asts[i], tmpc, &l);
				add_to_tree(tmpc);
				length += l;

				// printf("IDX: %d\nI: %d\n", length, get_scoop_length(i));
				// s_length = length - get_scoop_length(i) + 1;
				s_length += ( get_scoop_length(i) - length);
				// s_length--;
				// s_length = i - length;
				// s_length = length - i;

		} else if (asts[i].type == AST_IF_STATEMENT){
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
				strcpy(if_part[if_part_idx++], inner);
				total_if += if_lenght;
				++if_idx;
			}

			if(total_if == 0){
				strcatf(tmpc, "NOP ; if body is empty");
				strcpy(if_part[if_part_idx++], tmpc);
				length++;
			}


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
				length++;
				strcpy(else_part[else_part_idx++], tmpc);
				total_else = 1;
			}


			memset(tmpc, '\0', sizeof(tmpc));
			strcatf(tmpc, "\tGOTO 0x%x", length + total_else);
			length++;
			add_to_tree(tmpc);

			for(int j = 0; j < else_part_idx; ++j){
				add_to_tree(else_part[j]);
			}


			memset(tmpc, '\0', sizeof(tmpc));
			strcatf(tmpc, "\tGOTO 0x%x", length + total_else + total_if);
			length++;
			add_to_tree(tmpc);

			for(int j = 0; j < if_part_idx; ++j){
				add_to_tree(if_part[j]);
			}


		} else if(asts[i].type == AST_WHILE_LOOP_ASSIGNMENT){
			int top_of_while = s_length - 1;
			char while_sec[1024] = { 0 };
			code_emission(asts[i], while_sec, &l);
			int len = extract_to(tmpc, AST_WHILE_LOOP_ASSIGNMENT, &i);
			update_tree_lines(tmpc);


			add_to_tree(while_sec);
			length += l;


			if(len == 0){
				add_to_tree("NOP ; NO WHILE BODY");
				length++;
			} else {
				attf("\tGOTO 0x%x", top_of_while);
				length++;

			}
			attf("\tNOP  ; End of while", top_of_while);
			length++;
		}

	}

	printf("LENGTH: %d\n", length);
}




int extract_to(char dst[], ast_t type, int *index){
	int total_len = 0;
	int i = *index;
	int len = 0;
	i++;  // point to next instruction

	char part[100][128] = { 0 };
	int part_idx = 0;

	while(asts[i].refer == type){
		char inner[256] = { 0 };
		code_emission(asts[i], inner, &len);
		strcpy(part[part_idx++], inner);
		total_len += len;
		++i;
	}

	for(int j = 0; j < part_idx; ++j){
		strcat(dst, part[j]);
		if(j != part_idx - 1){ strcat(dst, "\n"); }
	}

	// *total = part_idx;
	*index = i - 1;  // go back by 1

	return total_len;
}


