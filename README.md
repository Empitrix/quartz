<div align="center" style="margin: 10px">
	<picture>
		<img alt="Quartz" src="./assets/quartz.png" width="45%">
	</picture>
	<h1>Quartz</h1>
	<!-- <p>A compiler for <a href="https://github.com/empitrix/8bitcpu">Amethyst</a></p> -->
	<p>Quartz is a cross-platform compiler for the PIC10F200 microcontroller that utilizes <a href="https://github.com/empitrix/assembler">assembler</a> as a linker to generate assembly code tailored for this type of microcontroller.</p>
</div>

## Build
First clone the project:
```bash
git clone https://github.com/empitrix/quartz
cd ./quartz
```

To build the project using `gnu make`:
```bash
make
```


## Quartz Script
The syntax of this language is similar to C but with fewer features.

### Types
There is only 3 type
- `int`
- `char`
- `char []` a sequence of `char (string)` (const)

> Because this is an 8-bit CPU compiler, both `int` and `char` are 8 bits in size, so they behave the same.


### Variable Assignment
All variables must be defined at the very top of the file.

```qz
int a = 5;
char b = 'A';
char c[] = "Hello";
```


### Comments
To write a comment, use `//`. Anything after `//` is part of the comment unless it's inside a string.
```qz
// This is a comment
int a = 7; // This is also a comment
```


### Raw Assembly
To write direct assembly code in a .qz file, use the backtick (\`) symbol.
To use a numeric variable or value, enclose it in `{ ... }`. Everything between `{}` will be replaced by a number.
It's important to note that if { variable } is used for a variable, the address of the variable in RAM will be set. Otherwise, if a numeric value is used, the value itself will be replaced.

For example:
```qz
char a = 'A';      // Variable a is initialized with the value 'A'
`MOVF { a }, W`;   // Move the value of 'a' into the W register (address of 'a' will be used)
`MOVLW { 144 }`;   // Move the literal value 144 into the W register (value itself will be used)
```

### Functions

Every function must have a numerical return type (`int` or `char`, due to the `RETLW` opcode), but the arguments can be of any of the three types.

```qz
(int | char) name(...args){
	// Function Body
}
```

For example:
```qz
int putchar(char l){
	`MOVLW { l }`;
	`MOVWF 0x06`;
	`BSF 0x06, 7`;
	`CLRF 0x06`;
	return 0;
}

int main(){
	putchar('H');
	putchar('I');
	putchar('!');
	return 0;
}
```


### Increment / Decrement

To use increment or decrement, place `++` or `--` right after the variable name (without any whitespace).

```qz
// Increment
variable++;

// Decrement
variable++;
```


### Loops

Loops are similar to C syntax, except that you can't define a new variable in the initialization part of the `for` loop.

`for` loop:
```qz
int i = 0;
for(i = 'A'; i < 'K'; i++){
	// Do something
}
```

`while` loop:
```qz
int a = 0;

while (a > 10){
	// Do something
	a++;
}
```


### Conditions

`if` and `else` are the same as in other languages.

```qz
if (a > b){
	// do something
} else {
	// do something else
}
```

### Macro

Quartz does not have macros like C. However, if you use a macro (define), it directly translates into the following assembly code:

For example, the following code in Quartz (`#define` - a name - numeric value):
```qz
#define GPIO 0x06
```

Acts like the following code in assembly:
```asm
GPIO EQU 0x06
```

You can also use macros in raw assembly. For example:
```qz
#define GPIO 0x06
`MOVWF { GPIO }`;  // Move value of register W to register GPIO (0x06)
```

### Numbers:

The type of the numbers is `uint8_t`, meaning the value range is from `0x00` to `0xFF`.

- `int`: `0` to `255`
- `hex`: `0x00` to `0xFF`
- `binary`: `00000000B` to `11111111B`


### Escape Sequences
- `\n`: new line character.
- `\t`: a two-space tab (due to the [console](https://github.com/Empitrix/8bitcpu?tab=readme-ov-file#console)).
- `\\`: backslash `\`.


### Operators

There are only two compact operators:
```qz
a += 5;  // add 5 to a
a -= 5;  // subract 5 from a
```

Conditonal operaotrs:
- Greater (`>`)
- Smaller (`<`)
- Greater or Eqaul (`>=`)
- Smaller or Eqaul (`<=`)

```qz
if(a > b){ ... }
if(a < b){ ... }
if(a >= b){ ... }
if(a <= b){ ... }
```

Complement:
```
a = ~a;   // complement of 'a' (store in 'a')
```

OR (`|`):
```
a = a | b;  // OR between 'a' and 'b' (store in 'a')
```

AND (`&`):
```
a = a & b;  // AND between 'a' and 'b' (store in 'a')
```

XOR (`^`):
```
a = a ^ b;  // XOR between 'a' and 'b' (store in 'a')
```

Add (`+`):
```
a = a + 5;  // Add 5 to 'a' (store in 'a')
```

Minus (`-`):
```
a = a - 5;  // subract 5 from a (store in 'a')
```


Right Shift (`>>`):
```
a = a >> 1;  // Move value of 'a' to right by 1 (store in 'a')
```


Left Shift (`<<`):
```
a = a << 1;  // Move value of 'a' to left by 1 (store in 'a')
```


## Examples

Look at the following example:

```c
#define GPIO 0x06

int i = 0;
int j = 0;

int putchar(int l){
	`MOVF { l }, W`;
	`MOVWF { GPIO }`;
	`BSF { GPIO }, 7`;
	`CLRF { GPIO }`;
	return 0;
}

int main(){
	for(i = 0; i <= 5; i++){
		for(j = 0; j <= i; j++){
			putchar('*');
		}
		putchar('\n');
	}

	putchar('\n');

	// print letters
	i = 'A'
	while(i < 'K'){
		putchar(i);
		i++;
	}

	return 0;
}

```

the output is looks like this:

```text
*
**
***
****
*****
******

ABCDEFGHIJ
```


---


Another example to see the conditions for loops:

```c
#define GPIO 0x06

int i = 0;
int j = 0;

int putchar(int l){
	`MOVF { l }, W`;
	`MOVWF { GPIO }`;
	`BSF { GPIO }, 7`;
	`CLRF { GPIO }`;
	return 0;
}

int main(){

	// ABCDEFGHIJ
	i = 'A'
	while(i < 'K'){
		putchar(i);
		i++;
	}
	putchar('\n');

	// ABCDEFGHIJK
	i = 'A'
	while(i <= 'K'){
		putchar(i);
		i++;
	}
	putchar('\n');

	// KJIHGFEDCBA
	i = 'K'
	while(i <= 'A'){
		putchar(i);
		i--;
	}
	putchar('\n');

	// KJIHGFEDCB
	i = 'K'
	while(i < 'A'){
		putchar(i);
		i--;
	}


	return 0;
}
```

And the expected output is something like this:

```text
ABCDEFGHIJ
ABCDEFGHIJK
KJIHGFEDCBA
KJIHGFEDCB
```

