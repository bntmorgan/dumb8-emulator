%{
#include <stdlib.h>
#include <stdio.h>
#include "syntaxic_analyzer.h"
%}

INTEGER [0-9]+

%%

ADD {return tADD;}
MUL {return tMUL;}
SOU {return tSOU;}
DIV {return tDIV;}
COP {return tCOP;}
AFC {return tAFC;}
JMP {return tJMP;}
JMF {return tJMF;}
INF {return tINF;}
SUP {return tSUP;}
EQU {return tEQU;}
PRI {return tPRI;}
PSH {return tPSH;}
POP {return tPOP;}
CAL {return tCAL;}
RET {return tRET;}
ebp {yylval.entier = REG_EBP; return tEBP;}
esp {yylval.entier = REG_ESP; return tESP;}
eax {yylval.entier = REG_EAX; return tEAX;}
ebx {yylval.entier = REG_EBX; return tEBX;}
"[" {return tHOOKO;}
"]" {return tHOOKC;}
"+" {return tPLUS;}
"-" {return tMINUS;}
{INTEGER} {yylval.entier = atoi(yytext); return tINTEGER;}
\n {}
. {}

%%
