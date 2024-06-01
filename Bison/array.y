%{
  #include <stdio.h>
  int yyparse();
  int yylex();
  void yyerror(char *s) {
    fprintf (stderr, "%s\n", s);
  }
  int yywrap(void) { return 1; }
%}

%token NUMTOKEN
%token NAME
%token RBTOKEN
%token LBTOKEN
%token EOL

/*
Target expression:
int Array[1];
int Array[D];
*/

%%

EVALUATE: EXPR EOL { printf("%d\n", $$);  YYACCEPT; };  

EXPR:   TYPENAME VARNAME LBTOKEN VARNAME  RBTOKEN 
      | TYPENAME VARNAME LBTOKEN NUMTOKEN RBTOKEN
      ;

TYPENAME: VARNAME;

VARNAME: NAME;

%%