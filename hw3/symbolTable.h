extern char *yytext;		/* declared by lex */
extern int linenum;		/* declared in lex.l */
extern int Opt_D;		/* declared in lex.l */
extern int loop_cnt;
extern char* fn;

typedef struct ArraySig ArraySig;
typedef struct Type Type;
typedef struct TypeList TypeList;
typedef struct TableEntry TableEntry;
typedef struct SymbolTable SymbolTable;
typedef struct IdList IdList;
typedef struct Value Value;
typedef struct Attribute Attribute;
typedef struct Expr Expr;
typedef struct ExprList ExprList;
typedef struct FunctionInnvocation FunctionInnvocation;
extern SymbolTable* symbol_table;
struct SymbolTable {
	int current_level;
	int pos;
	int capacity;
	TableEntry** Entries;
} ;

struct TableEntry {
	char name[33];
	char kind[20];
	int level;
	Type* type;
	Attribute* attri;

};

struct ArraySig{
	int capacity;
	ArraySig* next_dimension;
};

struct Attribute{
	Value* val;
	TypeList* type_list;
};

struct Type{
	char name[32];
	ArraySig* array_signature;
};

struct TypeList{
	int current_size;
	int capacity;
	Type** types;
};

struct Expr{
	char kind[16]; //var,func,const
	char name[33];
	Type* type;
	TableEntry* entry;
	int current_dimension;
	TypeList* para;
};

struct ExprList{
	int current_size;
	int capacity;
	Expr** exprs;
};

struct IdList{
	int pos;
	int capacity;
	char** Ids;
};

struct Value{
	Type* type;
	int ival;
	double dval;
	char* sval;
};

SymbolTable* BuildSymbolTable();
void InsertTableEntry(SymbolTable*,TableEntry*);
void InsertTableEntryFromList(SymbolTable*,IdList*,const char*,Type*,Attribute*);
void PopTableEntry(SymbolTable*);
void PopTableEntryByName(SymbolTable*,char*);
TableEntry* BuildTableEntry(char*,const char*,int,Type*,Attribute*);

void PrintSymbolTable(SymbolTable*);
void PrintLevel(int);
char* PrintType(const Type*,int);
void PrintIdList(IdList*);
void PrintAttribute(Attribute*);

Attribute* BuildConstAttribute(Value*);
Attribute* BuildFuncAttribute(TypeList*);

Expr* FindVarRef(SymbolTable*,char*);
Expr* FindFuncRef(SymbolTable*,char*);
Expr* ConstExpr(Value*);
Expr* FunctionCall(char*,ExprList*);
Expr* RelationalOp(Expr*,Expr*,char*);
Expr* MulOp(Expr*,Expr*,char*);
Expr* AddOp(Expr*,Expr*,char*);
Expr* BooleanOp(Expr*,Expr*,char*);

ExprList* BuildExprList(ExprList*,Expr*);

TableEntry* FindEntryInScope(SymbolTable*,char*);
TableEntry* FindEntryInGlobal(SymbolTable*,char*);
TableEntry* FindEntryLoopVar(SymbolTable*,char*);

IdList* BuildIdList();
void ResetIdList(IdList*);
void InsertIdList(IdList*,char*);

Type* BuildType(const char*);
Type* AddArrayToType(Type*,int);

TypeList* AddTypeToList(TypeList*,Type*,int);
TypeList* ExtendTypelist(TypeList*,TypeList*);

Value* BuildValue(const char*,const char*);
Value* SubOp(Value*);

int CheckConstAssign(Expr*);
int CheckType(Expr*,Expr*);
int CheckFuncParaNum(Expr*);
int CheckFuncRet(Type*,Expr*);
int CanCoerce(Expr*,Expr*);
int CheckSimple(Expr*);
int CheckFilename(char*,char*);
