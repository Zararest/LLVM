%{
  #include "array.tab.h"
  #include <stdio.h>
%}

WS      [ \t\v]+
DIGIT   [0-9]
DIGIT1  [1-9]
NUMEXPR ({DIGIT1}{DIGIT}*)|"0"

LETTER  [a-z]|[A-Z]
UNDERLI "_"
VAREXPR     ({LETTER}|{UNDERLI})({UNDERLI}|{NUMEXPR}|{LETTER})*

%%

{WS}
";"         { printf(";"); return EOL; }
{VAREXPR}   { printf("var"); return NAME; }
{NUMEXPR}   { printf("num"); return NUMTOKEN; }
"["         { printf("["); return LBTOKEN; }
"]"         { printf("]"); return RBTOKEN; }
.           { printf("end"); return -1; }  

%%