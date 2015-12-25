#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
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
void PrintIdList(IdList* l){
	int i;
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
	if(strcmp(PrintType(e->type,0),"null")==0)return ;
	if(FindEntryInScope(t,e->name)!=NULL ||FindEntryLoopVar(t,e->name)!=NULL){
		printf("Error at Line#%d: %s '%s' is redeclared\n",linenum,e->kind,e->name);
		return;
	}
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
void PopTableEntryByName(SymbolTable* s,char* name){
	int i;
	TableEntry* ptr;
	for(i=0;i<s->pos;i++){
		ptr=s->Entries[i];
		if(ptr->level==s->current_level && strcmp(ptr->name,name)==0){
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
	if(!Opt_D)return;
	int i;
	TableEntry* ptr;

	for(i=0;i< 110;i++)
		printf("=");
	printf("\n");
	printf("%-32s\t%-11s\t%-11s\t%-17s\t%-11s\t\n","Name","Kind","Level","Type","Attribute");
	for(i=0;i< 110;i++)
		printf("-");
	printf("\n");
	for(i=0;i<t->pos;i++){
		ptr=t->Entries[i];
		if(ptr->level==t->current_level){
			printf("%-32s\t%-11s\t",ptr->name,ptr->kind);
			PrintLevel(ptr->level);
			printf("%-17s\t",PrintType(ptr->type,0));
			PrintAttribute(ptr->attri);
			printf("\n");
		}

	}
	for(i=0;i< 110;i++)
		printf("=");
	printf("\n");
}

char* PrintType(const Type* t,int current_dim){
	if(t==NULL)return "type_error";
	ArraySig* ptr=t->array_signature;
	char* output_buf=(char*)malloc(sizeof(char)*18);
	char tmp_buf[5];
	int name_len=strlen(t->name)+1;
	memset(output_buf,0,18);
	snprintf(output_buf,name_len,"%s",t->name);

	while(ptr!=NULL){
		if(current_dim){
			current_dim--;
		}else{
			snprintf(tmp_buf,4,"[%d]",ptr->capacity);
			strcat(output_buf,tmp_buf);
		}
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
		else if(strcmp(a->val->type->name,"real")==0)
			if(strstr(a->val->sval,"e") ||strstr(a->val->sval,"E")){
				printf("%-11s\t",a->val->sval);
			}else{
				printf("%-11f\t",a->val->dval);
			}
		else if(strcmp(a->val->type->name,"boolean")==0)
			printf("%-11s\t",a->val->sval);
	}else if(a->type_list!=NULL){
		TypeList* l=a->type_list;
		int i;
		printf("%s",PrintType(l->types[0],0));
		for(i=1;i<l->current_size;i++){
			printf(",%s",PrintType(l->types[i],0));
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

/*TODO*/
Type* AddArrayToType(Type* t,int d){
	if(d<0){
			strcpy(t->name,"dim_err");
		}
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
	v->sval=NULL;
	v->ival=0;
	if(strcmp(t->name,"real")==0 ){
		v->dval=atof(val);
		v->sval=strdup(val);
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
Value* SubOp(Value* v){
	if(v==NULL)return NULL;
	Type* t=v->type;
	if(strcmp(t->name,"real")==0 ){
		if(strstr(v->sval,"E") ||strstr(v->sval,"e")){
			char* tmp=v->sval;
			v->sval=(char*)malloc(strlen(v->sval)+2);
			v->sval[0]='-';
			strcat(v->sval,tmp);
			free(tmp);
		}else{
			v->dval*=-1.0;
		}
	}else if(strcmp(t->name,"integer")==0){
		v->ival*=-1;
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

TableEntry* FindEntryInScope(SymbolTable* tbl,char* name){
	int i;
	for(i=0;i<tbl->pos;i++){
		TableEntry* it=tbl->Entries[i];
		if(strcmp(name,it->name)==0 && it->level==tbl->current_level){
			return it;
		}
	}
	return NULL;
}

TableEntry* FindEntryInGlobal(SymbolTable* tbl,char* name){
	int i;
	for(i=0;i<tbl->pos;i++){
		TableEntry* it=tbl->Entries[i];
		if(strcmp(name,it->name)==0 && it->level==0){
			return it;
		}
	}
	return NULL;
}
TableEntry* FindEntryLoopVar(SymbolTable* tbl,char* name){
	int i;
	for(i=0;i<tbl->pos;i++){
		TableEntry* it=tbl->Entries[i];
		if(strcmp(name,it->name)==0 && strcmp(it->kind,"loop varible")==0){
			return it;
		}
	}
	return NULL;
}

Expr* FindVarRef(SymbolTable* tbl,char* name){
	Expr* e =(Expr*)malloc(sizeof(Expr));
	TableEntry* tmp=FindEntryInScope(tbl,name);
	if(tmp==NULL)tmp=FindEntryInGlobal(tbl,name);
	if(tmp==NULL)tmp=FindEntryLoopVar(tbl,name);
	if(tmp==NULL){
		strcpy(e->kind,"err");
		strcpy(e->name,name);
		e->current_dimension=0;
		e->entry=NULL;
		e->para=NULL;
		printf("Error at Line#%d: symbol %s is not declared\n",linenum,name);
		return e;
	}
	strcpy(e->kind,"var");
	strcpy(e->name,name);
	e->current_dimension=0;
	e->entry=tmp;
	e->type=e->entry->type;
	return e;
}

Expr* ConstExpr(Value* v){
	Expr* e =(Expr*)malloc(sizeof(Expr));
	strcpy(e->kind,"const");
	e->current_dimension=0;
	e->entry=NULL;
	e->type=v->type;
	return e;
}
/*TODO*/
Expr* FunctionCall(char* name,ExprList* l){
	int i;
	Expr* e =(Expr*)malloc(sizeof(Expr));
	strcpy(e->kind,"function");
	strcpy(e->name,name);
	e->current_dimension=0;
	e->entry=FindEntryInGlobal(symbol_table,name);
	if(e->entry==NULL){
		printf("Error at Line#%d: function %s is not declared\n",linenum,name);
		strcpy(e->kind,"err");
		e->para=NULL;
		return e;
	}else{
		e->type=e->entry->type;
	}
	if(l==NULL){
		e->para=NULL;
	}else{
		TypeList* para=AddTypeToList(NULL,l->exprs[0]->type,1);
		for(i=1;i<l->current_size;i++){
			AddTypeToList(para,l->exprs[i]->type,1);
		}
		e->para=para;
	}
	if(!CheckFuncParaNum(e)){ //numcheck
		int i;
		if(e->para==NULL) return e;//void function
		for(i=0;i<e->para->current_size;i++){
			if(strcmp( PrintType(l->exprs[i]->type,l->exprs[i]->current_dimension),\
						PrintType(e->entry->attri->type_list->types[i],0))!=0){
				//can coerce
				if(strcmp(PrintType(l->exprs[i]->type,l->exprs[i]->current_dimension),"integer")==0 &&\
					strcmp(PrintType(e->entry->attri->type_list->types[i],0),"real")==0 ){
					continue;
				}

				printf("Error at Line#%d: parameter type mismatch\n",linenum);
				return e;
			}
		}
	}
	return e;
}

ExprList* BuildExprList(ExprList* l,Expr* e){
	int i;
	if(l==NULL){
		l=(ExprList*)malloc(sizeof(ExprList));
		l->exprs=(Expr**)malloc(sizeof(Expr**)*4);
		l->capacity=4;
		l->current_size=0;
	}
	if(l->current_size == l->capacity){
		l->capacity*=2;
		Expr** tmp=l->exprs;
		l->exprs=(Expr**)malloc(sizeof(Expr**)*l->capacity);
		for(i=0;i<l->current_size;i++){
			(l->exprs)[i] = tmp[i];
		}
		free(tmp);
	}
	l->exprs[l->current_size++]=e;
	return l;
}

int CheckConstAssign(Expr* r){
	if(strcmp(r->kind,"err")==0) return 0;
	if(r->entry==NULL)return 0;
	if(strcmp(r->entry->kind,"constant")==0){
		printf("Error at Line#%d: constant %s cannot be assigned\n",linenum,r->entry->name);
		return 1;
	}
	else if(strcmp(r->entry->kind,"loop varible")==0){
		printf("Error at Line#%d: loop varible '%s' cannot be assigned\n",linenum,r->entry->name);
		return 1;
	}
	return 0;
}

int CheckType(Expr* LHS,Expr* RHS){
	if(strcmp(LHS->kind,"err")==0 ||strcmp(RHS->kind,"err")==0) return 0;
	if(LHS==NULL || RHS==NULL) return 0;
	if(strcmp( LHS->kind,"error")==0) return 0;
	if(strcmp( PrintType(LHS->type,LHS->current_dimension), PrintType(RHS->type,RHS->current_dimension))!=0){
		if(!CanCoerce(LHS,RHS)){
			printf("Error at Line#%d: type mismatch, LHS= %s, RHS= %s\n",\
					linenum,PrintType(LHS->type,LHS->current_dimension),PrintType(RHS->type,RHS->current_dimension));
			return 1;
		}
	}
	return 0;
}

int CheckFuncParaNum(Expr* e){
	if(strcmp(e->kind,"err")==0) return 0;

	if(strcmp(e->kind,"function")!=0){
		return 0;
	}
	if(strcmp(e->type->name,"void")==0 && e->para==NULL){
		return 0;
	}
	else if(strcmp(e->type->name,"void")==0 && e->para!=NULL){
		printf("Error at Line#%d: too many arguments to function '%s'\n",linenum,e->name);
		return 1;
	}
	else if(strcmp(e->type->name,"void")!=0 && e->para==NULL){
		printf("Error at Line#%d: too few arguments to function '%s'\n",linenum,e->name);
		return 1;
	}
	else if(e->para->current_size > e->entry->attri->type_list->current_size){
		printf("Error at Line#%d: too many arguments to function '%s'\n",linenum,e->name);
		return 1;
	}
	else if(e->para->current_size < e->entry->attri->type_list->current_size){
		printf("Error at Line#%d: too few arguments to function '%s'\n",linenum,e->name);
		return 1;
	}
	return 0;
}

Expr* RelationalOp(Expr* LHS,Expr* RHS,char* op){
	Expr* e =(Expr*)malloc(sizeof(Expr));
	strcpy(e->kind,"var");
	e->current_dimension=0;
	e->entry=NULL;
	e->type=BuildType("boolean");
	if(strcmp(LHS->kind,"err")==0 ||strcmp(RHS->kind,"err")==0){
		strcpy(e->kind,"err");
		return e;
	}
	if(strcmp(LHS->type->name,"string")==0 ||strcmp(RHS->type->name,"string")==0){
		printf("Error at Line#%d: Side of %s is %s type\n",linenum,op,PrintType(LHS->type,LHS->current_dimension));
		strcpy(e->kind,"err");
		return e;
	}
	//both side not the same
	if(!(
		(strcmp(PrintType(LHS->type,LHS->current_dimension),"integer")!=0 &&\
		strcmp(PrintType(RHS->type,RHS->current_dimension),"integer")!=0)\
		||
		(strcmp(PrintType(LHS->type,LHS->current_dimension),"real")!=0 &&\
		strcmp(PrintType(RHS->type,RHS->current_dimension),"real")!=0)
		)
	)
	{
		printf("Error at Line#%d: between %s are not both integer/real\n",linenum,op);
		strcpy(e->kind,"err");
		e->type=BuildType(LHS->type->name);
		return e;
	}

	return e;
}
Expr* AddOp(Expr* LHS,Expr* RHS,char* op){
	if(strcmp(LHS->kind,"err")==0 ||strcmp(RHS->kind,"err")==0) return 0;
	Expr* e =(Expr*)malloc(sizeof(Expr));
	e->current_dimension=0;
	e->entry=NULL;
	strcpy(e->kind,"var");
	//ok both is string
	if(strcmp(LHS->type->name,"string")==0\
	&&strcmp(RHS->type->name,"string")==0){
		e->type=BuildType("string");
		strcpy(e->kind,"var");
		return e;
	}

	//error, one side not int/real
	if(
		(strcmp(PrintType(LHS->type,LHS->current_dimension),"integer")!=0 &&\
		strcmp(PrintType(LHS->type,LHS->current_dimension),"real")!=0)\
		||
		(strcmp(PrintType(RHS->type,RHS->current_dimension),"integer")!=0 &&\
		strcmp(PrintType(RHS->type,RHS->current_dimension),"real")!=0)
	)
	{
		printf("Error at Line#%d: between %s are not integer/real\n",linenum,op);
		strcpy(e->kind,"err");
		e->type=BuildType(LHS->type->name);
		return e;
	}

	//one of side is real
	if(strcmp(PrintType(LHS->type,LHS->current_dimension),"real")==0\
	||strcmp(PrintType(RHS->type,RHS->current_dimension),"real")==0){
		e->type=BuildType("real");
		return e;
	}
	free(e->type);
	free(e);
	return LHS;
}
Expr* BooleanOp(Expr* LHS,Expr* RHS,char* op){
	if(strcmp(LHS->kind,"err")==0 ||strcmp(RHS->kind,"err")==0) return 0;
	Expr* e =(Expr*)malloc(sizeof(Expr));
	e->current_dimension=0;
	e->entry=NULL;
	strcpy(e->kind,"var");
	//error, one side not int/real
	if(
		(strcmp(PrintType(LHS->type,LHS->current_dimension),"boolean")!=0 )||
		(strcmp(PrintType(RHS->type,RHS->current_dimension),"boolean")!=0)
		 )
	{
		printf("Error at Line#%d: operand(s) between '%s' are not boolean\n",linenum,op);
		strcpy(e->kind,"err");
		e->type=BuildType(LHS->type->name);
		return e;
	}

	free(e->type);
	free(e);
	return LHS;
}

Expr* MulOp(Expr* LHS,Expr* RHS,char* op){
	if(strcmp(LHS->kind,"err")==0 ||strcmp(RHS->kind,"err")==0) return 0;
	Expr* e =(Expr*)malloc(sizeof(Expr));
	e->current_dimension=0;
	e->entry=NULL;
	//error , mod without integr
	if(strcmp(op,"mod")==0){
		if(strcmp(PrintType(LHS->type,LHS->current_dimension),"integer")!=0 ||\
				strcmp(PrintType(RHS->type,RHS->current_dimension),"integer")!=0){
			printf("Error at Line#%d: between 'mod' are not integer\n",linenum);
			strcpy(e->kind,"err");
			e->type=BuildType(LHS->type->name);
			return e;
		}
	}
	//error, one side not int/real
	if(
		(strcmp(PrintType(LHS->type,LHS->current_dimension),"integer")!=0 &&\
		strcmp(PrintType(LHS->type,LHS->current_dimension),"real")!=0)\
		||
		(strcmp(PrintType(LHS->type,LHS->current_dimension),"integer")!=0 &&\
		strcmp(PrintType(LHS->type,LHS->current_dimension),"real")!=0)
	)
	{
		printf("Error at Line#%d: between %s are not integer/real\n",linenum,op);
		strcpy(e->kind,"err");
		e->type=BuildType(LHS->type->name);
		return e;
	}
	//one of side is real
	if(strcmp(PrintType(LHS->type,LHS->current_dimension),"real")==0\
	||strcmp(PrintType(RHS->type,RHS->current_dimension),"real")==0){
		e->type=BuildType("real");
		strcpy(e->kind,"err");
		return e;
	}

	free(e->type);
	free(e);
	return LHS;
}
int CheckFuncRet(Type* ft,Expr* e){
	if(ft==NULL)return 0;
	if(strcmp(e->kind,"err")==0) return 0;

	if(strcmp( PrintType(ft,0),PrintType(e->type,e->current_dimension))!=0){
		printf("Error at Line#%d: return type mismatch, ",linenum);
		printf("should return %s, got %s \n",PrintType(ft,0),PrintType(e->type,e->current_dimension));
		return 1;
	}
	return 0;
}

int CanCoerce(Expr* LHS,Expr* RHS){
	if(strcmp(PrintType(LHS->type,LHS->current_dimension),"real")==0 &&\
		strcmp(PrintType(RHS->type,RHS->current_dimension),"integer")==0 ){
		return 1;//can coerce
	}
	return 0;//can not coerce
}

int CheckSimple(Expr* in){
	if(strcmp(PrintType(in->type,in->current_dimension),"integer")!=0 &&
		strcmp(PrintType(in->type,in->current_dimension),"real")!=0 &&
		strcmp(PrintType(in->type,in->current_dimension),"boolean")!=0 &&
		strcmp(PrintType(in->type,in->current_dimension),"string")!=0
			){
		printf("Error at Line#%d: print/read statement's operand must be scalar type\n",linenum);
		return 1;
	}
	return 0;
}
int CheckFilename(char* fn,char* proname){
	int len=strlen(fn);
	fn[len-2]='\0';
	char* p=strstr(fn,"/");
	if(p!=NULL){
		p++;
	}else{
		p=fn;
	}
	if(strcmp(p,proname)!=0){
		printf("Error at Line#%d: program beginning ID inconsist with file name\n",linenum);
	}
	return 0;
}
