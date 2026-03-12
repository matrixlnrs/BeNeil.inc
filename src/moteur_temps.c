#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "moteur_temps.h" // N'oublie pas d'inclure ton .h !

// 1. DÉFINITION DES VARIABLES GLOBALES (Allouées ici en mémoire)
Horloge horloge_jeu;
Evenement* tete_echeancier = NULL; // La file d'attente est vide au début

// 2. INITIALISATION DE L'HORLOGE
void init_horloge() {
    horloge_jeu.jour = 1;
    horloge_jeu.heure = 8;
    horloge_jeu.minute = 0;
    // temps_absolu est hyper pratique pour comparer deux dates facilement (tout en minutes)
    horloge_jeu.temps_absolu = (horloge_jeu.jour - 1) * 24 * 60 + horloge_jeu.heure * 60; 
}

// 3. AJOUTER UN ÉVÉNEMENT (Insertion triée dans la liste chaînée)
void ajouter_evenement(int temps_absolu_declenchement, int id_action, const char* desc) {
    // Allocation dynamique de la mémoire pour le nouvel événement (Exigence ISEN validée)
    Evenement* nouvel_ev = (Evenement*)malloc(sizeof(Evenement));
    if (nouvel_ev == NULL) {
        printf("ERREUR CRITIQUE : Plus de memoire disponible !\n");
        exit(1);
    }

    // Remplissage des données de l'événement
    nouvel_ev->declenchement_absolu = temps_absolu_declenchement;
    nouvel_ev->type_action = id_action;
    strncpy(nouvel_ev->description, desc, 127);
    nouvel_ev->description[127] = '\0'; // Sécurité pour les chaînes en C
    nouvel_ev->suivant = NULL;

    // Logique d'insertion : L'échéancier doit toujours être trié chronologiquement !
    // Cas A : La liste est vide OU le nouvel événement arrive AVANT le premier de la liste
    if (tete_echeancier == NULL || tete_echeancier->declenchement_absolu > temps_absolu_declenchement) {
        nouvel_ev->suivant = tete_echeancier;
        tete_echeancier = nouvel_ev;
        return;
    }

    // Cas B : On parcourt la liste avec un pointeur pour trouver sa place
    Evenement* courant = tete_echeancier;
    while (courant->suivant != NULL && courant->suivant->declenchement_absolu <= temps_absolu_declenchement) {
        courant = courant->suivant;
    }
    
    // On insère entre 'courant' et 'courant->suivant'
    nouvel_ev->suivant = courant->suivant;
    courant->suivant = nouvel_ev;
}

// 4. EXÉCUTER L'ÉVÉNEMENT (Retrait de la liste et libération mémoire)
void executer_prochain_evenement(Neil* n) {
    if (tete_echeancier == NULL) return; // Rien à faire

    // On "dépile" le premier événement
    Evenement* ev_a_traiter = tete_echeancier;
    tete_echeancier = ev_a_traiter->suivant; // La tête recule d'un cran

    // --- ICI ON APPLIQUE L'EFFET DE L'ÉVÉNEMENT SUR NEIL ---
    // (Pour l'instant un simple printf, on liera ça à tes actions plus tard)
    printf("\n>>> EVENEMENT ATTEINT : %s <<<\n", ev_a_traiter->description);
    
    // Si c'est un événement négatif (ex: id = -1 pour Interro Surprise)
    if (ev_a_traiter->type_action == -1) {
        n->sante_mentale -= 20;
        printf("-> Neil perd 20 points de sante mentale !\n");
    }

    // Libération de la mémoire dynamique (Très important pour ne pas avoir de fuite mémoire !)
    free(ev_a_traiter);
}

// 5. FONCTION UTILITAIRE : Fait avancer l'horloge proprement
void avancer_horloge_un_pas(int minutes) {
    horloge_jeu.temps_absolu += minutes;
    horloge_jeu.minute += minutes;
    
    // Gestion des dépassements (60 min = 1h, 24h = 1 jour)
    if (horloge_jeu.minute >= 60) {
        horloge_jeu.heure += horloge_jeu.minute / 60;
        horloge_jeu.minute = horloge_jeu.minute % 60;
    }
    if (horloge_jeu.heure >= 24) {
        horloge_jeu.jour += horloge_jeu.heure / 24;
        horloge_jeu.heure = horloge_jeu.heure % 24;
    }
}

// 6. L'ELLIPSE TEMPORELLE (Le mode "Youtuber's Life")
void fast_forward(Neil* n, int minutes_a_passer, const char* nom_activite) {
    printf("\n>>> DEBUT ELLIPSE : %s (%d minutes) <<<\n", nom_activite, minutes_a_passer);
    
    // On avance par "pas" de 15 minutes pour vérifier souvent l'échéancier
    for (int i = 0; i < minutes_a_passer; i += 15) {
        int pas = (minutes_a_passer - i >= 15) ? 15 : (minutes_a_passer - i);
        
        avancer_horloge_un_pas(pas);
        
        // Simuler la fatigue et la faim qui montent avec le temps
        n->energie -= 1; // Perd 1 d'énergie toutes les 15 min
        n->faim += 2;    // La faim monte plus vite
        
        printf("[Jour %02d - %02d:%02d] ...\n", horloge_jeu.jour, horloge_jeu.heure, horloge_jeu.minute);
        
        // VÉRIFICATION 1 : Y a-t-il un événement prévu à cette heure-ci ?
        // On utilise une boucle while car il peut y avoir plusieurs événements à la même minute
        while (tete_echeancier != NULL && tete_echeancier->declenchement_absolu <= horloge_jeu.temps_absolu) {
            executer_prochain_evenement(n);
        }
        
        // VÉRIFICATION 2 : Interruption critique (Seuils)
        if (n->energie <= 0) {
            printf("\n!!! URGENCE : Neil s'effondre de fatigue !!!\n");
            n->energie = 0; // On bloque à 0
            n->sante_mentale -= 30; // Gros malus pour le burnout
            break; // ON CASSE LA BOUCLE ! L'activité est interrompue.
        }
        
        // Optionnel : un petit usleep(100000) ici pour l'animation texte
    }
    
    printf(">>> FIN DE L'ELLIPSE <<<\n\n");
}