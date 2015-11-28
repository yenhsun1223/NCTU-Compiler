%{
#include <stdio.h>
#include <stdlib.h>

extern int linenum;             /* declared in lex.l */
extern FILE *yyin;              /* declared by lex */
extern char *yytext;            /* declared by lex */
extern char buf[256];           /* declared in lex.l */
%}

/*symbols*/
%token END COMMA VAR ARRAY OF TO BG ASSIGN PRINT READ LBRACKET RBRACKET
%token WHILE DO FOR RETURN
%token IDENT TYPE MOD LE GE NE NOT AND OR IF THEN ELSE INT FLOAT SCI OCTAL TRUE FALSE

%precedence PARENTHESES
%precedence NEG
%left '*' '/'
%left '+' '-'
%left '>' '<'
%precedence NOT
%precedence AND
%precedence OR


%%
program		: programname ';' programbody END IDENT TYPE ;
		 programname	: identifier
			;

programbody : const_decl_list
			| varibles
			| function_list
			| compound
		;

identifier	: IDENT
		   ;

number 		:INT
		 	|OCTAL
			|SCI
			|FLOAT
			;

literal_constant :number
				 ;

function_list : /*epsilon*/
			  |nonEmptyfunctions
			  ;

nonEmptyfunctions : function nonEmptyfunctions
				  |function
				  ;


function	: functionName '(' arguments ')' ':' TYPE ';' functionBody END IDENT /*function*/ RETURN
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
idList		: /*epsilon*/
		| nonEmptyidList
			;
nonEmptyidList :identifier COMMA nonEmptyidList
			   | identifier
			   ;

const_decl_list : nonEmptyConst_decl
				;

nonEmptyConst_decl :nonEmptyConst_decl const_decl
				   |const_decl
				   ;

const_decl:VAR idList ':' literal_constant ';'
		  ;
varible		: VAR idList ':' TYPE ';'
		 | VAR idList ':' ARRAY int_constant TO int_constant OF TYPE ';'
			;

int_constant :INT
			 ;

varibles	: /*epsilon*/
		 | nonEmptyvaribles
		  ;
nonEmptyvaribles:nonEmptyvaribles varible
				|varible
				;


statements: nonEmptystatements
		  |/*epsilon*/
		  ;

nonEmptystatements : nonEmptystatements statement
				   | statement
				   ;

/*TODO undone!!*/
statement :	compound
		  |	simple
		  |	expressions
		  | conditional
		  |	while
		  | for
		  | return
		  | function_invocation
		   ;


compound	: BG varibles statements END
		 ;

simple		:varible_reference ASSIGN expression ';'
			| PRINT varible_reference ';'
			| PRINT expression ';'
			| READ varible_reference
			;

array_reference		:identifier array_reference_list
				 ;

array_reference_list : /*epsilon*/ /*so only idenfifier in array_reference*/
					 |LBRACKET integer_expression RBRACKET array_reference_list
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
		   | expression MOD expression %prec '*'	{$$ = $1 % $2;	}
		   | expression AND expression %prec AND	{$$ = $1 && $2;	}
		   | expression OR  expression %prec OR		{$$ = $1 || $2;	}
		   | NOT expression %prec NOT				{$$ = !$2;	}
		   | '(' expression ')' %prec PARENTHESES
		   | number									{$$ = $1;		}
		   | IDENT
		   | function_invocation
		   ;

/* TODO*/
integer_expression :
				   ;

boolean_expr :  boolean_expr '>' boolean_expr 				{$$ = $1 > $2;	}
			 | boolean_expr '<' boolean_expr 				{$$ = $1 < $2;	}
			| boolean_expr LE boolean_expr %prec '>'	{$$ = $1 <= $2;	}
			| boolean_expr GE boolean_expr %prec '>'	{$$ = $1 >= $2;	}
			| boolean_expr '=' boolean_expr 				{$$ = $1 == $2;	}
			| boolean_expr NE boolean_expr %prec '>'	{$$ = $1 != $2;	}
			| number 									{$$ = $1;		}
			| IDENT
			;

function_invocation : identifier '(' expression_list ')'
					;

expression_list 	:expression
				 |expression COMMA expression_list
					;

conditional			:IF boolean_expr THEN conditional_body END IF
			  ;

conditional_body 	:statements
				  |statements ELSE statements

while				:WHILE boolean_expr DO statements END DO
		 ;

for					:FOR identifier ASSIGN int_constant TO int_constant DO statements END DO
		;

return 				: RETURN expressions
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

