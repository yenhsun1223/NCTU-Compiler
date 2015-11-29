%{
#include <stdio.h>
#include <stdlib.h>

extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */
%}

/*symbols*/
%token END VAR ARRAY OF TO BG ASSIGN PRINT READ
%token WHILE DO FOR RETURN
%token IDENT TYPE MOD LE GE NE NOT AND OR IF THEN ELSE INT FLOAT SCI OCTAL TRUE FALSE STRING

%precedence PARENTHESES
%precedence NEG
%left '*' '/'
%left '+' '-'
%left '>' '<'
%precedence NOT
%precedence AND
%precedence OR


%%
program			: programname ';' programbody END programname
				;

programname		: identifier
				;

identifier		: IDENT
				;

programbody		:varible_list function_list compound
				;




idList				: /*epsilon*/
					|nonEmptyidList
					;

nonEmptyidList		:nonEmptyidList ',' identifier
					|identifier
					;

literal_constant	:number
				 	|STRING
					|const_boolean
					;

const_boolean		:TRUE
			   		|FALSE
					;

number		:INT
		 	|OCTAL
			|SCI
			|FLOAT
			;

varible_list	: /*epsilon*/
				|nonEmptyvaribles
				;

nonEmptyvaribles:nonEmptyvaribles varible
				|varible
				;

function_list : /*epsilon*/
			  |nonEmptyfunctions
			  ;

nonEmptyfunctions : function nonEmptyfunctions
				  |function
				  ;


function	: functionName '(' arguments ')' ':' TYPE ';' functionBody END IDENT /*function*/
		 | functionName '(' arguments ')' ';' functionBody END IDENT /*procedure*/
			;

functionBody: compound
			;

functionName :identifier
			 ;


arguments	: /*epsilon*/
		  | nonEmptyDeclar
			;
nonEmptyDeclar : idList ':' TYPE ';' nonEmptyDeclar
			   | idList ':' TYPE
			   ;

varible			:VAR idList ':' TYPE ';'
				|VAR idList ':' ARRAY int_constant TO int_constant OF array_types';'
				|VAR idList ':' literal_constant ';'
				;

array_types		:ARRAY int_constant TO int_constant OF array_types
			 	| TYPE
				;

int_constant	:INT
				;

statements: nonEmptystatements
		  |/*epsilon*/
		  ;

nonEmptystatements : nonEmptystatements statement
				   | statement
				   ;

statement :	compound
		  |	simple
		  |	expressions
		  | conditional
		  |	while
		  | for
		  | return
		  | function_invocation ';'
		   ;


compound	: BG varible_list statements END
		 ;

simple		:varible_reference ASSIGN expressions ';'
			| PRINT varible_reference ';'
			| PRINT expressions ';'
			| READ varible_reference ';'
			;

array_reference		:identifier array_reference_list
				 ;

array_reference_list : /*epsilon*/ /*so only idenfifier in array_reference*/
					 |'[' integer_expression ']' array_reference_list
					 ;

varible_reference : array_reference
				  ;

expressions :expression
			|integer_expression
			|boolean_expr
			;

expression	: '-' expression %prec NEG				{$$ = -$2;		}
		   | expression '+' expression 				{$$ = $1 + $2;	}
		   | expression '-' expression 				{$$ = $1 - $2;	}
		   | expression '*' expression 				{$$ = $1 * $2;	}
		   | expression '/' expression 				{$$ = $1 / $2;	}
		   | expression MOD expression %prec '*'
		   | '(' expression ')' %prec PARENTHESES
		   | number									{$$ = $1;		}
		   | identifier
		   | function_invocation
		   | STRING
		   ;

integer_expression : int_constant
				   | identifier
				   ;

boolean_expr:expression '>' expression 				{$$ = $1 > $2;	}
			|expression '<' expression 				{$$ = $1 < $2;	}
			|expression LE expression %prec '>'		{$$ = $1 <= $2;	}
			|expression GE expression %prec '>'		{$$ = $1 >= $2;	}
			|expression '=' expression 				{$$ = $1 == $2;	}
			|expression NE expression %prec '>'		{$$ = $1 != $2;	}
			|expression AND expression %prec AND	{$$ = $1 && $2;	}
			|expression OR  expression %prec OR		{$$ = $1 || $2;	}
			|NOT expression %prec NOT				{$$ = !$2;	}
			|const_boolean 							{$$ = $1;		}
			|identifier
			|function_invocation
			;

function_invocation : identifier'('expression_list')'
					;

expression_list 	:/*epsilon*/
				 	|expression
					|expression_list ',' expression
					;

conditional			:IF boolean_expr THEN conditional_body END IF
			  ;

conditional_body 	:statements
				  |statements ELSE statements

while				:WHILE boolean_expr DO statements END DO
		 ;

for					:FOR identifier ASSIGN int_constant TO int_constant DO statements END DO
		;

return 				: RETURN expressions ';'
		   ;


%%

int yyerror( char *msg )
{
		fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );
	fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
	fprintf( stderr, "|\n" );
	fprintf( stderr, "| Unmatched token: %s\n", yytext );
		fprintf( stderr, "|--------------------------------------------------------------------------\n" );
		exit(-1);
}

int  main( int argc, char **argv )
{
	yydebug=1;
	if( argc != 2 ) {
		fprintf(  stdout,  "Usage:  ./parser  [filename]\n"  );
		exit(0);
	}

FILE *fp = fopen( argv[1], "r" );

if( fp == NULL )  {
		fprintf( stdout, "Open  file  error\n" );
		exit(-1);
	}

yyin = fp;
	yyparse();

fprintf( stdout, "\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	fprintf( stdout, "|  There is no syntactic error!  |\n" );
	fprintf( stdout, "|--------------------------------|\n" );
	exit(0);
}

