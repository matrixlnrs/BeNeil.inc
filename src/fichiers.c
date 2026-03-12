#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fichiers.h"
#include "moteur_temps.h" // Pour sauvegarder l'horloge plus tard

// Lit le fichier txt et remplit le tableau d'actions
int charger_actions(Action tab[]) {
    FILE *f = fopen("actions.txt", "r");
    if (!f) {
        printf("ERREUR : Impossible d'ouvrir actions.txt !\n");
        return 0;
    }
    
    int nb = 0; 
    char buf[256];
    
    while (fgets(buf, 256, f) && nb < 100) { // 100 actions max
        // On ignore les lignes vides ou les commentaires (qui commencent par #)
        if (buf[0] == '#' || strlen(buf) < 10) continue;
        
        // On parse les données selon ton format : ID;DESC;SANTE;ETUDES;BONHEUR;ARGENT;REQ;CIBLE;LIEU
        sscanf(buf, "%d;%[^;];%d;%d;%d;%f;%d;%d;%d", 
               &tab[nb].id, 
               tab[nb].desc, 
               &tab[nb].m_san, 
               &tab[nb].m_etu, 
               &tab[nb].m_bon, 
               &tab[nb].m_arg, 
               &tab[nb].req_am, 
               &tab[nb].cible, 
               (int*)&tab[nb].lieu);
               
        // ATTENTION : Ton fichier actions.txt n'a pas de durée !
        // Pour l'instant on met une durée par défaut de 2h (120 minutes) pour tout le monde.
        // On affinera ça plus tard.
        tab[nb].duree_minutes = 120; 
               
        nb++;
    }
    fclose(f);
    return nb; // Retourne le nombre d'actions chargées
}

// Les fonctions de sauvegarde binaires (pour valider le cahier des charges)
void sauvegarder_partie(Neil *n) {
    FILE *f = fopen("neil_save.dat", "wb");
    if (f) {
        fwrite(n, sizeof(Neil), 1, f);
        fwrite(&horloge_jeu, sizeof(Horloge), 1, f); // On sauvegarde aussi l'heure !
        fclose(f);
    }
}

int charger_partie(Neil *n) {
    FILE *f = fopen("neil_save.dat", "rb");
    if (f) {
        fread(n, sizeof(Neil), 1, f);
        fread(&horloge_jeu, sizeof(Horloge), 1, f);
        fclose(f);
        return 1;
    }
    return 0;
}