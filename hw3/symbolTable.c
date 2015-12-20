#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolTable.h"

SymbolTable* BuildSymbolTable(){
	SymbolTable *new=(SymbolTable*) malloc(sizeof(SymbolTable));
	new->current_level=0;
	new->pos=0;
	new->capacity=4;
	new->Entries = (TableEntry**) malloc(sizeof(TableEntry*)*4);
	return new;
}

IdList* BuildIdList(){
	IdList* new=(IdList*) malloc(sizeof(IdList));
	new->pos=0;
	new->capacity=4;
	new->Ids= (char**) malloc(sizeof(char*)*4);
	return new;
}

void InsertIdList(IdList* l,char* id){
	char* id_tmp=strdup(id);

	if(l->pos == l->capacity){
		l->capacity*=2;
		char** tmp_Ids=l->Ids;
		l->Ids= (char**) malloc(sizeof(char*)*l->capacity);
		int i;
		for(i=0;i<l->pos;i++){
			(l->Ids)[i] = tmp_Ids[i];
		}
		free(tmp_Ids);
	}

	l->Ids[l->pos++] = id_tmp;

}
//just for debug
void PrintIdList(IdList* l){
	int i;
	printf("IdList:");
	for(i=0;i<l->pos;i++){
		printf("%s ",l->Ids[i]);
	}
	printf("\n");
}

void ResetIdList(IdList* l){
	int i;
	for(i=(l->pos)-1;i>=0;i--){
		free(l->Ids[i]);
	}
	free(l->Ids);
	l->pos=0;
	l->capacity=4;
	l->Ids= (char**) malloc(sizeof(char*)*4);
}

TableEntry* BuildTableEntry(char* name, char* kind,int level,Type* type,char* attri){
	TableEntry* new=(TableEntry*)malloc(sizeof(TableEntry));
	strcpy(new->name,name);
	strcpy(new->kind,kind);
	new->level=level;
	new->type=type;
	strcpy(new->attribute,attri);
	return new;
}

void InsertTableEntry(SymbolTable* t,TableEntry* e){
	//grow the capacity
	if(t->pos == t->capacity){
		t->capacity*=2;
		TableEntry** tmp_entries=t->Entries;
		t->Entries = (TableEntry**) malloc(sizeof(TableEntry*)*t->capacity);
		int i;
		for(i=0;i<t->pos;i++){
			(t->Entries)[i] = tmp_entries[i];
		}
		free(tmp_entries);
	}

	t->Entries[t->pos++] = e;
}

void PrintSymbolTable(SymbolTable* t){
	int i;
	TableEntry* ptr;
	printf("Name\tKind\tLevel\tType\tAttribute\n");
	printf("-------------------------------------------\n");
	for(i=0;i<t->pos;i++){
		ptr=t->Entries[i];
		if(ptr->level==t->current_level){
			printf("%s %s %d\n",ptr->name,ptr->kind,ptr->level);
		}
	}
	printf("-------------------------------------------\n");
}

//for debug
void yytextPrint(){
	printf("%s ",yytext);
}

Type* BuildType(const char* typename){
	Type* new = (Type*)malloc(sizeof(Type));
	strcpy(new->name,typename);
	new->array_signature=NULL; /*FIXME*/
	return new;
}

Value* BuildValue(const char* typename,const char* val){
	Type* t=BuildType(typename);
	Value* v=(Value*) malloc(sizeof(Value));
	v->type=t;
	if(strcmp(t->name,"float")==0 ){
		v->dval=atof(val);
	}else if(strcmp(t->name,"string")==0){
		v->sval=strdup(val);
	}else if(strcmp(t->name,"int")==0){
		v->ival=atoi(val);
	}else if(strcmp(t->name,"oct_int")==0){
		v->ival=strtol(val,NULL,8);
	}else if(strcmp(t->name,"scientific")==0){
		v->sval=strdup(val);
	}else if(strcmp(t->name,"boolean")==0){
		v->sval=strdup(val);
	}
	return v;
}
