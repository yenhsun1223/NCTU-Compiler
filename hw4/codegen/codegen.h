extern FILE* outfp;
extern int scope;
extern int hasRead;
extern struct SymTable *symbolTable;	// main symbol table
extern char fileName[256];

void GenProgramStart(char*);
void GenProgramEnd();

void GenLoadExpr(struct expr_sem* expr);
void GenSaveExpr(struct expr_sem* expr);

void GenMethod(char*,int,char*,char*);
void GenGlobalVar(char*,struct PType*);
void GenPrintStart();
void GenPrint(struct expr_sem*);
void GenReadStart();
void GenRead(struct expr_sem*);
void LoadConstToStack(struct ConstAttr*);
void GenAssign(struct expr_sem*);
void GenArithmetic( struct expr_sem *op1, OPERATOR operator, struct expr_sem *op2);
