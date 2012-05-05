#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include "options.h"

typedef void (*sighandler_t)(int);

// Sauvegarde de l'entrée standard
int stdin_fd = 0;

// Flag mode stepper
int mode_stepper = 0;
// Flag mode verbose
int mode_verbose = 0;
// Flag mode debug
int mode_breakpoint = 0;

// Entrée standard bufferisée
FILE *stdin_terminal = NULL;

// Breakpoints
int breakpoints[OPTIONS_BREAKPOINTS];
// Nombre de breakpoints
int nb_breakpoints = 0;

// Sauvegarde des attributs du terminal
struct termios orig_term_attr;   
struct termios new_term_attr;

void sig_handler(int signum) {
  switch (signum) {
  case SIGINT:
    term_mode_restore();
    break;
  }
}

void term_init() {
  // Sauvegarde des aciens paramètres
  tcgetattr(stdin_fd, &orig_term_attr);
  // Configuration du mode raw
  memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
  new_term_attr.c_lflag &= ~(ECHO | ICANON);
  new_term_attr.c_cc[VTIME] = 0;
  new_term_attr.c_cc[VMIN] = 1;
  
  sighandler_t sh;
  // On restore les attributs du terminal quand l'application l'aisse la main ou quitte
  sh = signal(SIGINT, sig_handler);
  if (sh == SIG_ERR) {
    perror("Error while setting SIGINT signal handler");
    exit(1);
  }
}

void term_mode_raw() {
  // Terminal en mode raw
  tcsetattr(stdin_fd, TCSANOW, &new_term_attr);  
}

void term_mode_restore() {
  // On restaure les attributs du terminal
  tcsetattr(stdin_fd, TCSANOW, &orig_term_attr);
}

static int cmpint(const void *p1, const void *p2) {
  return *((int*)p1) > *((int*)p2);
}

void breakpoint_add(int bp) {
  if (nb_breakpoints < OPTIONS_BREAKPOINTS) {
    breakpoints[nb_breakpoints] = atoi(optarg);
    nb_breakpoints++;
    // Tri du tableau
    qsort(&breakpoints, nb_breakpoints, sizeof(int), cmpint);
  }
}

int breakpoint_stop(int line) {
  int i;
  for (i = 0; i < nb_breakpoints; i++) {
    if (breakpoints[i] > line) {
      break;
    } else if (breakpoints[i] == line) {
      return 1;
    }
  }
  return 0;
}

void do_options(int argc, char **argv) {
  int c;

  while ((c = getopt (argc, argv, "svb:")) != -1) {
    switch (c) {
    case 'b':
      breakpoint_add(atoi(optarg));
      mode_breakpoint = 1;
      break;
    case 's':
      mode_stepper = 1;
      break;
    case 'v':
      mode_verbose = 1;
      break;
    case '?':
      if (optopt == 'o') {
        fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      } else if (isprint (optopt)) {
        fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      } else {
        fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
      }
      exit(1);
    default:
      abort ();
    }
  }

  // Gestion du fichier d'entrée à compiler
  if (optind < argc) {
    // On sauvegarde l'entrée standard
    stdin_fd = dup(STDIN_FILENO);
    // Réouverture de l'entrée standard bufferisée
    stdin_terminal = fdopen(stdin_fd, "r");
    if (stdin == NULL) {
      perror("Error while reopenning stdin");
      exit(1);
    }
    // On ferme le vieux descripteur de fichier
    close(STDIN_FILENO);
    // On ouvre le nouveau fichier qui prendra STDIN_FILENO en descripteur
    if (open(argv[optind], O_RDONLY) == -1) {
      perror("Error while openning file to compile");
      exit(1);
    }
    if (mode_stepper || mode_breakpoint) {
      term_init();
    }
  } else {
    fprintf(stderr, "Cannot compile no file\n");
    exit(1);
  }
}

void close_files() {
  // Fermeture du fichier redirigé sur l'entrée standard
  close(STDIN_FILENO);
  // Récupération de l'entrée standard
  dup(stdin_fd);
  // Libération du descripteur de sauvegarde de l'entrée standard
  close(stdin_fd);
}
