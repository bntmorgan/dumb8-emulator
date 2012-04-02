%{
#include <stdlib.h>
#include <stdio.h>

#define MAX_INSTRUCTIONS 1024
#define MEMORY_SIZE 1024

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

struct instruction programme[MAX_INSTRUCTIONS] = {};
int memory[MEMORY_SIZE];

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

%}

%token tADD tMUL tSOU tDIV tCOP tAFC tJMP tJMF tINF tSUP tEQU tPRI tINTEGER

%start instructions

%%

instructions : instruction instructions {}
	     |
	     ;

instruction : tADD tINTEGER tINTEGER tINTEGER {
  	      	   set_ins(iadd, $2, $3, $4);
	    }
	    | tMUL tINTEGER tINTEGER tINTEGER {
	      	   set_ins(imul, $2, $3, $4);
	    }
	    | tSOU tINTEGER tINTEGER tINTEGER {
	      	   set_ins(isou, $2, $3, $4);
	    }
	    | tDIV tINTEGER tINTEGER tINTEGER {
	      	   set_ins(idiv, $2, $3, $4);
	    }
	    | tCOP tINTEGER tINTEGER {
	      	   set_ins(icop, $2, $3, 0);
	    }
	    | tAFC tINTEGER tINTEGER {
	      	   set_ins(iafc, $2, $3, 0);
	    }
	    | tJMP tINTEGER {
	      	   set_ins(ijmp, $2, 0, 0);
	    }
	    | tJMF tINTEGER tINTEGER {
	      	   set_ins(ijmf, $2, $3, 0);
	    }
	    | tINF tINTEGER tINTEGER tINTEGER {
	      	   set_ins(iinf, $2, $3, $4);
	    }
	    | tSUP tINTEGER tINTEGER tINTEGER {
	      	   set_ins(isup, $2, $3, $4);
	    }
	    | tEQU tINTEGER tINTEGER tINTEGER {
	      	   set_ins(iequ, $2, $3, $4);
	    }
	    | tPRI tINTEGER {
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

int main(int argc, char **argv) {
    yyparse();
    exe();
    return 0;  
}
