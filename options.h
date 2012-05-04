#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#define OPTIONS_BREAKPOINTS 1024

/**
 * Fichier de sortie
 *
 * Si on ne spécifie pas de fichier avec l'option o 
 * a.out.s sera choisi
 */
extern FILE* file_out;

/**
 * Traite les options du compilateur
 *
 * @param argc Nombre d'argument y compris le nom de l'exécutable
 * @param argv Tableau des arguments
 */
void do_options(int argc, char **argv);

/**
 * Fermeture des fichiers et annulation de redirection de l'entrée standard
 */
void close_files();

/**
 * Met le terminal en mode raw
 */
void term_mode_raw();

/**
 * Restaure l'ancien mode du terminal
 */
void term_mode_restore();

/**
 * Initialise correctement la gestion du terminal
 * en terme d'attributs de terminal et de signaux
 */
void term_init();

/**
 * Ajoute un breakpoint
 */
void breakpoint_add(int bp);

/**
 * Regarde si l'adresse courante a un breakpoint
 * @return 1 to stop 0 else
 */
int breakpoint_stop(int line);

// Flags de fonctionnement stepper et verbose
extern int mode_stepper;
extern int mode_verbose;
extern int mode_breakpoint;

extern FILE *stdin_terminal;

#endif//__OPTIONS_H__
