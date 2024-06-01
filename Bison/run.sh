#!/bin/bash
cd build

bison ../$1.y -d || exit 1
flex ../$1.l || exit 1
clang -ly lex.yy.c $1.tab.c -o $1.parser || exit 1
echo "int a[a];"
echo "int a[a];" | ./$1.parser
