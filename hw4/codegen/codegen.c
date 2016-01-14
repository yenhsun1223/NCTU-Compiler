
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symtab.h"
#include"codegen.h"


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
	if(expr->varRef){
		struct SymNode* lookup= lookupSymbol(symbolTable,expr->varRef->id,scope,__FALSE);
		if(lookup){
			if(lookup->category==CONSTANT_t){
				switch(expr->pType->type){
					case INTEGER_t:
						fprintf(outfp, "sipush %d\n",lookup->attribute->constVal->value.integerVal);
						break;
					case REAL_t:
						fprintf(outfp, "ldc %lf\n",lookup->attribute->constVal->value.realVal);
						break;
					case BOOLEAN_t:
						fprintf(outfp, "iconst_%d\n",lookup->attribute->constVal->value.booleanVal);
						break;
				}
			}
			else if(lookup->category==VARIABLE_t && lookup->scope!=0){
				switch(expr->pType->type){
					case INTEGER_t:
						fprintf(outfp, "iload_%d\n",lookup->attribute->var_no);
						break;
					case REAL_t:
						fprintf(outfp, "fload_%d\n",lookup->attribute->var_no);
						break;
					case BOOLEAN_t:
						fprintf(outfp, "iload_%d\n",lookup->attribute->var_no);
						break;
				}
			}
			else if(lookup->category==VARIABLE_t && lookup->scope==0){
				switch(expr->pType->type){
					case INTEGER_t:
						fprintf(outfp, "getstatic %s/%s I\n",fileName,lookup->name);
						break;
					case REAL_t:
						fprintf(outfp, "getstatic %s/%s F\n",fileName,lookup->name);
						break;
					case BOOLEAN_t:
						fprintf(outfp, "getstatic %s/%s Z\n",fileName,lookup->name);
						break;
				}
			}
		}
	}
}

void GenPrintStart(){
	fprintf(outfp, "getstatic java/lang/System/out Ljava/io/PrintStream;\n");
}
void GenPrint(struct expr_sem* expr){
	if(expr->varRef){
		struct SymNode* lookup= lookupSymbol(symbolTable,expr->varRef->id,scope,__FALSE);
		if(lookup){
			if(lookup->category==CONSTANT_t){
				switch(expr->pType->type){
					case INTEGER_t:
						fprintf(outfp, "sipush %d\n",lookup->attribute->constVal->value.integerVal);
						break;
					case REAL_t:
						fprintf(outfp, "ldc %lf\n",lookup->attribute->constVal->value.realVal);
						break;
					case BOOLEAN_t:
						fprintf(outfp, "iconst_%d\n",lookup->attribute->constVal->value.booleanVal);
						break;
				}
			}
			else if(lookup->category==VARIABLE_t && lookup->scope!=0){
				switch(expr->pType->type){
					case INTEGER_t:
						fprintf(outfp, "iload_%d\n",lookup->attribute->var_no);
						break;
					case REAL_t:
						fprintf(outfp, "fload_%d\n",lookup->attribute->var_no);
						break;
					case BOOLEAN_t:
						fprintf(outfp, "iload_%d\n",lookup->attribute->var_no);
						break;
				}
			}
			else if(lookup->category==VARIABLE_t && lookup->scope==0){
				switch(expr->pType->type){
					case INTEGER_t:
						fprintf(outfp, "getstatic %s/%s I\n",fileName,lookup->name);
						break;
					case REAL_t:
						fprintf(outfp, "getstatic %s/%s F\n",fileName,lookup->name);
						break;
					case BOOLEAN_t:
						fprintf(outfp, "getstatic %s/%s Z\n",fileName,lookup->name);
						break;
				}
			}
		}
	}
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
					fprintf(outfp, "istore %d\n",lookup->attribute->var_no);
					break;
				case REAL_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextFloat()F\n");
					fprintf(outfp, "fstore %d\n",lookup->attribute->var_no);
					break;
				case BOOLEAN_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextBoolean()Z\n");
					fprintf(outfp, "istore %d\n",lookup->attribute->var_no);
					break;
			}
		}else if(lookup->category==VARIABLE_t && lookup->scope==0){
			switch(expr->pType->type){
				case INTEGER_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextInt()I\n");
					fprintf(outfp, "putstatic %s/%s I\n",fileName,lookup->name);
					break;
				case REAL_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextFloat()F\n");
					fprintf(outfp, "putstatic %s/%s F\n",fileName,lookup->name);
					break;
				case BOOLEAN_t:
					fprintf(outfp, "invokevirtual java/util/Scanner/nextBoolean()Z\n");
					fprintf(outfp, "putstatic %s/%s Z\n",fileName,lookup->name);
					break;
			}
		}
	}
	fprintf(outfp, "\n");
}
void LoadConstToStack(struct ConstAttr* constattr){
	if(constattr->category==STRING_t){
		fprintf(outfp, "ldc \"%s\"\n",constattr->value.stringVal);
	}
	else if(constattr->category==INTEGER_t){
		fprintf(outfp, "sipush %d\n",constattr->value.integerVal);
	}
	else if(constattr->category==REAL_t){
		fprintf(outfp, "ldc %lf\n",constattr->value.realVal);
	}
	else if(constattr->category==BOOLEAN_t){
		fprintf(outfp, "iconst_%d\n",constattr->value.booleanVal);
	}
}
void GenAssign(struct expr_sem* expr){
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
				printf("global\n");
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

