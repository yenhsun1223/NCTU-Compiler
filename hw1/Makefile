scanner.out:lex.yy.c
	gcc lex.yy.c -lfl -o scanner.out
lex.yy.c:lextemplate.l
	flex lextemplate.l
clean:
	rm -f lex.yy.c

