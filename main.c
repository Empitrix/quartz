#include "src/utility.h"
#include "src/tokenizer.h"
#include "src/parser.h"


int main(int argc, char *argv[]){
	GFLAG gflag;
	update_glfag(&gflag, argc, argv);

	char buff[MAXFILE];
	read_file(gflag.input, buff);

	TKNS tkns;
	tokenizer(buff, &tkns);

	parser(&tkns);

	return 0;
}

