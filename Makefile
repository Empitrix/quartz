# able to create executable file using 'gcc'
all:
	@ gcc ./main.c -o ./quartz

# create executable for windows using mingw
windows:
	@ x86_64-w64-mingw32-gcc ./main.c -o ./quartz

# create executable for windows/linux using zig's C compiler
zbuild:
	@ zig cc ./main.c -o ./quartz

# check for memory leak and warnings in source code
debug:
	@ gcc -g -fanalyzer -Wall -Wextra -pedantic -fsanitize=undefined,address,leak ./main.c -lm -o ./quartz

# run cppcheck
check:
	@ cppcheck . --check-level=exhaustive

