%{
/**
 * Introduction to Compiler Design by Prof. Yi Ping You
 * Project 2 YACC sample
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"

extern int linenum;		/* declared in lex.l */

extern FILE *yyin;		/* declared by lex */
extern char *yytext;		/* declared by lex */
extern char buf[256];		/* declared in lex.l */
extern int yylex(void);
int yyerror(char* );


SymbolTable* symbol_table;
TableEntry* entry_buf;
IdList* idlist_buf;
Type* return_buf;

%}
/* types */
%union	{
	int num;
	double dnum;
	char* str;
	int nodetype;
	Value* value;
	Type* type;
	TableEntry* tableentry;
	TypeList* typelist;
	Expr* expression;
	ExprList* exprlist;
		}
/* tokens */
%token <str> ARRAY
%token <str> BEG
%token <str> BOOLEAN
%token <str> DEF
%token <str> DO
%token <str> ELSE
%token <str> END
%token 		 FALSE
%token <str> FOR
%token <str> INTEGER
%token <str> IF
%token <str> OF
%token <str> PRINT
%token <str> READ
%token <str> REAL
%token <str> RETURN
%token <str> STRING
%token <str> THEN
%token <str> TO
%token <str> TRUE
%token <str> VAR
%token <str> WHILE

%token <str> ID
%token OCTAL_CONST
%token <num> INT_CONST
%token <dnum>FLOAT_CONST
%token <str> SCIENTIFIC
%token <str> STR_CONST

%token <str> OP_ADD
%token <str> OP_SUB
%token <str> OP_MUL
%token <str> OP_DIV
%token <str> OP_MOD
%token <str> OP_ASSIGN
%token <str> OP_EQ
%token <str> OP_NE
%token <str> OP_GT
%token <str> OP_LT
%token <str> OP_GE
%token <str> OP_LE
%token <str> OP_AND
%token <str> OP_OR
%token <str> OP_NOT

%token <str> MK_COMMA
%token <str> MK_COLON
%token <str> MK_SEMICOLON
%token <str> MK_LPAREN
%token <str> MK_RPAREN
%token <str> MK_LB
%token <str> MK_RB
/* non-terminal */
%type <str> rel_op mul_op
%type <type> scalar_type type opt_type array_type
%type <typelist> param_list opt_param_list param
%type <value> literal_const int_const
%type <tableentry> func_decl
%type <expression> var_ref factor boolean_expr term boolean_term boolean_factor relop_expr expr return_stmt
%type <exprlist> boolean_expr_list opt_boolean_expr_list


/* start symbol */
%start program
%%

program			: ID MK_SEMICOLON
				program_body
				END ID
				{
					TableEntry* tmp=BuildTableEntry($1,"program",symbol_table->current_level,BuildType("void"),NULL);
					InsertTableEntry(symbol_table,tmp);
					PrintSymbolTable(symbol_table);
				}
			;

program_body		: opt_decl_list opt_func_decl_list compound_stmt
			;

opt_decl_list		: decl_list
			| /* epsilon */
			;

decl_list		: decl_list decl
			| decl
			;

decl			: VAR id_list MK_COLON scalar_type MK_SEMICOLON  /* scalar type declaration */
			{
				InsertTableEntryFromList(symbol_table,idlist_buf,"varible",$4,NULL);
				ResetIdList(idlist_buf);
			}

			| VAR id_list MK_COLON array_type MK_SEMICOLON       /* array type declaration */
			{
				InsertTableEntryFromList(symbol_table,idlist_buf,"varible",$4,NULL);
				ResetIdList(idlist_buf);
			}
			| VAR id_list MK_COLON literal_const MK_SEMICOLON     /* const declaration */
			{
				Attribute* tmp_attri=BuildConstAttribute($4);
				InsertTableEntryFromList(symbol_table,idlist_buf,"constant",$4->type,tmp_attri);
				ResetIdList(idlist_buf);
			}

			;
int_const	:	INT_CONST		{$$=BuildValue("integer",yytext);}
			|	OCTAL_CONST 	{$$=BuildValue("octal",yytext);}
			;
/*FIXME*/
literal_const		: int_const {$$=$1;}
			| OP_SUB int_const  {$$=$2;}
			| FLOAT_CONST 		{$$=BuildValue("real",yytext);}
			| OP_SUB FLOAT_CONST {$$=BuildValue("real",yytext);}
			| SCIENTIFIC		{$$=BuildValue("scientific",yytext);}
			| OP_SUB SCIENTIFIC {$$=BuildValue("scientific",yytext);}
			| STR_CONST 		{$$=BuildValue("string",yytext);}
			| TRUE 				{$$=BuildValue("boolean",yytext);}
			| FALSE				{$$=BuildValue("boolean",yytext);}
			;

opt_func_decl_list	: func_decl_list
			| /* epsilon */
			;

func_decl_list		: func_decl_list func_decl {InsertTableEntry(symbol_table,$2);}
					| func_decl					{InsertTableEntry(symbol_table,$1);}

			;

func_decl		: ID
				MK_LPAREN { symbol_table->current_level++;}
				opt_param_list
				MK_RPAREN { symbol_table->current_level--; }
				opt_type  {return_buf=$7;}
				MK_SEMICOLON
				compound_stmt
				END ID
				{
					Attribute* func_attr=BuildFuncAttribute($4);
					$$=BuildTableEntry($1,"function",symbol_table->current_level,$7,func_attr);

				}
			;

opt_param_list		: param_list
			| 						{$$=NULL;}
			;

param_list		: param_list MK_SEMICOLON param {$$=ExtendTypelist($1,$3);}
			| param 							{$$=$1;}
			;

param			: id_list MK_COLON type
				{
					$$=AddTypeToList(NULL,$3,idlist_buf->pos);
					InsertTableEntryFromList(symbol_table,idlist_buf,"parameter",$3,NULL);
					ResetIdList(idlist_buf);
				}
			;

id_list			: id_list MK_COMMA ID 	{InsertIdList(idlist_buf,yytext);}
				| ID 					{InsertIdList(idlist_buf,yytext);}
			;

opt_type		: MK_COLON type {$$=$2;}
			| /* epsilon */		{$$=BuildType("void");}
			;

type			: scalar_type 	{$$=$1;}
			| array_type 		{$$=$1;}
			;

scalar_type		: INTEGER 	{$$=BuildType("integer");}
			| REAL 			{$$=BuildType("real");}
			| BOOLEAN		{$$=BuildType("boolean");}
			| STRING		{$$=BuildType("string");}
			;

array_type		: ARRAY int_const TO int_const OF type
			{
				int sz=($4->ival)-($2->ival)+1;
				$$=AddArrayToType($6,sz);
			}
			;

stmt			: compound_stmt
			| simple_stmt
			| cond_stmt
			| while_stmt
			| for_stmt
			| return_stmt {CheckFuncRet(return_buf,$1); return_buf=NULL;}
			| proc_call_stmt
			;

compound_stmt		: BEG 		{symbol_table->current_level++;}
			  opt_decl_list
			  opt_stmt_list
			  END
			{
				PrintSymbolTable(symbol_table);
				PopTableEntry(symbol_table);
				symbol_table->current_level--;
			}

			;

opt_stmt_list		: stmt_list
			| /* epsilon */
			;

stmt_list		: stmt_list stmt
			| stmt
			;

simple_stmt		: var_ref OP_ASSIGN boolean_expr MK_SEMICOLON
			 {
			 	if(!CheckConstAssign($1)){

					CheckType($1,$3);
				}
			 }
			| PRINT boolean_expr MK_SEMICOLON
			| READ boolean_expr MK_SEMICOLON
			;

proc_call_stmt		: ID MK_LPAREN opt_boolean_expr_list MK_RPAREN MK_SEMICOLON
			;

cond_stmt		: IF boolean_expr THEN
			  opt_stmt_list
			  ELSE
			  opt_stmt_list
			  END IF
			| IF boolean_expr THEN opt_stmt_list END IF
			;

while_stmt		: WHILE boolean_expr DO
			  opt_stmt_list
			  END DO
			;

for_stmt		: FOR ID OP_ASSIGN int_const TO int_const DO
			  opt_stmt_list
			  END DO
			;

return_stmt		: RETURN boolean_expr MK_SEMICOLON{$$=$2;}
			;

opt_boolean_expr_list	: boolean_expr_list
			| /* epsilon */ {$$=NULL;}
			;

boolean_expr_list	: boolean_expr_list MK_COMMA boolean_expr {$$=BuildExprList($1,$3);}
			| boolean_expr {$$=BuildExprList(NULL,$1);}
			;

boolean_expr		: boolean_expr OP_OR boolean_term {$$=$3;}
			| boolean_term
			;

boolean_term		: boolean_term OP_AND boolean_factor {$$=$3;}
			| boolean_factor
			;

boolean_factor		: OP_NOT boolean_factor {$$=$2;}
			| relop_expr
			;

relop_expr		: expr rel_op expr
			{
				$$=RelationalOp($1,$3,$2);
			}
			| expr
			;

rel_op			: OP_LT
			| OP_LE
			| OP_EQ
			| OP_GE
			| OP_GT
			| OP_NE
			;

expr			: expr add_op term {$$=$3;}
			| term
			;

add_op			: OP_ADD
			| OP_SUB
			;

term			: term mul_op factor
			   {
				$$=$1;
				MulOp($1,$3,$2);
			   }
			| factor {$$=$1;}
			;

mul_op			: OP_MUL
			| OP_DIV
			| OP_MOD
			;

factor			: var_ref
			| OP_SUB var_ref {$$=$2;}
			| MK_LPAREN boolean_expr MK_RPAREN {$$=$2;}
			| OP_SUB MK_LPAREN boolean_expr MK_RPAREN {$$=$3;}
			| ID MK_LPAREN opt_boolean_expr_list MK_RPAREN {$$=FunctionCall($1,$3);}
			| OP_SUB ID MK_LPAREN opt_boolean_expr_list MK_RPAREN
			{
				$$=FunctionCall($2,$4);
			}
			| literal_const {$$=ConstExpr($1);}
			;

var_ref			: ID		{$$=FindVarRef(symbol_table,$1);}
			| var_ref dim
			{
				$1->current_dimension++;
				$$=$1;
			}
			;

dim			: MK_LB boolean_expr MK_RB
			;

%%

int yyerror( char *msg )
{
	(void) msg;
	fprintf( stderr, "\n|--------------------------------------------------------------------------\n" );
	fprintf( stderr, "| Error found in Line #%d: %s\n", linenum, buf );
	fprintf( stderr, "|\n" );
	fprintf( stderr, "| Unmatched token: %s\n", yytext );
	fprintf( stderr, "|--------------------------------------------------------------------------\n" );
	exit(-1);
}

