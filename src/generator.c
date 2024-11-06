#include "global.h"
#include "emission.h"
#include "rules.h"
#include <stdio.h>


void generator(void){
	int i;
	int length = 0;

	int if_detected = 0;

	for(i = 0; i < ast_len(); ++i){
		char tmpc[256] = { 0 };  // Temp Code
		int l = 0;

		// printf("AST %d\n", i + 1);

		if(if_detected != 1 && asts[i].type != AST_ELSE_STATEMENT){ if_detected = 0; }

		if (asts[i].type == AST_VARIABLE_ASSIGNMENT ||
			asts[i].type == AST_RAW_ASM ||
			asts[i].type == AST_FUNCTION_ASSIGNMENT ||
			asts[i].type == AST_RETURN_STATEMENT) {
				code_emission(asts[i], tmpc, &l);
				add_to_tree(tmpc);
				length += l;

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
				strcpy(else_part[else_part_idx++], tmpc);
				total_else = 1;
			}


			memset(tmpc, '\0', sizeof(tmpc));
			strcatf(tmpc, "\tGOTO 0x%x", length + total_else);
			add_to_tree(tmpc);

			for(int j = 0; j < else_part_idx; ++j){
				add_to_tree(else_part[j]);
			}


			memset(tmpc, '\0', sizeof(tmpc));
			strcatf(tmpc, "\tGOTO 0x%x", length + total_else + total_if);
			add_to_tree(tmpc);

			for(int j = 0; j < if_part_idx; ++j){
				add_to_tree(if_part[j]);
			}
















		} else if(asts[i].type == AST_WHILE_LOOP_ASSIGNMENT){
			code_emission(asts[i], tmpc, &l);
			add_to_tree(tmpc);
			memset(tmpc, '\0', sizeof(tmpc));
			length += l;



			int body_idx = i + 1;
			int body_len = 0;
			int total = 0;

			char part[100][128] = { 0 };
			int part_idx = 0;


			while(asts[body_idx].refer == AST_WHILE_LOOP_ASSIGNMENT){
				char inner[256] = { 0 };
				code_emission(asts[body_idx], inner, &body_len);
				strcpy(part[part_idx++], inner);
				total += body_len;
				++body_idx;
			}

			if(total == 0){
				strcatf(tmpc, "NOP ; if body is empty");
				strcpy(part[part_idx++], tmpc);
				total = 1;
			}

			i = body_idx - 1;
		}

	}

	printf("LENGTH: %d\n", length);
}

