extern FILE* outfp;
extern int scope;
extern struct SymTable *symbolTable;	// main symbol table

void GenProgramStart(char*);
void GenProgramEnd();

void GenMethod(char*,int,char*,char*);
void GenGlobalVar(char*,struct PType*);
void GenPrintStart();
void GenPrint(struct expr_sem*);
void LoadConstToStack(struct ConstAttr*);
