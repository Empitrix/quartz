
// int extract_to(char[], ast_t, int *);

// void generator(void){
// 	int i;
// 	int if_detected = 0;
// 
// 	for(i = 0; i < ast_len(); ++i){
// 		char tmpc[256] = { 0 };  // Temp Code
// 
// 		if(if_detected != 1 && asts[i].type != AST_ELSE_STATEMENT){ if_detected = 0; }
// 
// 		if(asts[i].type == AST_VARIABLE_ASSIGNMENT ||
// 			asts[i].type == AST_RAW_ASM ||
// 			asts[i].type == AST_FUNCTION_CALL ||
// 			asts[i].type == AST_MACRO ||
// 			asts[i].type == AST_FUNCTION_ASSIGNMENT) {
// 				code_emission(asts[i], tmpc, NULL);
// 				add_to_tree(tmpc);
// 
// 		} else if(asts[i].type == AST_RETURN_STATEMENT){
// 				code_emission(asts[i], tmpc, NULL);
// 				add_to_tree(tmpc);
// 
// 		} else if (asts[i].type == AST_IF_STATEMENT){
// 			const char *btm_label = get_label();
// 			const char *eq_label = get_label();
// 			const char *neq_label = get_label();
// 
// 			char if_sec[1024] = { 0 };
// 			char else_block[1024] = { 0 };
// 			code_emission(asts[i], if_sec, NULL);
// 			int if_len = extract_to(tmpc, AST_IF_STATEMENT, &i);
// 			int else_len = 0;
// 
// 			// Else detected
// 			if(asts[i + 1].type == AST_ELSE_STATEMENT){
// 				i++;
// 				else_len = extract_to(else_block, AST_ELSE_STATEMENT, &i);
// 			}
// 
// 			add_to_tree(if_sec);
// 			attf("\tGOTO %s", eq_label);
// 
// 
// 			if(else_len != 0){
// 				attf("\tGOTO %s", neq_label);
// 				attf("%s:", neq_label);
// 				add_to_tree(else_block);
// 			}
// 
// 			attf("\tGOTO %s", btm_label);
// 			attf("%s:", eq_label);
// 
// 			if(if_len == 0){
// 				attf("\tNOP  ; No 'IF' body");
// 			} else {
// 				add_to_tree(tmpc);
// 			}
// 
// 			attf("%s:", btm_label);
// 
// 
// 		} else if(asts[i].type == AST_WHILE_LOOP_ASSIGNMENT){
// 			const char *top_label = get_label();
// 			const char *bottom_label = get_label();
// 
// 			attf("%s:", top_label);
// 
// 			char while_sec[1024] = { 0 };
// 			code_emission(asts[i], while_sec, NULL);
// 			int len = extract_to(tmpc, AST_WHILE_LOOP_ASSIGNMENT, &i);
// 
// 			add_to_tree(while_sec);
// 
// 			if(len == 0){
// 				add_to_tree("NOP ; NO WHILE BODY");
// 			} else {
// 				attf("\tGOTO %s", bottom_label);
// 			}
// 
// 			update_tree_lines(tmpc);
// 			attf("\tGOTO %s", top_label);
// 			attf("%s:", bottom_label);
// 
// 
// 
// 		} else if(asts[i].type == AST_FOR_LOOP_ASSIGNMENT){
// 			int si = i;   // save current state of i 
// 			const char *bottom_label = get_label();
// 			char top_label[20];
// 
// 			char for_sec[1024] = { 0 };
// 			code_emission(asts[i], for_sec, top_label);
// 			int len = extract_to(tmpc, AST_FOR_LOOP_ASSIGNMENT, &i);
// 
// 			add_to_tree(for_sec);
// 
// 			if(len == 0){
// 				add_to_tree("NOP ; NO FOR BODY");
// 			} else {
// 				attf("\tGOTO %s", bottom_label);
// 			}
// 
// 			update_tree_lines(tmpc);
// 
// 			if(asts[si].for_asgmt.iter.op == INCREMENT_OP){
// 				attf("\tINCF %s, 1", asts[si].for_asgmt.iter.left);
// 			} else {
// 				attf("\tDECF %s, 1", asts[si].for_asgmt.iter.left);
// 			}
// 
// 			attf("\tGOTO %s", top_label);
// 			attf("%s:", bottom_label);
// 
// 
// 
// 
// 		} else if(asts[i].type == AST_STATEMENT){
// 			code_emission(asts[i], tmpc, NULL);
// 			add_to_tree(tmpc);
// 		}
// 
// 	}
// }




// int extract_to(char dst[], ast_t type, int *index){
// 	int total_len = 0;
// 	int i = *index;
// 	i++;  // point to next instruction
// 
// 	char part[100][128] = { 0 };
// 	int part_idx = 0;
// 
// 	while(asts[i].refer == type){
// 		char inner[256] = { 0 };
// 		code_emission(asts[i], inner, NULL);
// 		strcpy(part[part_idx++], inner);
// 		if(strcmp(inner, "") != 0){ total_len++; }
// 		++i;
// 	}
// 
// 	for(int j = 0; j < part_idx; ++j){
// 		strcat(dst, part[j]);
// 		if(j != part_idx - 1){ strcat(dst, "\n"); }
// 	}
// 
// 	*index = i - 1;  // go back by 1
// 
// 	return total_len;
// }
