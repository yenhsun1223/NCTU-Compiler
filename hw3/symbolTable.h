extern char *yytext;		/* declared by lex */
typedef struct ArraySig ArraySig;
typedef struct Type Type;
typedef struct TypeList TypeList;
typedef struct TableEntry TableEntry;
typedef struct SymbolTable SymbolTable;
typedef struct IdList IdList;
typedef struct Value Value;
typedef struct Attribute Attribute;

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
	char name[16];
	ArraySig* array_signature;
};

struct TypeList{
	int current_size;
	int capacity;
	Type** types;
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
TableEntry* BuildTableEntry(char*,const char*,int,Type*,Attribute*);


void PrintSymbolTable(SymbolTable*);
void PrintLevel(int);
char* PrintType(const Type*);
void PrintIdList(IdList*);
void PrintAttribute(Attribute*);

Attribute* BuildConstAttribute(Value*);
Attribute* BuildFuncAttribute(TypeList*);

IdList* BuildIdList();
void ResetIdList(IdList*);
void InsertIdList(IdList*,char*);

Type* BuildType(const char*);
Type* AddArrayToType(Type*,int);
TypeList* AddTypeToList(TypeList*,Type*,int);
TypeList* ExtendTypelist(TypeList*,TypeList*);

Value* BuildValue(const char*,const char*);
