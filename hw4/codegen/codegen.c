
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symtab.h"
#include"codegen.h"

void pushIns(char* ins){
	insList.list[insList.size++]=strdup(ins);
}
void GenExprIns(){
	int i;
	for(i=0;i<insList.size;i++){
		printf("%s\n",insList.list[i]);
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
	fprintf(outfp, "; %s\n",pname);
	fprintf(outfp, ".class public %s\n",pname);
	fprintf(outfp, ".super java/lang/Object\n\n");

}

void GenProgramEnd(){
	fprintf(outfp, "return\n");
	fprintf(outfp, ".end method\n");
}
void GenMethod(char* name,int stack_lim,char* para,char* ret){
	fprintf(outfp, ".field public static _sc Ljava/util/Scanner;\n ");
	fprintf(outfp, ".method public static %s(%s)%s\n",name,para,ret);
	fprintf(outfp, ".limit stack %d ; up to %ditems can be pushed\n",stack_lim,stack_lim);
	fprintf(outfp, ".limit locals 64 ; up to 64 varibles can be pushed\n\n");
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
		struct SymNode* lookup= lookupSymbol(symbolTable,expr->varRef->id,scope,__FALSE);
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
			else if(lookup->category==VARIABLE_t && lookup->scope!=0){
				switch(expr->pType->type){
					case INTEGER_t:
						snprintf(insBuf,sizeof(insBuf), "iload_%d\n",lookup->attribute->var_no);
						break;
					case REAL_t:
						snprintf(insBuf,sizeof(insBuf), "fload_%d\n",lookup->attribute->var_no);
						break;
					case BOOLEAN_t:
						snprintf(insBuf,sizeof(insBuf), "iload_%d\n",lookup->attribute->var_no);
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
			pushIns(insBuf);
			memset(insBuf,0,sizeof(insBuf));
		}
	}
}
void GenSaveExpr(struct expr_sem* expr){
	if(expr->varRef){
		struct SymNode* lookup= lookupSymbol(symbolTable,expr->varRef->id,scope,__FALSE);
		if(lookup){
			if(lookup->category==VARIABLE_t && lookup->scope!=0){ //varible but not global
				switch(expr->pType->type){
					case INTEGER_t:
						fprintf(outfp, "istore %d\n",lookup->attribute->var_no);
						break;
					case REAL_t:
						fprintf(outfp, "fstore %d\n",lookup->attribute->var_no);
						break;
					case BOOLEAN_t:
						fprintf(outfp, "istore %d\n",lookup->attribute->var_no);
						break;
				}
			}
			else if(lookup->category==VARIABLE_t && lookup->scope==0){
				switch(expr->pType->type){
					case INTEGER_t:
						fprintf(outfp, "putstatic %s/%s I",fileName,lookup->name);
						break;
					case REAL_t:
						fprintf(outfp, "putstatic %s/%s F",fileName,lookup->name);
						break;
					case BOOLEAN_t:
						fprintf(outfp, "putstatic %s/%s Z",fileName,lookup->name);
						break;
				}
			}
		}
	}
	fprintf(outfp, "\n");
}

void GenPrintStart(){
	fprintf(outfp, "getstatic java/lang/System/out Ljava/io/PrintStream;\n");
}
void GenPrint(struct expr_sem* expr){
	GenExprIns();
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
		GenSaveExpr(expr);
	}
	ClearExprIns();
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
	GenLoadExpr(op2);
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
			if(op1 && op2){
				if(op1->pType->type == INTEGER_t){
					snprintf(insBuf,sizeof(insBuf), "isub\n");
				}
				else if(op1->pType->type == REAL_t){
					snprintf(insBuf,sizeof(insBuf), "fsub\n");
				}
			}else{
				if(op1->pType->type == INTEGER_t){
					snprintf(insBuf,sizeof(insBuf), "ineg\n");
				}
				else if(op1->pType->type == REAL_t){
					snprintf(insBuf,sizeof(insBuf), "fneg\n");
				}
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
}
void GenRelational( struct expr_sem *op1, OPERATOR operator, struct expr_sem *op2){
	GenLoadExpr(op2);
	if(op1->pType->type == INTEGER_t){
		pushIns("isub\n");
	}
	else if(op1->pType->type == REAL_t){
		pushIns("fcmpl\n");
	}
	switch(operator){
		case LT_t:
			pushIns( "iflt L1\n");
			break;
		case LE_t:
			pushIns( "ifle L1\n");
			break;
		case NE_t:
			pushIns( "ifne L1\n");
			break;
		case GE_t:
			pushIns( "ifge L1\n");
			break;
		case GT_t:
			pushIns( "ifgt L1\n");
			break;
		case EQ_t:
			pushIns( "ifeq L1\n");
			break;
	}
	pushIns( "iconst_0\n"); //false
	pushIns( "goto L2\n");
	pushIns( "L1:\n");
	pushIns( "iconst_1\n");//true
	pushIns( "L2:\n");
}
