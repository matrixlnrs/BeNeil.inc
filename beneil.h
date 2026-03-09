#ifndef BENEIL_H
#define BENEIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAX_ACT       60
#define JOURS_MAX     30
#define FICHIER_SAVE  "neil_save.dat"

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

typedef enum { SOLO, DRAGUE, COUPLE, PIRATE } StatutAmour;
typedef enum { PARTOUT, CROUS, FOYER809, APPART, RUE } Lieu;

typedef struct {
    char nom[32];
    int sante, etudes, bonheur;
    float argent;
    int jour;
    StatutAmour etat_ngoc, etat_charlotte;
    int suspicion;
    Lieu lieu_actuel;
} Neil;

typedef struct {
    int id;
    char desc[128];
    int m_san, m_etu, m_bon, req_am, cible; 
    float m_arg;
    Lieu lieu;
} Action;

int get_terminal_width();
void print_padding(int largeur_term, int largeur_contenu);
void barre_master(int val, char* label, char* col, int tw);
void afficher_ui(Neil n);
void taper_texte(char* texte, char* couleur, int tw);
void animation_flash(int iterations);
void animation_shake(int intensite);
void cinematique_collision(Neil *n);
void check_collision(Neil *n);
int charger_actions(Action tab[]);
int lire_entier(int min, int max, const char* msg, int tw);
void sauvegarder_partie(Neil n);
int charger_partie(Neil *n);

#endif