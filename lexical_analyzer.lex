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
ebp {yylval.entier = REG_EBP; return tEBP;}
esp {yylval.entier = REG_ESP; return tESP;}
"[" {return tHOOKO;}
"]" {return tHOOKC;}
"+" {return tPLUS;}
"-" {return tMINUS;}
{INTEGER} {yylval.entier = atoi(yytext); return tINTEGER;}
\n {}
. {}

%%
