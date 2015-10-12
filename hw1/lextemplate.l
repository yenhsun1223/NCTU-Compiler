%{
#define LIST               strcat(buf, yytext)
#define token(t)           {LIST; if (Opt_T) printf("<%s>\n", #t);}
#define tokenChar(c)       {LIST; if (Opt_T) printf("<%c>\n", c);}
#define tokenInteger(t, i) {LIST; if (Opt_T) printf("<%s: %d>\n", #t, i);}
#define tokenOctInteger(t, i) {LIST; if (Opt_T) printf("<%s: %s>\n", #t, i);}
#define tokenString(t, s)  {if (Opt_T) printf("<%s: %s>\n", #t, s);}
#define tokenOther(t, s)  {LIST;if (Opt_T) printf("<%s: %s>\n", #t, s);}
#define MAX_LINE_LENG      256

int Opt_S = 1;
int Opt_T = 1;
int linenum = 1;
char buf[MAX_LINE_LENG];
%}


digit [0-9]
letter [a-zA-Z]
float [0-9]+\.[0-9]*

%Start COMMENT1
%%

<INITIAL>"/*".* {LIST;BEGIN COMMENT1;}
<COMMENT1>.  {LIST;}
<COMMENT1>.*"*/" {LIST;BEGIN INITIAL;}


<INITIAL>"//&S+".* {Opt_S=1;LIST;}
<INITIAL>"//&S-".* {Opt_S=0;LIST;}
<INITIAL>"//&T+".* {Opt_T=1;LIST;}
<INITIAL>"//&T-".* {Opt_T=0;LIST;}

<INITIAL>"//".* {LIST;}

\n      {
    LIST;
    if (Opt_S)
        printf("%d: %s", linenum, buf);
    linenum++;
    buf[0] = '\0';
}
<INITIAL>[\t ]   {LIST;}



<INITIAL>\"([^"]|\"{2})*\"  {
LIST;
int len=yyleng;
char res[len];
int i=0,j=0;
while(i<len){
    if(yytext[i]=='\"' && yytext[i+1] == '\"'){
        i++;
    }
    res[j++]=yytext[i++];
}
res[j]=0;
// get rid of the " in the beginning and end
res[strlen(res)-1]='\0';
char *out=&res[1];

tokenString(string,out);
}


<INITIAL>"("     {tokenChar('(');}
<INITIAL>")"     {tokenChar(')');}
<INITIAL>"["     {tokenChar('[');}
<INITIAL>"]"     {tokenChar(']');}
<INITIAL>","     {tokenChar(',');}
<INITIAL>";"     {tokenChar(';');}
<INITIAL>":"     {tokenChar(':');}

<INITIAL>"+"     {token(+);}
<INITIAL>"-"     {token(-);}
<INITIAL>"*"     {token(*);}
<INITIAL>"/"     {token(/);}
<INITIAL>"mod"     {token(mod);}
<INITIAL>":="     {token(:=);}
<INITIAL>"and"     {token(and);}
<INITIAL>"not"     {token(not);}
<INITIAL>"<"     {token(<);}
<INITIAL>"<="     {token(<=);}
<INITIAL>"<>"     {token(<>);}
<INITIAL>">="     {token(>=);}
<INITIAL>">"     {token(>);}
<INITIAL>"="     {token(=);}

<INITIAL>"array" {token(KWarray)}
<INITIAL>"begin" {token(KWbegin)}
<INITIAL>"boolean" {token(KWboolean)}
<INITIAL>"def" {token(KWdef)}
<INITIAL>"do" {token(KWdo)}
<INITIAL>"else" {token(KWelse)}
<INITIAL>"end" {token(KWend)}
<INITIAL>"false" {token(KWfalse)}
<INITIAL>"for" {token(KWfor)}
<INITIAL>"integer" {token(KWinteger)}
<INITIAL>"if" {token(KWif)}
<INITIAL>"of" {token(KWof)}
<INITIAL>"print" {token(KWprint)}
<INITIAL>"read" {token(KWread)}
<INITIAL>"real" {token(KWreal)}
<INITIAL>"string" {token(KWstring)}
<INITIAL>"then" {token(KWthen)}
<INITIAL>"to" {token(KWto)}
<INITIAL>"true" {token(KWtrue)}
<INITIAL>"return" {token(KWreturn)}
<INITIAL>"var" {token(KWvar)}
<INITIAL>"while" {token(KWwhile)}
<INITIAL>[+-]?0|([1-9][0-9]*) {tokenInteger(integer,atoi(yytext));}
<INITIAL>[+-]?0[0-7]+ {tokenOctInteger(oct_integer,yytext);}

<INITIAL>{letter}({letter}|{digit})* {tokenOther(id,yytext);}
<INITIAL>[+-]?{digit}+\.{digit}* {tokenOther(float,yytext)}
<INITIAL>[+-]?({float}|{digit}*)?[eE][+-]?{digit} {tokenOther(scientific,yytext)}


%%
int main( int argc, char **argv )
{
  if( argc != 2 ) {
    fprintf( stderr, "Usage: ./scanner [filename]\n" );
    exit(0);
  }

  FILE *fp = fopen( argv[1], "r" );
  if( fp == NULL ) {
    fprintf( stderr, "Open file error\n" );
    exit(-1);
  }

  yyin = fp;

  yylex();

  exit(0);
}
