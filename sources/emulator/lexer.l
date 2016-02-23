/*
Copyright (C) 2012  Carla Sauvanaud
Copyright (C) 2012, 2016 Benoît Morgan

This file is part of dumb8-emulator.

dumb8-emulator is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

dumb8-emulator is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with dumb8-emulator.  If not, see <http://www.gnu.org/licenses/>.
*/

%{
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
int line = 1;
%}

%option noinput
%option nounput

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
"#" {return tSHARP;}
{INTEGER} {yylval.entier = atoi(yytext); return tINTEGER;}
\n {line++;}
. {}

%%
