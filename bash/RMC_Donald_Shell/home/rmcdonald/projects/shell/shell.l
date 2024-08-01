%{
	#include <stdio.h>
	#include "shell.tab.h"
%}

%option noyywrap

%%

[0-9]+	{ yylval.ival = atoi(yytext); return INTEGER; }

[a-zA-Z0-9]+	{ return COMMAND; }

[0-9]+\.[0-9]*	{ yylval.fval = atof(yytext); return FLOAT; }

"="		{ return EQUAL; }

"\n"	{ return NEWLINE; }

","		{ return COMMA; }

.		{ printf("Invalid character\n"); }
%%
