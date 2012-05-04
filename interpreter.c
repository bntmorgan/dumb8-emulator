#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "options.h"
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

void verbose_instruction(const char *format, ...) {
  if (mode_verbose) {
    printf("[%4d] ", compteur_exe);
    va_list args;
    va_start (args, format);
    vprintf(format, args);
    va_end(args);
  }
}

int user_next_step() {
  char c = 0;
  int nok = 1;
  int ret = 0;
  term_mode_raw();
  printf("[%4d] Continue y / n / a ? ", compteur_exe);
  while (nok) {
    c = fgetc(stdin_terminal);
    if (c == 'y') {
      printf("y");
      ret = 1;
      nok = 0;
    } else if (c == 'n') {
      printf("n");
      ret = 0;
      nok = 0;
    } else if (c == 'a') {
      printf("a");
      ret = 1;
      nok = 0;
      mode_stepper = 0;
    } else {
      printf("\n[%4d] Continue y / n / a ? ", compteur_exe);
    }
  }
  printf("\n");
  term_mode_restore();
  return ret;
}

void exe(){
  compteur_exe = 0;
  // Tant que l'on a des instructions
  while (programme[compteur_exe].fun != NULL) {
    // On teste si on est en mode stepper, si on veut continuer
    if (mode_stepper && !user_next_step()) {
      break;
    }
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
  verbose_instruction("ADD %p %d %d\n", get_parameter_address(op1), get_parameter_value(op2), get_parameter_value(op3));
  *(get_parameter_address(op1)) = get_parameter_value(op2) + get_parameter_value(op3);
}

void imul(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("MUL %p %d %d\n", get_parameter_address(op1), get_parameter_value(op2), get_parameter_value(op3));
    *(get_parameter_address(op1)) = get_parameter_value(op2) * get_parameter_value(op3);
}

void isou(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("SOU %p %d %d\n", get_parameter_address(op1), get_parameter_value(op2), get_parameter_value(op3));
  *(get_parameter_address(op1)) = get_parameter_value(op2) - get_parameter_value(op3);
}

void idiv(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("DIV %p %d %d\n", get_parameter_address(op1), get_parameter_value(op2), get_parameter_value(op3));
  *(get_parameter_address(op1)) = get_parameter_value(op2) / get_parameter_value(op3);
}

void icop(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("COP %p %d\n", get_parameter_address(op1), get_parameter_value(op2));
  *(get_parameter_address(op1)) = get_parameter_value(op2);
}

void iafc(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("AFC %p %d\n", get_parameter_address(op1), get_parameter_value(op2));
  *(get_parameter_address(op1)) = get_parameter_value(op2);
}

void ijmp(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  // Traitement spécial pour le jump
  if(op1->type == PARAM_ADDRESS) {
    verbose_instruction("JMP %d\n", op1->address.adr - 1);
    compteur_exe = op1->address.adr - 1; // A cause du compteur_exe++
  }
}

void ijmf(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  if(!get_parameter_value(op1)) {
    verbose_instruction("JMF false\n");
    ijmp(op2, 0, 0);
  } else {
    verbose_instruction("JMF true\n");
  }
}

void iinf(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("INF %p %d %d\n", get_parameter_address(op1), get_parameter_value(op2), get_parameter_value(op3));
  *(get_parameter_address(op1)) = (get_parameter_value(op2) < get_parameter_value(op3));
}

void isup(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("SUP %p %d %d\n", get_parameter_address(op1), get_parameter_value(op2), get_parameter_value(op3));
  *(get_parameter_address(op1)) = (get_parameter_value(op2) > get_parameter_value(op3));
}

void iequ(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("EQU %p %d %d\n", get_parameter_address(op1), get_parameter_value(op2), get_parameter_value(op3));
  *(get_parameter_address(op1)) = (get_parameter_value(op2) == get_parameter_value(op3));
}

void ipri(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("PRI %d\n", get_parameter_value(op1));
  printf("%d\n", get_parameter_value(op1));
}


void ipsh(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("PSH %d\n", get_parameter_value(op1));
  // Push de la valeur op1
  memory[regs[REG_ESP]-1] = get_parameter_value(op1);
  // Tête de pile à la prochaine position
  regs[REG_ESP]--;
}

void ipop(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("POP %p\n", get_parameter_address(op1));
  // Affectation de la zone mémoire op1 avec la valeur en tête de pile
  *(get_parameter_address(op1)) = memory[regs[REG_ESP]];
  // Pop de la tête de pile
  regs[REG_ESP]++;
}

void ical(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("CAL %d\n", get_parameter_value(op1));
  // Push de l'adresse de retour
  struct parameter ret;
  ret.type = PARAM_VALUE;
  //On stocke compteur_exe et pas compteur_exe+1
  //(voir incrementation dans exe())
  ret.value.val = compteur_exe;
  ipsh(&ret, NULL, NULL);
  
  // Saut a l'adresse de la fonction appelee
  ijmp(op1, NULL, NULL);
}

/**
 * iret() est equivalente a l'execution de l'instuction leave suivi de l'instruction ret d'intel
 */
void iret(struct parameter *op1, struct parameter *op2, struct parameter *op3) {
  verbose_instruction("RET\n");
  // Affectation de ebp a esp
  set_reg_value(REG_ESP, get_reg_value(REG_EBP));
  // Depile ebp
  struct parameter param;
  param.type = PARAM_REG;
  param.reg.reg = REG_EBP;
  ipop(&param, NULL, NULL);
  
  struct parameter ret;
  ret.type = PARAM_ADDRESS;
  // Affectation de l'adresse de retour avec la valeur en tête de pile
  ret.address.adr = memory[get_reg_value(REG_ESP)];
  // Pop de la tête de pile
  (*(get_reg_address(REG_ESP)))++;
  
  ijmp(&ret, NULL, NULL);
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
