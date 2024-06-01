#!/bin/bash
cd build

bison ../$1.y || exit 1
clang -ly $1.tab.c -o $1.parser || exit 1
echo "0[0];"
echo "0[0];" | ./$1.parser