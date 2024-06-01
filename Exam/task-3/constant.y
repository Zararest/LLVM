%{
  #include <stdio.h>
  int yyparse();
  int yylex();
  void yyerror(char *s) {
    fprintf (stderr, "%s\n", s);
  }
  int yywrap(void) { return 1; }
%}

%token NAME
%token NUM
%token DEFINE

%%
PROG:   DECL
      | PROG DECL
      ;

DECL: DEFINE NAME NUM { printf("definition\n"); }
%%