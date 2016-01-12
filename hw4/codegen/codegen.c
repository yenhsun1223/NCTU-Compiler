
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
	fprintf(outfp, ".method public static %s(%s)%s\n",name,para,ret);
	fprintf(outfp, ".limit stack %d ; up to %ditems can be pushed\n",stack_lim,stack_lim);
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
						fprintf(outfp, "ldc %f\n",lookup->attribute->constVal->value.realVal);
						break;
					case BOOLEAN_t:
						fprintf(outfp, "iconst_%d\n",lookup->attribute->constVal->value.booleanVal);
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
}
void LoadConstToStack(struct ConstAttr* constattr){
	if(constattr->category==STRING_t){
		fprintf(outfp, "ldc \"%s\"\n",constattr->value.stringVal);
	}
	else if(constattr->category==INTEGER_t){
		fprintf(outfp, "sipush %d\n",constattr->value.integerVal);
	}
	else if(constattr->category==REAL_t){
		fprintf(outfp, "ldc %f\n",constattr->value.realVal);
	}
	else if(constattr->category==BOOLEAN_t){
		fprintf(outfp, "iconst_%d\n",constattr->value.booleanVal);
	}
}
