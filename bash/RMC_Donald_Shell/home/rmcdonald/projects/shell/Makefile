
LEX = flex
YACC = bison -d
CC = cc


shell: shell.tab.o lex.yy.o
	$(CC) -o shell shell.tab.o lex.yy.o -ll -lm 


lex.yy.o: lex.yy.c shell.tab.h


shell.tab.c shell.tab.h: shell.y
	$(YACC) -v shell.y


lex.yy.c: shell.l
	$(LEX) shell.l


clean:
	-rm -f *.o lex.yy.c *.tab.*  shell *.output

