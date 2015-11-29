/**
 * Introduction to Compiler Design by Prof. Yi Ping You
 * Project 1 Sample
 */

%{
#include <string.h>
#include "y.tab.h"


#define LIST strcat( buf, yytext )
#define token(t) { LIST; if (Opt_T) printf("<%s>\n", #t );}
#define tokenChar(t) { LIST; if (Opt_T) printf("<%c>\n", t );}
#define tokenInteger(t, i) { LIST; if (Opt_T) printf("<%s: %d>\n", t, i); }
#define tokenOctInteger(t, i) { LIST; if (Opt_T) printf("<%s: %s>\n", t, i); }
#define tokenFloat(t, i) { LIST; if (Opt_T) printf("<%s: %s>\n", t, i ); }
#define tokenScientific(t, s) { LIST; if ( Opt_T ) printf("<%s: %s>\n", t, s); }
#define tokenString(t, s) { LIST; if (Opt_T) printf("<%s: %s>\n", t, s); }


#define  MAX_LINE_LENG  256

int Opt_S = 1;				/* list source code option */
int Opt_T = 1;				/* list token option */
int linenum = 1;			/* line number */
char buf[MAX_LINE_LENG];		/* source code line buffer */
char strContent[MAX_LINE_LENG];		/* string buffer */

void setPseudoComment( const char * );	/* pseudo comment option switch */
%}

whitespace [ \t]+
letter [A-Za-z]
digit [0-9]
integer 0|[1-9][0-9]*
octal 0[0-7]+
fraction 0|[0-9]*
float {integer}"."{fraction}
scientific ({integer}|{float}|{octal})[Ee][+-]?({integer}|{octal})
identifier ({letter})({letter}|{digit})*

 /* exclusive start conditions */
%x STR
%x COMMENT
%x COMMENT_CPP



%%

","				{ tokenChar(','); return COMMA;}
";"				{ tokenChar(';'); return ';';}
":"				{ tokenChar(':'); return ':';}
")"				{ tokenChar(')'); return ')';}
"("				{ tokenChar('('); return '(';}
"["				{ tokenChar('['); return LBRACKET;}
"]"				{ tokenChar(']'); return RBRACKET;}

"+"				{ tokenChar('+'); return '+';}
"-"				{ tokenChar('-'); return '-';}
"*"				{ tokenChar('*'); return '*';}
"/"				{ tokenChar('/'); return '/';}
"mod"				{ token(mod); return MOD;}
":="				{ token(:=); return ASSIGN;}
"<"				{ tokenChar('<'); return '<'; }
"<="				{ token(<=); return LE; }
"<>"				{ token(<>); return NE;}
">="				{ token(>=); return GE;}
">"				{ tokenChar('>'); return '>';}
"="				{ tokenChar('='); return '=';}
"and"				{ token(and); return AND;}
"or"				{ token(or); return OR;}
"not"				{ token(not); return NOT;}

"array"				{ token(KWarray); return ARRAY;}
"begin"				{ token(KWbegin); return BG;}
"boolean"			{ token(KWboolean); return TYPE;}
"def"				{ token(KWdef);}
"do"				{ token(KWdo); return DO;}
"else"				{ token(KWelse); return ELSE;}
"end"				{ token(KWend); return END;}
"false"				{ token(KWfalse); return FALSE;}
"for"				{ token(KWfor); return FOR;}
"integer"			{ token(KWinteger); return TYPE;}
"if"				{ token(KWif); return IF;}
"of"				{ token(KWof); return OF;}
"print"				{ token(KWprint); return PRINT;}
"read"				{ token(KWread); return READ;}
"real"				{ token(KWreal); return TYPE;}
"string"			{ token(KWstring); return TYPE;}
"then"				{ token(KWthen); return THEN;}
"to"				{ token(KWto); return TO;}
"true"				{ token(KWtrue); return TRUE;}
"return"			{ token(KWreturn); return RETURN;}
"var"				{ token(KWvar); return VAR;}
"while"				{ token(KWwhile); return WHILE;}

{octal} { tokenOctInteger("oct_integer",yytext);return OCTAL;}
{identifier}			{ tokenString("id",yytext); return IDENT;}
{integer}			{ tokenInteger("integer",atoi(yytext)); return INT;}
{float}				{ tokenFloat("float",yytext); return FLOAT;}
{scientific}			{ tokenScientific("scientific", yytext); return SCI;}

"\""				{ /* begin string  */
				  LIST;
				  BEGIN STR;
				}

<STR>([^"\n])*			{
				  LIST;
				  strcat( strContent, yytext );
				}

<STR>"\""			{ /* end of string or "" */
				  char c;
				  c = input();
				  if( c == '\"' )	/* "" */
				  {
					LIST;
					strcat( buf, "\"" );
					strcat( strContent, "\"" );
				  }
				  else			/* end of string */
				  {
					unput(c);
					strcat(buf,"\"");
					if (Opt_T)
						printf("<%s: %s>\n", "string", strContent );
					strContent[0] = '\0';
				  	BEGIN INITIAL;
					return STRING;
				  }
				}
<STR>"\n"			{
				  /* error, cancel execution immediately!  */
				  fprintf( stdout, "error at line %d: string `%s` missing terminating \" character\n", linenum, strContent );
				  exit(-1);
				}

"//&"[A-Z][+-][^\n]*		{ /* pseudo comment */
				  LIST;
				  setPseudoComment( yytext );
				}
"//"				{ /* C++ stype comment */
				  LIST;
				  BEGIN COMMENT_CPP;
				}

<COMMENT_CPP>[^\n]+		{
				  LIST;
				}

<COMMENT_CPP>"\n"		{
				  LIST;
				  if( Opt_S )
				  	printf("%d: %s",  linenum,  buf);
				  linenum++;
				  buf[0] = '\0';
				  BEGIN INITIAL;
				}

"/*"				{ /* C-style comment */
				  LIST;
				  BEGIN COMMENT;
				}
<COMMENT>"*/"			{
				  LIST;
				  BEGIN INITIAL;
				}
<COMMENT>"\n"			{
				  LIST;
				  if  (Opt_S)
				  	printf("%d: %s",  linenum,  buf);
				  linenum++;
				  buf[0] = '\0';
				}
<COMMENT>.			{
				  LIST;
				}

{whitespace}			{
				  LIST;
				}

\n 				{
				  LIST;
				  if  (Opt_S)
				  	printf("%d: %s",  linenum,  buf);
				  linenum++;
				  buf[0] = '\0';
		   		}

.				{
				  /* error */
				  printf("error at line %d: bad character \"%s\"\n", linenum, yytext );
				  exit(-1);
				}

%%
/*
int  main( int argc, char **argv )
{
	if( argc != 2 ) {
		fprintf(  stdout,  "Usage:  ./scanner  [filename]\n"  );
		exit(0);
	}

	FILE *fp = fopen( argv[1], "r" );

	if( fp == NULL )  {
		fprintf( stdout, "Open  file  error\n" );
		exit(-1);
	}

	yyin = fp;
	yylex();
	exit(0);
}
*/

void setPseudoComment( const char *text )
{
	/* format : //&[A-Z][+-] */
	switch( text[3] ) {
	 case 'S':
		if( text[4] == '+' )
			Opt_S = 1;
		else if( text[4] == '-' )
			Opt_S = 0;
		break;
	 case 'T':
		if( text[4] == '+' )
			Opt_T = 1;
		else if( text[4] == '-' )
			Opt_T = 0;
	 	break;
	 default:
	 	break;
	}

}


