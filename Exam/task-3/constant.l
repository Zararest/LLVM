%{
  #include "constant.tab.h"
%}

WS      [ \t\v]+
DIGIT   [0-9]
DIGIT1  [1-9]
NUM ({DIGIT1}{DIGIT}*)|"0"

LETTER  [a-z]|[A-Z]
UNDERLI "_"
NAME     ({LETTER}|{UNDERLI})({UNDERLI}|{NUM}|{LETTER})*

%%

{WS}
"#define"   { return DEFINE; }
{NAME}      { return NAME; }
{NUM}       { return NUM; }
.           { return -1; }

%%