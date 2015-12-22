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
	l->pos=0;
	l->capacity=4;
	l->Ids= (char**) malloc(sizeof(char*)*4);
}

TableEntry* BuildTableEntry(char* name,const char* kind,int level,Type* type,Attribute* attri){
	TableEntry* new=(TableEntry*)malloc(sizeof(TableEntry));
	strcpy(new->name,name);
	strcpy(new->kind,kind);
	new->level=level;
	new->type=type;
	new->attri=attri;
	return new;
}

void InsertTableEntry(SymbolTable* t,TableEntry* e){
	//grow the capacity
	if(t->pos == t->capacity){
		t->capacity*=2;
		TableEntry** tmp_entries=t->Entries;
		t->Entries = (TableEntry**) malloc(sizeof(TableEntry*)*(t->capacity));
		int i;
		for(i=0;i<t->pos;i++){
			(t->Entries)[i] = tmp_entries[i];
		}
		free(tmp_entries);
	}

	t->Entries[t->pos++] = e;
}

void PopTableEntry(SymbolTable* s){
	int i;
	TableEntry* ptr;
	for(i=0;i<s->pos;i++){
		ptr=s->Entries[i];
		if(ptr->level==s->current_level){
			free(ptr);
			if(i < (s->pos)-1){ //如果不是最後的話
				s->Entries[i]=s->Entries[--(s->pos)]; //把最後的拿過來
				i--;//同一個
				continue; //再檢查一次
			}else{
				s->pos--;
			}
		}
	}
}

void InsertTableEntryFromList(SymbolTable* t,IdList* l,const char* kind,Type* type,Attribute* attri){
	int i;
	for(i=0; i < l->pos; i++){
		TableEntry* new_entry=BuildTableEntry(l->Ids[i],kind,\
		t->current_level,type,attri);
		InsertTableEntry(t,new_entry);
	}
}

void PrintSymbolTable(SymbolTable* t){
	int i;
	TableEntry* ptr;
	printf("%-32s\t%-11s\t%-11s\t%-17s\t%-11s\t\n","Name","Kind","Level","Type","Attribute");
	for(i=0;i< 110;i++)
		printf("-");
	printf("\n");
	for(i=0;i<t->pos;i++){
		ptr=t->Entries[i];
		if(ptr->level==t->current_level){
			printf("%-32s\t%-11s\t",ptr->name,ptr->kind);
			PrintLevel(ptr->level);
			printf("%-17s\t",PrintType(ptr->type));
			PrintAttribute(ptr->attri);
			printf("\n");
		}

	}
	for(i=0;i< 110;i++)
		printf("-");
	printf("\n");
}

char* PrintType(const Type* t){
	ArraySig* ptr=t->array_signature;
	char* output_buf=(char*)malloc(sizeof(char)*18);
	char tmp_buf[5];
	int name_len=strlen(t->name)+1;
	memset(output_buf,0,18);
	snprintf(output_buf,name_len,"%s",t->name);

	while(ptr!=NULL){
		snprintf(tmp_buf,4,"[%d]",ptr->capacity);
		strcat(output_buf,tmp_buf);
		ptr=ptr->next_dimension;
	}
	return output_buf;
}

void PrintAttribute(Attribute* a){
	if(a==NULL){
		return;
	}else if(a->val!=NULL){
		if(strcmp(a->val->type->name,"string")==0)
			printf("%-11s\t",a->val->sval);
		else if(strcmp(a->val->type->name,"integer")==0)
			printf("%-11d\t",a->val->ival);
		else if(strcmp(a->val->type->name,"float")==0)
			printf("%-11f\t",a->val->dval);
		else if(strcmp(a->val->type->name,"boolean")==0)
			printf("%-11s\t",a->val->sval);
	}else if(a->type_list!=NULL){
		TypeList* l=a->type_list;
		int i;
		printf("%s",PrintType(l->types[0]));
		for(i=1;i<l->current_size;i++){
			printf(",%s",PrintType(l->types[i]));
		}
	}
}

void PrintLevel(int l){
	if(l==0){
		printf("%d%-10s\t",l,"(global)");
	}else{
		printf("%d%-10s\t",l,"(local)");
	}
}

//for debug
void yytextPrint(){
	printf("%s ",yytext);
}

Type* BuildType(const char* typename){
	Type* new = (Type*)malloc(sizeof(Type));
	strcpy(new->name,typename);
	new->array_signature=NULL; /*TODO*/
	return new;
}

Type* AddArrayToType(Type* t,int d){
	ArraySig* it;
	if(t->array_signature==NULL){
		t->array_signature=(ArraySig*)malloc(sizeof(ArraySig));
		t->array_signature->capacity=d;
		t->array_signature->next_dimension=NULL;
	}else{
		it=t->array_signature;
		while(it->next_dimension!=NULL) it=it->next_dimension;
		it->next_dimension=(ArraySig*)malloc(sizeof(ArraySig));
		it->next_dimension->capacity=d;
		it->next_dimension->next_dimension=NULL;
	}
	return t;
}

TypeList* AddTypeToList(TypeList* l,Type* t,int cnt){
	int i;
	if(l==NULL){
		l=(TypeList*)malloc(sizeof(TypeList));
		l->types=(Type**)malloc(sizeof(Type**)*4);
		l->capacity=4;
		l->current_size=0;
	}
	if(l->current_size <= l->capacity+cnt){
		l->capacity*=2;
		Type** tmp=l->types;
		l->types=(Type**)malloc(sizeof(Type**)*l->capacity);
		for(i=0;i<l->current_size;i++){
			(l->types)[i] = tmp[i];
		}
		free(tmp);
	}
	for(i=0;i<cnt;i++){
		l->types[l->current_size++]=t;
	}
	return l;
}
TypeList* ExtendTypelist(TypeList* dest,TypeList* src){
	int i;
	if(dest->capacity- dest->current_size < src->current_size){
		while((dest->capacity - dest->current_size) < src->current_size){
			dest->capacity*=2;
		}
		Type** tmp=dest->types;
		dest->types=(Type**)malloc(sizeof(Type**)*dest->capacity);

		for(i=0;i<dest->current_size;i++){
			(dest->types)[i] = tmp[i];
		}
		free(tmp);
	}
	for(i=0;i<src->current_size;i++){
		dest->types[dest->current_size++]=src->types[i];
	}
	free(src);
	return dest;
}

Value* BuildValue(const char* typename,const char* val){
	Type* t=BuildType(typename);
	Value* v=(Value*) malloc(sizeof(Value));
	v->type=t;
	if(strcmp(t->name,"float")==0 ){
		v->dval=atof(val);
	}else if(strcmp(t->name,"string")==0){
		v->sval=strdup(val);
	}else if(strcmp(t->name,"integer")==0){
		v->ival=atoi(val);
	}else if(strcmp(t->name,"octal")==0){
		v->ival=strtol(val,NULL,8);
	}else if(strcmp(t->name,"scientific")==0){
		v->sval=strdup(val);
	}else if(strcmp(t->name,"boolean")==0){
		v->sval=strdup(val);
	}
	return v;
}

Attribute* BuildConstAttribute(Value* v){
	Attribute* a=(Attribute*)malloc(sizeof(Attribute));
	a->val=v;
	a->type_list=NULL;
	return a;
}
Attribute* BuildFuncAttribute(TypeList* l){
	Attribute* a=(Attribute*)malloc(sizeof(Attribute));
	a->type_list=l;
	a->val=NULL;
	return a;
}


