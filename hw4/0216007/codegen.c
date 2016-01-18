
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "header.h"
#include "symtab.h"
#include"codegen.h"
int label_count=-1; //count loop label
//int tf_count=-1; //count Ltrue,Lfalse;
struct loop_stack loopStack;
//struct cond_stack condStack;


void pushIns(char* ins){
	insList.list[insList.size++]=strdup(ins);
}
void GenExprIns(){
	int i;
	for(i=0;i<insList.size;i++){
		fprintf(outfp, "%s",insList.list[i]);
		free(insList.list[i]);
	}
	insList.size=0;
}

void ClearExprIns(){
	int i;
	for(i=0;i<insList.size;i++){
		free(insList.list[i]);
	}
	insList.size=0;
}

void GenProgramStart(char* pname){
	loopStack.top=-1;
	//condStack.top=-1;
	fprintf(outfp, "; %s\n",pname);
	fprintf(outfp, ".class public %s\n",pname);
	fprintf(outfp, ".super java/lang/Object\n\n");
	fprintf(outfp, ".field public static _sc Ljava/util/Scanner;\n ");

}

void GenProgramEnd(){
	fprintf(outfp, "return\n");
	fprintf(outfp, ".end method\n");
}
void GenMethod(char* name,int stack_lim,char* para,char* ret){
	fprintf(outfp, "\n;main start\n");
	fprintf(outfp, ".method public static %s(%s)%s\n",name,para,ret);
	fprintf(outfp, ".limit stack %d ; up to %ditems can be pushed\n",stack_lim,stack_lim);
	fprintf(outfp, ".limit locals 128 ; up to 64 varibles can be pushed\n\n");
}

void GenGlobalVar(char* pname,struct PType* type){
	if(type->type == INTEGER_t){
		fprintf(outfp, ".field public static %s %s\n",pname,"I");
	}
	else if(type->type == BOOLEAN_t){
		fprintf(outfp, ".field public static %s %s\n",pname,"Z");
	}
	else if(type->type == REAL_t){
		fprintf(outfp, ".field public static %s %s\n",pname,"F");
	}
}

void GenLoadExpr(struct expr_sem* expr){
	if(!expr) return;
	if(expr->varRef){
		struct SymNode*	lookup= lookupLoopVar(symbolTable,expr->varRef->id);
		if(lookup){
			snprintf(insBuf,sizeof(insBuf), "iload %d\n",lookup->attribute->var_no);
		}
		else
		{
			lookup= lookupSymbol(symbolTable,expr->varRef->id,scope,__FALSE);
			if(lookup){
				if(lookup->category==CONSTANT_t){
					switch(expr->pType->type){
						case INTEGER_t:
							snprintf(insBuf,sizeof(insBuf), "sipush %d\n",lookup->attribute->constVal->value.integerVal);
							break;
						case REAL_t:
							snprintf(insBuf,sizeof(insBuf), "ldc %lf\n",lookup->attribute->constVal->value.realVal);
							break;
						case BOOLEAN_t:
							snprintf(insBuf,sizeof(insBuf), "iconst_%d\n",lookup->attribute->constVal->value.booleanVal);
							break;
					}
				}
				else if((lookup->category==VARIABLE_t ||lookup->category==PARAMETER_t)&& lookup->scope!=0){
					switch(expr->pType->type){
						case INTEGER_t:
							snprintf(insBuf,sizeof(insBuf), "iload %d\n",lookup->attribute->var_no);
							break;
						case REAL_t:
							snprintf(insBuf,sizeof(insBuf), "fload %d\n",lookup->attribute->var_no);
							break;
						case BOOLEAN_t:
							snprintf(insBuf,sizeof(insBuf), "iload %d\n",lookup->attribute->var_no);
							break;
					}
				}
				else if(lookup->category==VARIABLE_t && lookup->scope==0){
					switch(expr->pType->type){
						case INTEGER_t:
							snprintf(insBuf,sizeof(insBuf), "getstatic %s/%s I\n",fileName,lookup->name);
							break;
						case REAL_t:
							snprintf(insBuf,sizeof(insBuf), "getstatic %s/%s F\n",fileName,lookup->name);
							break;
						case BOOLEAN_t:
							snprintf(insBuf,sizeof(insBuf), "getstatic %s/%s Z\n",fileName,lookup->name);
							break;
					}
				}
			}
		}
		pushIns(insBuf);
		memset(insBuf,0,sizeof(insBuf));
	}

}
void GenSaveExpr(struct expr_sem* expr,struct expr_sem* RHS){
	if(expr->varRef){
		struct SymNode* lookup= lookupSymbol(symbolTable,expr->varRef->id,scope,__FALSE);
		if(lookup){
			if(lookup->category==VARIABLE_t && lookup->scope!=0){ //varible but not global
				switch(expr->pType->type){
					case INTEGER_t:
						snprintf(insBuf,sizeof(insBuf), "istore %d\n",lookup->attribute->var_no);
						break;
					case REAL_t:
						if(RHS && RHS->pType->type==INTEGER_t){
							snprintf(insBuf,sizeof(insBuf), "i2f\nfstore %d\n",lookup->attribute->var_no);
						}else{
							snprintf(insBuf,sizeof(insBuf), "fstore %d\n",lookup->attribute->var_no);
						}
						break;
					case BOOLEAN_t:
						snprintf(insBuf,sizeof(insBuf), "istore %d\n",lookup->attribute->var_no);
						break;
				}
			}
			else if(lookup->category==VARIABLE_t && lookup->scope==0){
				switch(expr->pType->type){
					case INTEGER_t:
						snprintf(insBuf,sizeof(insBuf), "putstatic %s/%s I\n",fileName,lookup->name);
						break;
					case REAL_t:
						if(RHS && RHS->pType->type==INTEGER_t){
							snprintf(insBuf,sizeof(insBuf), "i2f\nputstatic %s/%s F\n",fileName,lookup->name);
						}else{
							snprintf(insBuf,sizeof(insBuf), "putstatic %s/%s F\n",fileName,lookup->name);
						}
						break;
					case BOOLEAN_t:
						snprintf(insBuf,sizeof(insBuf), "putstatic %s/%s Z\n",fileName,lookup->name);
						break;
				}
			}
		}
		pushIns(insBuf);
		memset(insBuf,0,sizeof(insBuf));
	}
}
void GenPrintStart(){
	snprintf(insBuf,sizeof(insBuf), "getstatic java/lang/System/out Ljava/io/PrintStream;\n");
	pushIns(insBuf);
	memset(insBuf,0,sizeof(insBuf));
}

void GenPrint(struct expr_sem* expr){
	switch(expr->pType->type){
		case STRING_t:
			fprintf(outfp, "invokevirtual java/io/PrintStream/print(Ljava/lang/String;)V\n");
			break;
		case INTEGER_t:
			fprintf(outfp, "invokevirtual java/io/PrintStream/print(I)V\n");
			break;
		case REAL_t:
			fprintf(outfp, "invokevirtual java/io/PrintStream/print(F)V\n");
			break;
		case BOOLEAN_t:
			fprintf(outfp, "invokevirtual java/io/PrintStream/print(Z)V\n");
			break;
	}
	fprintf(outfp, "\n");

}

void GenReadStart(){
	if(!hasRead){
		fprintf(outfp, "new java/util/Scanner\n");
		fprintf(outfp, "dup\n");
		fprintf(outfp, "getstatic java/lang/System/in Ljava/io/InputStream;\n");
		fprintf(outfp, "invokespecial java/util/Scanner/<init>(Ljava/io/InputStream;)V\n");
		fprintf(outfp, "putstatic %s/_sc Ljava/util/Scanner;\n",fileName);
		fprintf(outfp, "\n");
		hasRead=1;
	}
}
void GenRead(struct expr_sem* expr){
	fprintf(outfp, "getstatic %s/_sc Ljava/util/Scanner;\n",fileName);
	if(expr->varRef){
		struct SymNode* lookup= lookupSymbol(symbolTable,expr->varRef->id,scope,__FALSE);
		if(lookup->category==VARIABLE_t && lookup->scope!=0){
			switch(expr->pType->type){
				case INTEGER_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextInt()I\n");
					break;
				case REAL_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextFloat()F\n");
					break;
				case BOOLEAN_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextBoolean()Z\n");
					break;
			}
		}else if(lookup->category==VARIABLE_t && lookup->scope==0){
			switch(expr->pType->type){
				case INTEGER_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextInt()I\n");
					break;
				case REAL_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextFloat()F\n");
					break;
				case BOOLEAN_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextBoolean()Z\n");
					break;
			}
		}
		ClearExprIns();
		GenSaveExpr(expr,NULL);
		GenExprIns();
	}
	fprintf(outfp, "\n");
}
void LoadConstToStack(struct ConstAttr* constattr){
	if(constattr->category==STRING_t){
		snprintf(insBuf,sizeof(insBuf), "ldc \"%s\"\n",constattr->value.stringVal);
	}
	else if(constattr->category==INTEGER_t){
		snprintf(insBuf,sizeof(insBuf), "sipush %d\n",constattr->value.integerVal);
	}
	else if(constattr->category==REAL_t){
		snprintf(insBuf,sizeof(insBuf), "ldc %lf\n",constattr->value.realVal);
	}
	else if(constattr->category==BOOLEAN_t){
		snprintf(insBuf,sizeof(insBuf), "iconst_%d\n",constattr->value.booleanVal);
	}
	pushIns(insBuf);
	memset(insBuf,0,sizeof(insBuf));
}

void GenArithmetic( struct expr_sem *op1, OPERATOR operator, struct expr_sem *op2){

	//coercion
	if(op1 && op2){//unary need no coercion
		if( ((op1->pType->type==INTEGER_t || op1->pType->type==REAL_t) && \
					(op2->pType->type==INTEGER_t || op2->pType->type==REAL_t)) ) {	// need to consider type coercion
			if( op1->pType->type==INTEGER_t && op2->pType->type==REAL_t) {
				fprintf(outfp,"i2f\n");
				GenExprIns();
				op1->pType->type = REAL_t;
			}else if( op1->pType->type==REAL_t && op2->pType->type==INTEGER_t){
				GenToList("i2f\n");
			}
		}
	}


	switch(operator){
		case ADD_t:
			if(op1->pType->type == INTEGER_t){
				snprintf(insBuf,sizeof(insBuf), "iadd\n");
			}
			else if(op1->pType->type == REAL_t){
				snprintf(insBuf,sizeof(insBuf), "fadd\n");
			}
			break;
		case SUB_t:
			if(op1->pType->type == INTEGER_t){
				snprintf(insBuf,sizeof(insBuf), "isub\n");
			}
			else if(op1->pType->type == REAL_t){
				snprintf(insBuf,sizeof(insBuf), "fsub\n");
			}
			break;
		case MUL_t:
			if(op1->pType->type == INTEGER_t){
				snprintf(insBuf,sizeof(insBuf), "imul\n");
			}
			else if(op1->pType->type == REAL_t){
				snprintf(insBuf,sizeof(insBuf), "fmul\n");
			}
			break;
		case DIV_t:
			if(op1->pType->type == INTEGER_t){
				snprintf(insBuf,sizeof(insBuf), "idiv\n");
			}
			else if(op1->pType->type == REAL_t){
				snprintf(insBuf,sizeof(insBuf), "fdiv\n");
			}
			break;
		case MOD_t:
			snprintf(insBuf,sizeof(insBuf), "irem\n");
			break;
	}
	pushIns(insBuf);
	memset(insBuf,0,sizeof(insBuf));
	GenExprIns();
}
void GenBoolean( struct expr_sem *op1, OPERATOR operator, struct expr_sem *op2){
	GenExprIns();
	switch(operator){
		case AND_t:
			GenToList( "iand\n");
			break;
		case OR_t:
			GenToList( "ior\n");
			break;
		case NOT_t:
			GenToList( "iconst_1\nixor\n");
			break;
	}
}
void GenRelational( struct expr_sem *op1, OPERATOR operator, struct expr_sem *op2){
	loopStack.top++;
	label_count++;
	loopStack.stack[loopStack.top]=label_count;
	if(op1->pType->type == INTEGER_t){
		pushIns("isub\n");
	}
	else if(op1->pType->type == REAL_t){
		pushIns("fcmpl\n");
	}
	switch(operator){
		case LT_t:
			GenToList( "iflt Ltrue_%d\n",loopStack.stack[loopStack.top]);
			break;
		case LE_t:
			GenToList( "ifle Ltrue_%d\n",loopStack.stack[loopStack.top]);
			break;
		case NE_t:
			GenToList( "ifne Ltrue_%d\n",loopStack.stack[loopStack.top]);
			break;
		case GE_t:
			GenToList( "ifge Ltrue_%d\n",loopStack.stack[loopStack.top]);
			break;
		case GT_t:
			GenToList( "ifgt Ltrue_%d\n",loopStack.stack[loopStack.top]);
			break;
		case EQ_t:
			GenToList( "ifeq Ltrue_%d\n",loopStack.stack[loopStack.top]);
			break;
	}
	pushIns( "iconst_0\n"); //false
	GenToList( "goto Lfalse_%d\n",loopStack.stack[loopStack.top]);
	GenToList( "Ltrue_%d:\n",loopStack.stack[loopStack.top]);
	pushIns( "iconst_1\n");//true
	GenToList( "Lfalse_%d:\n",loopStack.stack[loopStack.top]);
	loopStack.top--;
}
void GenFunctionStart(char* id,struct param_sem* params,struct PType* ret){
	struct param_sem *parPtr;
	struct idNode_sem *idPtr;
	snprintf(insBuf,sizeof(insBuf),".method public static %s(",id);
	for( parPtr=params ; parPtr!=0 ; parPtr=(parPtr->next) ) {
		for( idPtr=(parPtr->idlist) ; idPtr!=0 ; idPtr=(idPtr->next) ) {
			switch(parPtr->pType->type){
				case INTEGER_t:
					strncat(insBuf,"I",sizeof(insBuf)-strlen(insBuf));
					break;
				case REAL_t:
					strncat(insBuf,"F",sizeof(insBuf)-strlen(insBuf));
					break;
				case BOOLEAN_t:
					strncat(insBuf,"Z",sizeof(insBuf)-strlen(insBuf));
					break;
			}
		}
	}
	//return value
	switch(ret->type){
		case INTEGER_t:
			strncat(insBuf,")I\n",sizeof(insBuf)-strlen(insBuf));
			break;
		case REAL_t:
			strncat(insBuf,")F\n",sizeof(insBuf)-strlen(insBuf));
			break;
		case BOOLEAN_t:
			strncat(insBuf,")Z\n",sizeof(insBuf)-strlen(insBuf));
			break;
		default:
			strncat(insBuf,")V\n",sizeof(insBuf)-strlen(insBuf));
			break;
	}
	pushIns(insBuf);
	memset(insBuf,0,sizeof(insBuf));
	pushIns(".limit stack 128 ; Sets the maximum size of the operand stack required by the method\n");
	pushIns(".limit locals 128 ; Sets the maximum size of the operand stack required by the method\n");
	GenExprIns();
}
void GenFunctionEnd(struct PType* ret){
	switch(ret->type){
		case INTEGER_t:
			strncat(insBuf,"ireturn\n",sizeof(insBuf)-strlen(insBuf));
			break;
		case REAL_t:
			strncat(insBuf,"freturn\n",sizeof(insBuf)-strlen(insBuf));
			break;
		case BOOLEAN_t:
			strncat(insBuf,"ireturn\n",sizeof(insBuf)-strlen(insBuf));
			break;
		default:
			strncat(insBuf,"return\n",sizeof(insBuf)-strlen(insBuf));
			break;
	}
	strncat(insBuf,".end method\n\n",sizeof(insBuf)-strlen(insBuf));
	pushIns(insBuf);
	memset(insBuf,0,sizeof(insBuf));
	GenExprIns();
}
void GenFunctionCall(char* id){
	struct SymNode *node = 0;
	node = lookupSymbol( symbolTable, id, 0, __FALSE );	// function always in scope 0
	if(node){
		snprintf(insBuf,sizeof(insBuf),"invokestatic %s/%s(",fileName,id);
		struct PTypeList* parPtr;
		for( parPtr=node->attribute->formalParam->params; parPtr!=0 ; parPtr=(parPtr->next) ) {
			switch(parPtr->value->type){
				case INTEGER_t:
					strncat(insBuf,"I",sizeof(insBuf)-strlen(insBuf));
					break;
				case REAL_t:
					strncat(insBuf,"F",sizeof(insBuf)-strlen(insBuf));
					break;
				case BOOLEAN_t:
					strncat(insBuf,"Z",sizeof(insBuf)-strlen(insBuf));
					break;
			}
		}
		switch(node->type->type){
			case INTEGER_t:
				strncat(insBuf,")I\n",sizeof(insBuf)-strlen(insBuf));
				break;
			case REAL_t:
				strncat(insBuf,")F\n",sizeof(insBuf)-strlen(insBuf));
				break;
			case BOOLEAN_t:
				strncat(insBuf,")Z\n",sizeof(insBuf)-strlen(insBuf));
				break;
			default:
				strncat(insBuf,")V\n",sizeof(insBuf)-strlen(insBuf));
				break;
		}
		pushIns(insBuf);
		GenExprIns();
		memset(insBuf,0,sizeof(insBuf));
	}
}
void GenForLoop(char* id,int start,int end){
	loopStack.top++;
	label_count++;
	struct SymNode* ptr;
	ptr=lookupLoopVar(symbolTable,id);
	if(ptr){
		loopStack.stack[loopStack.top]=label_count; //push to stack

		snprintf(insBuf,sizeof(insBuf),"\nsipush %d\nistore %d\nLbegin_%d:\niload %d\nsipush %d\nisub\niflt Ltrue_%d\niconst_0\ngoto Lfalse_%d\nLtrue_%d:\niconst_1\nLfalse_%d:\nifeq Lexit_%d\n"\
	,start,ptr->attribute->var_no,loopStack.stack[loopStack.top],ptr->attribute->var_no,end,loopStack.stack[loopStack.top],loopStack.stack[loopStack.top],loopStack.stack[loopStack.top],loopStack.stack[loopStack.top],loopStack.stack[loopStack.top]);
		pushIns(insBuf);
		GenExprIns();
		memset(insBuf,0,sizeof(insBuf));
	}

}
void GenForLoopEnd(char* id){
	struct SymNode* ptr;
	ptr=lookupLoopVar(symbolTable,id);
	if(ptr){
		snprintf(insBuf,sizeof(insBuf),"iload %d\nsipush 1\niadd\nistore %d\ngoto Lbegin_%d\nLexit_%d:\n\n"\
				,ptr->attribute->var_no,ptr->attribute->var_no,loopStack.stack[loopStack.top],loopStack.stack[loopStack.top]);
		pushIns(insBuf);
		memset(insBuf,0,sizeof(insBuf));
		GenExprIns();
	}
	loopStack.top--;

}
void GenToList(char* fmt,...){
	char tmp[256];

	va_list para;
	va_start(para,fmt);
	vsnprintf(tmp,sizeof(tmp),fmt,para);
	va_end(para);
	pushIns(tmp);
}
void GenCoercion(struct expr_sem* LHS,struct expr_sem* RHS){
	if(LHS->pType->type==INTEGER_t && RHS->pType->type == REAL_t){
		GenToList("i2f\n");
	}
}
void GenNegative(struct expr_sem* expr){
	if(expr->pType->type==INTEGER_t){
		GenToList("ineg\n");
	}else if(expr->pType->type==REAL_t){
		GenToList("fneg\n");
	}
}
