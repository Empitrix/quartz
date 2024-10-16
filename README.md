<div align="center" style="margin: 10px">
	<picture>
		<img alt="Quartz" src="./assets/quartz.png" width="60%">
	</picture>
	<h1>Quartz</h1>
	<p>A compiler for <a href="https://github.com/empitrix/8bitcpu">Amethyst</a></p>
</div>

<!-- [langauge syntax](./syntax.md) -->

> [!NOTE]
> This repository will be updated!

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
The syntax of this language is similar to C, but with fewer features.

### Types
There is only 3 type

- `int`
- `char`
- `char []` a sequence of `char (string)` (const)


### Variable Assignment
All variables must be defined at the very top of the file or function.

```qz
int a = 5;
char b = 'A';
char c[] = "Hello";
```


### Comments
To write a comment, use //. Anything after // is part of the comment, unless it's inside a string.
```qz
// This is a comment
int a = 7; // This is a comment
```


### Function Definition

Every function must have a numerical return type (`int` or `char` for `RETLW`), but the arguments can be of any of the three types.

```qz
<return type> name(...args){
	// Function Body
}
```

In Quartz:
```qz
void main(){
	// Program starts executing from here
}

int function(int input){
	return 0;
}
```


### Increment / Decrement
To use increment or decrement, place `++` or `--` immediately after the variable name (without any whitespace).
```qz
// Increment
variable++;

// Decrement
variable++;
```

### Loops

`for` loop:
```qz
for(int i = 0; i < 10; ++i){
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
`if` & `else`:
```qz
if (a > b){
	// do something
} else {
	// do something else
}
```



### Macros?
to include a file use `#include "file-address"`:
```qz
#include "source.qz"
```

To define a global constant, use `#define`
```qz
#define NEWLINE '\n'
```


### Numbers:
The type of the numbers is `uint8_t`, meaning the value range is from `0x00` to `0xFF`.

- `int`: `0` to `255`
- `hex`: `0x00` to `0xFF`
- `binary`: `00000000B` to `11111111B`


### Allowed Escape Sequences
- `\n`: new line character.
- `\t`: a two-space tab (due to the [console](https://github.com/Empitrix/8bitcpu?tab=readme-ov-file#console)).
- `\\`: letter `\`.



## Todo
- [ ] **PREPROCESSOR**
- [ ] `side_t` supports `CNST_VAR`
- [ ] `CNST_VAR` accepts `name` now (make sure every function supports)
- [ ] auto `POP_RAM` for varialbe assignments
