TARGET = parser
OBJECT = lex.yy.c y.tab.h y.tab.c
CC = gcc -g
LEX = flex
LIBS = -lfl -ly
YACC = yacc -d -v

all: lex.yy.c y.tab.c
	$(CC) lex.yy.c y.tab.c -o $(TARGET) $(LIBS)

y.tab.c: parser.y
	$(YACC) parser.y

lex.yy.c: lex.l
	$(LEX) lex.l

clean:
	rm -f $(TARGET) $(OBJECT)

