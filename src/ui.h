#ifndef UI_H
#define UI_H

#include "entites.h"

#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define YELLOW    "\033[33m"
#define CYAN      "\033[36m"
#define MAGENTA   "\033[35m"
#define BOLD      "\033[1m"
#define UNDERLINE "\033[4m"
#define RESET     "\033[0m"

#define B_TL    "╔"
#define B_TR    "╗"
#define B_BL    "╚"
#define B_BR    "╝"
#define B_H     "═"
#define B_V     "║"
#define B_ML    "╠"
#define B_MR    "╣"

int get_terminal_width();
void print_padding(int largeur_term, int largeur_contenu);
void taper_texte(const char* texte, const char* couleur, int tw);
void animation_flash(int iterations);
void barre_master(int val, const char* label, const char* col, int tw);
void afficher_ui(Neil *n);

void afficher_ecran_fin(Neil *n);
#endif