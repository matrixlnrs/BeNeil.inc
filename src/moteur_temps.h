#ifndef MOTEUR_TEMPS_H
#define MOTEUR_TEMPS_H

#include "entites.h"

// Horloge globale du jeu
typedef struct {
    int jour;    // 1 à 30 (ou plus)
    int heure;   // 0 à 23
    int minute;  // 0 à 59
    int temps_absolu; // Pratique pour calculer les durées (en minutes depuis le début)
} Horloge;

// L'événement (Le noeud de notre liste chaînée)
typedef struct Evenement {
    int declenchement_absolu; // A quelle minute exacte ça "pop"
    int type_action;          // ID de l'action ou code spécial (ex: -1 = Interro surprise)
    char description[128];    // Texte pour les logs/UI
    struct Evenement* suivant; // Pointeur vers l'événement suivant (Liste chaînée)
} Evenement;

// Variables globales pour le moteur
extern Horloge horloge_jeu;
extern Evenement* tete_echeancier;

// Prototypes des fonctions clés (qui utiliseront pointeurs et allocation dynamique)
void init_horloge();
void ajouter_evenement(int minute_declenchement, int id_action, const char* desc);
void executer_prochain_evenement(Neil* n);
void fast_forward(Neil* n, int minutes_a_passer, const char* nom_activite, int est_repos);

#endif