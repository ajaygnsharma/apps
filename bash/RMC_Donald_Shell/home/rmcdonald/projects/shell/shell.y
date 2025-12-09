%{
  #include <stdio.h>
  int yylex (void);
  void yyerror (char const *);
%}

%union {
	int ival;
	float fval;
}

%token COMMAND EQUAL NEWLINE COMMA
%token <ival> INTEGER
%token <fval> FLOAT

%%
commands : %empty
			| commands command
			;

command : COMMAND NEWLINE { printf("C4A=1\n"); } ;
command : COMMAND EQUAL INTEGER NEWLINE { printf("C4A %d\n", $3); } ;
command : COMMAND EQUAL INTEGER COMMA INTEGER NEWLINE { printf("C4A %d %d\n", $3, $5); } ;
command : COMMAND EQUAL FLOAT NEWLINE { printf("C4A %f\n", $3); } ;
command : command error NEWLINE { yyerrok; } ;

%%

int
main (void)
{
  // yydebug = 1;
  return yyparse ();
}

#include <stdio.h>


/* Called by yyparse on error.  */
void
yyerror (char const *s)
{
  fprintf (stderr, "%s\n", s);
}
