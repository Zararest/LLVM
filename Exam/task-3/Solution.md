# Examples to parse
In this task program should parse C definition of a named constant:

```C
#define Name 10
```

## Solution
Run:
```bash
mkdir build
cd build

bison ../constant.y -d 
flex  ../constant.l
clang -ly lex.yy.c constant.tab.c -o parser
```