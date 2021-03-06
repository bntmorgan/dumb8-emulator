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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "options.h"
#include "interpreter.h"
#include "parser.h"

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

void display_instruction(const char *format, ...) {
  printf("[%4d] ", compteur_exe);
  va_list args;
  va_start (args, format);
  vprintf(format, args);
  va_end(args);
}

int user_next_step() {
  char c = 0;
  int nok = 1;
  int ret = 0;
  term_mode_raw();
  display_instruction(STEPPER_MESSAGE);
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
    } else if (c == 's') {
      stack_dump(0);
      display_instruction(STEPPER_MESSAGE);
    } else if (c == 'f') {
      stack_dump(1);
      display_instruction(STEPPER_MESSAGE);
    } else if (c == EOF) {
      printf("EOF");
      ret = 0;
      nok = 0;
    } else {
      printf("\n");
      display_instruction(STEPPER_MESSAGE);
    }
  }
  printf("\n");
  term_mode_restore();
  return ret;
}

void exe(){
  // On commence à exécuter la fin du fichier (le saut vers le main)
  compteur_exe = compteur - 1;
  // Tant que l'on a des instructions
  while (programme[compteur_exe].fun != NULL) {
    // On teste si on est sur un breakpoint
    if (mode_breakpoint && breakpoint_stop(compteur_exe)) {
      display_instruction("*\n");
      mode_stepper = 1;
    }
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
    verbose_instruction("JMP %d\n", op1->address.adr);
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
  verbose_instruction("CAL\n");
  // Push de l'adresse de retour
  struct parameter ret;
  ret.type = PARAM_VALUE;
  // On stocke compteur_exe et pas compteur_exe+1
  // (voir incrementation dans exe())
  // Comme jmp jumpe a adr - 1 il faut bien le + 1
  ret.value.val = compteur_exe + 1;
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
  verbose_instruction("COP %p %d\n", get_reg_address(REG_ESP), get_reg_value(REG_EBP));
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
 
  // Récupération de la valeur de retour
  int val = get_reg_value(REG_ESP);
  
  // Libération de la pile du nombre de paramètres de la fonction
  set_reg_value(REG_ESP, get_reg_value(REG_ESP)+get_parameter_value(op1));

  // On a libéré les paramètres, on recopie la valeur retour en tête de pile
  set_reg_value(REG_ESP, val);

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

void stack_dump(int frame) {
  int stack_bottom;
  if (frame) {
    stack_bottom = get_reg_value(REG_EBP);
  } else {
    stack_bottom = MEMORY_SIZE;
  }
  int stack_top = get_reg_value(REG_ESP);
  // Dumps the stack
  printf("\n---- stack ----\n");
  for (; stack_bottom >= stack_top && stack_bottom >= 0; --stack_bottom) {
    printf("%4x | %08x\n", stack_bottom, memory[stack_bottom]);
  }
  printf("-----------------\n");
}
