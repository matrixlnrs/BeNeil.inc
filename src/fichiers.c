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
    
    while (fgets(buf, 256, f) && nb < 100) { 
        if (buf[0] == '#' || strlen(buf) < 10) continue;
        
        // On parse EXACTEMENT les 11 colonnes avec l'Energie et la Durée !
        // ID ; DESC ; SANTE ; ENERGIE ; ETUDES ; BONHEUR ; ARGENT ; REQ ; CIBLE ; LIEU ; DUREE
        int lus = sscanf(buf, "%d;%[^;];%d;%d;%d;%d;%f;%d;%d;%d;%d", 
               &tab[nb].id, 
               tab[nb].desc, 
               &tab[nb].m_san, 
               &tab[nb].m_ene,
               &tab[nb].m_etu, 
               &tab[nb].m_bon, 
               &tab[nb].m_arg,   
               &tab[nb].req_am, 
               &tab[nb].cible, 
               (int*)&tab[nb].lieu,
               &tab[nb].duree_minutes); 
               
        // Sécurité : On valide l'action UNIQUEMENT si les 11 variables ont été lues correctement
        if (lus == 11) {
            nb++; // On passe à la case suivante du tableau
        } else {
            // Si une ligne est mal écrite dans actions.txt, elle sera ignorée
            // printf("Avertissement : Ligne %d mal formatee.\n", nb + 1);
        }
    }
    
    fclose(f);
    return nb; 
}

// Les fonctions de sauvegarde binaires (pour valider le cahier des charges)
void sauvegarder_partie(Neil *n) {
    FILE *f = fopen("neil_save.dat", "wb");
    if (f) {
        fwrite(n, sizeof(Neil), 1, f);
        fwrite(&horloge_jeu, sizeof(Horloge), 1, f); 
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