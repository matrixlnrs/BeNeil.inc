#ifndef BENEIL_H
#define BENEIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_ACT       60
#define JOURS_MAX     30
#define FICHIER_SAVE  "neil_save.dat"
#define FICHIER_LOG   "journal_neil.txt"

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

typedef enum { SOLO, DRAGUE, COUPLE, PIRATE } StatutAmour;
typedef enum { PARTOUT, BU, KFET, APPART, RUE } Lieu;

typedef struct {
    char nom[32];
    int sante;
    int etudes;
    int bonheur;
    float argent;
    int jour;
    StatutAmour etat_ngoc;
    StatutAmour etat_charlotte;
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

void initialiser_partie(Neil *n);
void afficher_ui(Neil n);
void barre_progression(int val, char* label, char* col);
int lire_entier(int min, int max, const char* msg);

void check_collision(Neil *n);
void executer_evenement_angoctoire(Neil *n);
void ecrire_journal(const char* message);

void sauvegarder_partie(Neil n);
int charger_partie(Neil *n);
int charger_actions(Action tab[]);

#endif