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
ebp {return tEBP;}
esp {return tESP;}
"[" {return tHOOKO;}
"]" {return tHOOKC;}
"+" {return tPLUS;}
"-" {return tMINUS;}
{INTEGER} {yylval = atoi(yytext); return tINTEGER;}
\n {}
. {}

%%
