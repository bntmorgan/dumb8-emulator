%{
#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "options.h"

void yyerror(char *s);

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
           | tHOOKO registers tHOOKC tPLUS tINTEGER {struct parameter p; p.type = PARAM_MEMORY_REG; p.memory_reg.reg = $2; p.memory_reg.n = $5; $$ = p;}
           | tHOOKO registers tHOOKC tMINUS tINTEGER {struct parameter p; p.type = PARAM_MEMORY_REG; p.memory_reg.reg = $2; p.memory_reg.n = -($5); $$ = p;}
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
	    | tCAL tINTEGER {
						 struct parameter p;
		         p.type = PARAM_ADDRESS;
		         p.address.adr = $2;
	      	   set_ins(ical, &(p), NULL, NULL);
	    }
	    | tRET {
	      	   set_ins(iret, NULL, NULL, NULL);
	    }
	    ;

%%

void yyerror(char *s){
  fprintf(stderr, "Vous ne maîtrisez pas les concepts : %s\n", s);
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
