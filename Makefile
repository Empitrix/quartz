all:
	@ gcc ./main.c -o ./quartz

windows:
	@ x86_64-w64-mingw32-gcc ./main.c -o ./quartz

debug:
	@ gcc -g -fanalyzer  -Wall -Wextra -pedantic  -fsanitize=undefined,address,leak ./main.c -lm -o ./quartz
