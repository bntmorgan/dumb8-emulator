%{
#include <stdlib.h>
#include <stdio.h>

#define MAX_INSTRUCTIONS 1024
#define MEMORY_SIZE 1024
#define REGISTERS_SIZE 2

void yyerror(char *s);

//compteur des intructions du programme
int compteur = 0;
int compteur_exe = 0;

struct instruction {
       void (*fun) (int, int, int);
       int op1;
       int op2;
       int op3;
};

enum {
     REG_EBP = 0,
     REG_ESP = 1
};

struct instruction programme[MAX_INSTRUCTIONS] = {};
int memory[MEMORY_SIZE] = {};

int registers[REGISTERS_SIZE] = {};

void iadd(int op1, int op2, int op3);
void imul(int op1, int op2, int op3);
void isou(int op1, int op2, int op3);
void idiv(int op1, int op2, int op3);
void icop(int op1, int op2, int op3);
void iafc(int op1, int op2, int op3);
void ijmp(int op1, int op2, int op3);
void ijmf(int op1, int op2, int op3);
void iinf(int op1, int op2, int op3);
void isup(int op1, int op2, int op3);
void iequ(int op1, int op2, int op3);
void ipri(int op1, int op2, int op3);
void set_ins(void (*fun) (int, int, int), int op1, int op2, int op3);

int reg_value(int reg);

%}

%token tADD tMUL tSOU tDIV tCOP tAFC tJMP tJMF tINF tSUP tEQU tPRI tEBP tESP tHOOKO tHOOKC tPLUS tMINUS tINTEGER

%start instructions

%%

instructions : instruction instructions {}
	     |
	     ;

address : tINTEGER {$$ = $1;}
        | tHOOKO register tHOOKC tPLUS tINTEGER {$$ = reg_value($2) + $5;}
        | tHOOKO register tHOOKC tMINUS tINTEGER {$$ = reg_value($2) + $5;}
	| tHOOKO register tHOOKC {$$ = reg_value($2);}
        ;

register : tEBP {$$ = $1;}
         | tESP {$$ = $1;}
	 ;

instruction : tADD address address address {
  	      	   set_ins(iadd, $2, $3, $4);
	    }
	    | tMUL address address address {
	      	   set_ins(imul, $2, $3, $4);
	    }
	    | tSOU address address address {
	      	   set_ins(isou, $2, $3, $4);
	    }
	    | tDIV address address address {
	      	   set_ins(idiv, $2, $3, $4);
	    }
	    | tCOP address address {
	      	   set_ins(icop, $2, $3, 0);
	    }
	    | tAFC address tINTEGER {
	      	   set_ins(iafc, $2, $3, 0);
	    }
	    | tJMP address {
	      	   set_ins(ijmp, $2, 0, 0);
	    }
	    | tJMF address address {
	      	   set_ins(ijmf, $2, $3, 0);
	    }
	    | tINF address address address {
	      	   set_ins(iinf, $2, $3, $4);
	    }
	    | tSUP address address address {
	      	   set_ins(isup, $2, $3, $4);
	    }
	    | tEQU address address address {
	      	   set_ins(iequ, $2, $3, $4);
	    }
	    | tPRI address {
	      	   set_ins(ipri, $2, 0, 0);
	    }
	    ;

%%

void yyerror(char *s){
     fprintf(stderr, "Vous ne maîtrisez pas les concepts : %s\n", s);
}

void exe(){
     compteur_exe = 0;
     //tant que l'on a des instructions
     while(programme[compteur_exe].fun != NULL) {
          //execution de l'instruction
	  programme[compteur_exe].fun(
		programme[compteur_exe].op1,
		programme[compteur_exe].op2,
		programme[compteur_exe].op3
	  );
          //instruction suivante
          compteur_exe++;
     }
     return;
}

void iadd(int op1, int op2, int op3) {
     memory[op1] = memory[op2] + memory[op3];
}

void imul(int op1, int op2, int op3) {
     memory[op1] = memory[op2] * memory[op3];
}

void isou(int op1, int op2, int op3) {
     memory[op1] = memory[op2] - memory[op3];
}

void idiv(int op1, int op2, int op3) {
     memory[op1] = memory[op2] / memory[op3];
}

void icop(int op1, int op2, int op3) {
     memory[op1] = memory[op2];
}

void iafc(int op1, int op2, int op3) {
     memory[op1] = op2;
}

void ijmp(int op1, int op2, int op3) {
     compteur_exe = op1 - 1; // a cause du compteur_exe++
}

void ijmf(int op1, int op2, int op3) {
     if(!memory[op1]) {
         ijmp(op2, 0, 0);
     }
}

void iinf(int op1, int op2, int op3) {
     memory[op1] = (memory[op2] < memory[op3]);
}

void isup(int op1, int op2, int op3) {
     memory[op1] = (memory[op2] > memory[op3]);
}

void iequ(int op1, int op2, int op3) {
     memory[op1] = (memory[op2] == memory[op3]);
}

void ipri(int op1, int op2, int op3) {
     printf("%d\n", memory[op1]);
}

void set_ins(void (*fun) (int, int, int), int op1, int op2, int op3) {
     struct instruction i;
     i.fun = fun;
     i.op1 = op1;
     i.op2 = op2;
     i.op3 = op3;
     programme[compteur] = i;
     compteur++;
}

int reg_value(int reg) {
    switch(reg) {
    case tEBP : 
    	 return registers[REG_EBP];
    case tESP : 
    	 return registers[REG_ESP];
    default :
    	 return 0;
    }
}

int main(int argc, char **argv) {
    yyparse();
    exe();
    return 0;  
}
