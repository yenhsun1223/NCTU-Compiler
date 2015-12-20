extern char *yytext;		/* declared by lex */
typedef struct ArraySig ArraySig;
typedef struct Type Type;
typedef struct TableEntry TableEntry;
typedef struct SymbolTable SymbolTable;
typedef struct IdList IdList;
typedef struct Value Value;

struct ArraySig{
	int capacity;
	ArraySig* next_dimension;
};

struct Type{
	char name[16];
	ArraySig* array_signature;
};

struct TableEntry {
	char name[33];
	char kind[20];
	int level;
	Type* type;
	char attribute[128];

};

struct SymbolTable {
	int current_level;
	int pos;
	int capacity;
	TableEntry** Entries;
} ;

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
void PrintSymbolTable(SymbolTable*);
void InsertTableEntry(SymbolTable*,TableEntry*);
TableEntry* BuildTableEntry(char*, char*,int,Type*,char*);

IdList* BuildIdList();
void PrintIdList(IdList*);
void ResetIdList(IdList*);
void InsertIdList(IdList*,char*);

Type* BuildType(const char*);
void PrintType(const Type*);
Value* BuildValue(const char*,const char*);
