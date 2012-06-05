#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#define MAX_INSTRUCTIONS 1024
#define MEMORY_SIZE 1024
#define REGS_SIZE 2
#define STEPPER_MESSAGE "Continue y / n / a / s / f ? "

// Maintenant qu'on doit gérer les adresses et les registres
// On doit différencier les paramètres d'un instruction

enum {
  PARAM_VALUE,
  PARAM_ADDRESS,
  PARAM_MEMORY_REG,
  PARAM_REG
};

// Valeur entiere sur 4 octets
struct value {
  int val;
};

// Adresse absolue
struct address {
  int adr;
};

// Adresse stockee dans un registre décalée de n
struct memory_reg {
  int reg;
  int n;
};

// Le registre lui même
struct reg {
  int reg;
};

// La structure paramètre union de tous les types
struct parameter {
  int type;
  union {
    struct value value;
    struct address address;
    struct memory_reg memory_reg;
    struct reg reg;
  };
};

// Définition d'une instruction
struct instruction {
       void (*fun) (struct parameter *, struct parameter *, struct parameter *);
       struct parameter op1;
       struct parameter op2;
       struct parameter op3;
};

// Registres
enum {
     REG_EBP = 0,
     REG_ESP = 1,
     REG_EAX = 2,
     REG_EBX = 3,
};

void iadd(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void imul(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void isou(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void idiv(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void icop(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void iafc(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void ijmp(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void ijmf(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void iinf(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void isup(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void iequ(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void ipri(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void ipsh(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void ipop(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void ical(struct parameter *op1, struct parameter *op2, struct parameter *op3);
void iret(struct parameter *op1, struct parameter *op2, struct parameter *op3);

void set_ins(void (*fun) (struct parameter *, struct parameter *, struct parameter *), struct parameter *op1, struct parameter *op2, struct parameter *op3);

int get_reg_value(int reg);
int *get_reg_address(int reg);
void set_reg_value(int reg, int value);

/**
 * Récupère un pointeur vers la zone mémoire représentée par le paramètre
 * Incompatible avec PARAM_VALUE :)
 */
int *get_parameter_address(struct parameter *op);
/**
 * Récupère la valeur représentée par le paramètre
 */
int get_parameter_value(struct parameter *op);

/**
 * Exécute le programme
 */
void exe();

/**
 * Demande a l'utilisateur la confirmation pour exécuter l'instruction suivante
 * @return 0 Si on doit arrêter ou 1 pour continuer
 */
int user_next_step();

/**
 * Verbose une instruction
 */
void debug_instruction(const char *format, ...);

/**
 * Affiche un message préfixé de l'adresse de l'instruction
 */
void display_instruction(const char *format, ...);

/**
 * Dumps the stack from the base to the top (esp)
 */
void stack_dump();

#endif//__INTERPRETER_H__
