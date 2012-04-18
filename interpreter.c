#include <stdlib.h>
#include <stdio.h>
#include "interpreter.h"
#include "syntaxic_analyzer.h"

// Compteur des intructions du programme
int compteur = 0;
int compteur_exe = 0;

// Memoire du programme a exécuter
struct instruction programme[MAX_INSTRUCTIONS] = {};
// Pile
int memory[MEMORY_SIZE] = {};

// On met ebp et esp au dernier index de la mémoire + 1 (pile vide)
int regs[REGS_SIZE] = {MEMORY_SIZE, MEMORY_SIZE};

void exe(){
  compteur_exe = 0;
  // Tant que l'on a des instructions
  while(programme[compteur_exe].fun != NULL) {
    //execution de l'instruction
    programme[compteur_exe].fun(
				&(programme[compteur_exe].op1),
				&(programme[compteur_exe].op2),
				&(programme[compteur_exe].op3)
				);
    // Instruction suivante
    compteur_exe++;
  }
  return;
}

void iadd(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  *(get_parameter_address(op1)) = get_parameter_value(op2) + get_parameter_value(op3);
}

void imul(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  *(get_parameter_address(op1)) = get_parameter_value(op2) * get_parameter_value(op3);
}

void isou(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  *(get_parameter_address(op1)) = get_parameter_value(op2) - get_parameter_value(op3);
}

void idiv(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  *(get_parameter_address(op1)) = get_parameter_value(op2) / get_parameter_value(op3);
}

void icop(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  *(get_parameter_address(op1)) = get_parameter_value(op2);
}

void iafc(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  *(get_parameter_address(op1)) = get_parameter_value(op2);
}

void ijmp(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  // Traitement spécial pour le jump
  if(op1->type == PARAM_ADDRESS) {
    compteur_exe = op1->address.adr - 1; // A cause du compteur_exe++
  }
}

void ijmf(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  if(!get_parameter_value(op1)) {
    ijmp(op2, 0, 0);
  }
}

void iinf(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  *(get_parameter_address(op1)) = (get_parameter_value(op2) < get_parameter_value(op3));
}

void isup(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  *(get_parameter_address(op1)) = (get_parameter_value(op2) > get_parameter_value(op3));
}

void iequ(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  *(get_parameter_address(op1)) = (get_parameter_value(op2) == get_parameter_value(op3));
}

void ipri(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  printf("%d\n", get_parameter_value(op1));
}


void ipsh(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  // Tête de pile à la prochaine position
  regs[REG_ESP]--;
  // Push de la valeur op1
  memory[regs[REG_ESP]] = get_parameter_value(op1);
}

void ipop(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  // Affectation de la zone mémoire op1 avec la valeur en tête de pile
  *(get_parameter_address(op1)) = memory[regs[REG_ESP]];
  // Pop de la tête de pile
  regs[REG_ESP]++;
}

void set_ins(void (*fun) (struct parameter *, struct parameter *, struct parameter *), struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  struct instruction i;
  i.fun = fun;
  if (op1 != NULL) {
    i.op1 = *op1;
  }
  if (op2 != NULL) {
    i.op2 = *op2;
  }
  if (op3 != NULL) {
    i.op3 = *op3;
  }
  programme[compteur] = i;
  compteur++;
}

int get_reg_value(int reg) {
  return regs[reg];
}

int *get_reg_address(int reg) {
  return &(regs[reg]);
}

void set_reg_value(int reg, int value) {
    regs[reg] = value;
}

int *get_parameter_address(struct parameter *op) {
  switch (op->type) {
  case PARAM_VALUE : 
    fprintf(stderr, "Erreur sémantique : une valeur entière n'est pas une adress vers une zone mémoire\n");
    exit(1);
  case PARAM_ADDRESS :
    return &(memory[op->address.adr]);
  case PARAM_MEMORY_REG :
    return &(memory[get_reg_value(op->memory_reg.reg) + op->memory_reg.n]);
  case PARAM_REG : 
    return get_reg_address(op->reg.reg);
  default :
    fprintf(stderr, "Erreur sémantique : paramètre d'instruction non reconnu\n");
    exit(1);
  }
  return NULL;
}

int get_parameter_value(struct parameter *op) {
  switch (op->type) {
  case PARAM_VALUE : 
    return op->value.val;
  case PARAM_ADDRESS :
    return memory[op->address.adr];
  case PARAM_MEMORY_REG :
    return memory[get_reg_value(op->memory_reg.reg) + op->memory_reg.n];
  case PARAM_REG : 
    return get_reg_value(op->reg.reg);
  default :
    fprintf(stderr, "Erreur sémantique : paramètre d'instruction non reconnu\n");
    exit(1);
  }
  return 0;
}
