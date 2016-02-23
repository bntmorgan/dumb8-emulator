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
#include "interpreter.h"
#include "options.h"
#include "lexer.h"

void yyerror(char *s);

extern int line;

%}

// Declaration des types utilisés
%union {
  int entier;
  struct parameter parameter;
};

%code requires {
  #include "interpreter.h"
 }

%token <entier> tADD tMUL tSOU tDIV tCOP tAFC tJMP tJMF tCAL tRET tINF tSUP tEQU tPRI tPSH tPOP tEBP tESP tEAX tEBX tHOOKO tHOOKC tPLUS tMINUS tINTEGER tSHARP

%type <parameter> expression
%type <entier> registers

%start instructions

%%

instructions : instruction instructions {}
             |
             ;

expression : tINTEGER {struct parameter p; p.type = PARAM_ADDRESS; p.address.adr = $1; $$ = p;}
           | tHOOKO registers tPLUS tINTEGER tHOOKC {struct parameter p; p.type = PARAM_MEMORY_REG; p.memory_reg.reg = $2; p.memory_reg.n = $5; $$ = p;}
           | tHOOKO registers tMINUS tINTEGER tHOOKC {struct parameter p; p.type = PARAM_MEMORY_REG; p.memory_reg.reg = $2; p.memory_reg.n = -($5); $$ = p;}
           | tHOOKO registers tHOOKC {struct parameter p; p.type = PARAM_MEMORY_REG; p.memory_reg.reg = $2; p.memory_reg.n = 0; $$ = p;}
           | registers {struct parameter p; p.type = PARAM_REG; p.reg.reg = $1; $$ = p;}
           ;

registers : tEBP {} //Par defaut $$ = $1
          | tESP {}
          | tEAX {}
          | tEBX {}
          ;

instruction : tADD expression expression expression {
              set_ins(iadd, &($2), &($3), &($4));
            }
            | tMUL expression expression expression {
              set_ins(imul, &($2), &($3), &($4));
            }
            | tSOU expression expression expression {
              set_ins(isou, &($2), &($3), &($4));
            }
            | tDIV expression expression expression {
              set_ins(idiv, &($2), &($3), &($4));
            }
            | tCOP expression expression {
              set_ins(icop, &($2), &($3), NULL);
            }
            | tAFC expression tSHARP tINTEGER {
              struct parameter p;
              p.type = PARAM_VALUE;
              p.value.val = $4;
              set_ins(iafc, &($2), &(p), NULL);
            }
            | tJMP expression {
              set_ins(ijmp, &($2), NULL, NULL);
            }
            | tJMF expression expression {
              set_ins(ijmf, &($2), &($3), NULL);
            }
            | tINF expression expression expression {
              set_ins(iinf, &($2), &($3), &($4));
            }
            | tSUP expression expression expression {
              set_ins(isup, &($2), &($3), &($4));
            }
            | tEQU expression expression expression {
              set_ins(iequ, &($2), &($3), &($4));
            }
            | tPRI expression {
              set_ins(ipri, &($2), NULL, NULL);
            }     
            | tPSH expression {
              set_ins(ipsh, &($2), NULL, NULL);
            }
            | tPOP expression {
              set_ins(ipop, &($2), NULL, NULL);
            }
            | tCAL expression {
              set_ins(ical, &($2), NULL, NULL);
            }
            | tRET tINTEGER{
              struct parameter p;
              p.type = PARAM_VALUE;
              p.value.val = $2;
              set_ins(iret, &(p), NULL, NULL);
            }
            ;

%%

void yyerror(char *s){
  fprintf(stderr, "Vous ne maîtrisez pas les concepts : %s, à la ligne %d\n", s, line);
}

//extern int yydebug;

int main(int argc, char **argv) {
  //yydebug = 1;
  do_options(argc, argv);
  yyparse();
  exe();
  close_files();
  return 0;  
}
