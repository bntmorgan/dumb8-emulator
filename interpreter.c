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

// On met ebp au dernier index de la mémoire et esp au dernier index de la mémoire + 1 (pile vide)
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
  // Push de la valeur op1
  memory[regs[REG_ESP]-1] = get_parameter_value(op1);
  // Tête de pile à la prochaine position
  regs[REG_ESP]--;
}

void ipop(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  // Affectation de la zone mémoire op1 avec la valeur en tête de pile
  *(get_parameter_address(op1)) = memory[regs[REG_ESP]];
  // Pop de la tête de pile
  regs[REG_ESP]++;
}

void ical(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  // Push de l'adresse de retour
  struct parameter ret;
  ret.type = PARAM_ADDRESS;
  //On stocke compteur_exe et pas compteur_exe+1
  //(voir incrementation dans exe())
  ret.address.adr = compteur_exe;
  ipsh(&ret,NULL,NULL);
  
  // Saut a l'adresse de la fonction appelee
  ijmp(op1,NULL,NULL);

}

/**
 * iret() est equivalente a l'execution de l'instuction leave suivi de l'instruction ret d'intel
 */
void iret(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  struct parameter param_ebp;
  param_ebp.type = PARAM_REG;
  param_ebp.reg.reg = REG_EBP;
	
  struct parameter param_esp;
  param_esp.type = PARAM_REG;
  param_esp.reg.reg = REG_ESP;
  
  // Affectation de ebp a esp
  icop(&param_esp, &param_ebp, NULL);

  // Depile ebp
  ipop(&param_ebp,NULL,NULL);

  struct parameter param_eax;
  param_eax.type = PARAM_REG;
  param_eax.reg.reg = REG_EAX;
  // Pop de la tête de pile
  ipop(&param_eax, NULL, NULL);
	
  struct parameter ret;
  ret.type = PARAM_ADDRESS;
  ret.address.adr = get_reg_value(REG_EAX);

  ijmp(&ret,NULL,NULL);
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
