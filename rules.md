# Quartz Script
C-like syntax


### Types
- `int`
- `char`
- `string` a sequence of `char` (all of the strings are `const`)


### Variable Assignment
```qz
int a = 5;
char b = 'A';
char c[] = "Hello";
```


### Comments
Use `//`:
```qz
// This is a comment
int a = 7; // This is a comment
```


### Function Definition
```qz
<return type> name(...args){
	// Function Body
}
```

in Quartz:
```qz
void main(){
	// Program starts executing from here
}
```


### Increment / Decrement
Increment:
```qz
variable++;
```

Decrement:
```qz
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


to define a global constant use `#define`
```qz
#define MAX 1000
```

