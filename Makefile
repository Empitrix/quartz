# able to create executable file using 'gcc'
all:
	@ gcc ./main.c -o ./quartz

# create executable for windows using mingw
windows:
	@ x86_64-w64-mingw32-gcc ./main.c -o ./quartz

# create executable for windows/linux using zig's C compiler
zbuild:
	@ zig cc ./main.c -o ./quartz

cl:
	@ clang ./main.c -o ./quartz

# check for memory leak and warnings in source code
debug:
	@ gcc -g -fanalyzer -Wall -Wextra -pedantic -fsanitize=undefined,address,leak ./main.c -lm -o ./quartz

# run cppcheck
check:
	@ cppcheck . --check-level=exhaustive

# valgrind
val:
	@ valgrind ./quartz ./test.qz

# Full check (if test.qz exists)
full:
	@ echo "CPPCHECK..."
	@ cppcheck . --check-level=exhaustive
	@ echo "VALGRIND..."
	@ valgrind ./quartz ./test.qz
	@ echo "DEBUG..."
	@ gcc -g -fanalyzer -Wall -Wextra -pedantic -fsanitize=undefined,address,leak ./main.c -lm -o ./quartz

test:
	@ clear
	@ gcc ./main.c -o ./quartz
	@ ./quartz ./test.qz
	@ echo ""
	@ echo ""
	@ echo ""
	@ echo "------------->"
	@ cat ./out
	@ echo "<-------------"

assemble:
	@ clear
	@ gcc ./main.c -o ./quartz
	@ ./quartz ./test.qz
	@ ../assembler/assembler ./out -vo ../amethyst/examples/test.bin

run:
	@ clear
	@ gcc ./main.c -o ./quartz
	@ ./quartz ./test.qz
	@ ../assembler/assembler ./out -o ../amethyst/examples/test.bin
	@ ../amethyst/cpu -p ../amethyst/examples/test.bin -csf 10

asm:
	@ clear
	@ gcc ./main.c -o ./quartz
	@ ./quartz ./test.qz
	@ echo
	@ echo "------------->"
	@ ../assembler/assembler ./out -o ../amethyst/examples/test.bin -v
	@ echo "<-------------"

auto:
	@ clear
	@ gcc ./main.c -o ./quartz
	@ ./quartz ./test.qz
	@ ../assembler/assembler ./out -o ../amethyst/examples/test.bin
	@ ../amethyst/cpu -p ../amethyst/examples/test.bin -cf 10

imm:
	@ clear
	@ gcc ./main.c -o ./quartz
	@ ./quartz ./test.qz
	@ ../assembler/assembler ./out -o ../amethyst/examples/test.bin
	@ ../amethyst/cpu -p ../amethyst/examples/test.bin -cf 1000000

linker:
	@ cc ./assembler/main.c -o ./linker

