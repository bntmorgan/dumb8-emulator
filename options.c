#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "options.h"

// Sauvegarde de l'entrée standard
int stdin_fd = 0;

void do_options(int argc, char **argv) {
  int c;

  while ((c = getopt (argc, argv, "")) != -1) {
    switch (c) {
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
    // On ferme le vieux descripteur de fichier
    close(STDIN_FILENO);
    // On ouvre le nouveau fichier qui prendra STDIN_FILENO en descripteur
    if (open(argv[optind], O_RDONLY) == -1) {
      perror("Error while openning file to compile");
      exit(1);
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
