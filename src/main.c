#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "entites.h"
#include "moteur_temps.h"
// #include "ui.h"       // À décommenter quand on aura fait ui.c
// #include "fichiers.h" // À décommenter quand on aura fait fichiers.c

int main() {
    // Initialisation de l'aléatoire pour tes imprévus
    srand((unsigned int)time(NULL)); 
    
    // 1. INITIALISATION DU JEU
    Neil n;
    // (Ici tu pourras mettre ton code de chargement de sauvegarde plus tard)
    
    // Valeurs par défaut pour une nouvelle partie
    strcpy(n.nom, "Neil");
    n.sante_mentale = 80;
    n.energie = 100;
    n.faim = 0;
    n.etudes = 20;
    n.bonheur = 50;
    n.argent = 250.0;
    n.etat_ngoc = SOLO;
    n.etat_charlotte = SOLO;
    n.lieu_actuel = APPART;
    
    init_horloge(); // Met l'horloge au Jour 1, 08h00
    
    printf("=== BIENVENUE DANS BE NEIL.INC ===\n\n");

    // 2. LA BOUCLE PRINCIPALE DU JEU
    // Tant que N n'est pas en burnout et qu'on n'a pas fini le mois
    while (horloge_jeu.jour <= 30 && n.sante_mentale > 0 && n.etudes > 0) {
        
        // Calcul du jour de la semaine (1 = Lundi, 7 = Dimanche)
        int jour_semaine = (horloge_jeu.jour - 1) % 7 + 1;
        
        // --- BLOCS OBLIGATOIRES (L'ellipse temporelle) ---
        // Si on est un jour de semaine ET qu'il est 8h du matin
        if (jour_semaine <= 5 && horloge_jeu.heure == 8) {
            if (jour_semaine <= 2) {
                // Lundi et Mardi : ISEN
                n.lieu_actuel = ISEN;
                fast_forward(&n, 8 * 60, "Cours a l'ISEN");
            } else {
                // Mercredi, Jeudi, Vendredi : Entreprise
                n.lieu_actuel = ENTREPRISE;
                fast_forward(&n, 8 * 60, "Alternance au bureau");
            }
            continue; // On repart au début de la boucle while (il sera 16h)
        }
        
        // --- TEMPS LIBRE (Le joueur a la main) ---
        // (Ici, on appellera afficher_ui(n) pour afficher tes belles barres de stats)
        printf("\n[Jour %02d - %02d:%02d] Temps libre ! Lieu: %d\n", 
               horloge_jeu.jour, horloge_jeu.heure, horloge_jeu.minute, n.lieu_actuel);
        printf("Sante Mentale: %d | Energie: %d | Argent: %.2f\n", n.sante_mentale, n.energie, n.argent);
        
        // MENU PROVISOIRE POUR TESTER LE MOTEUR
        printf("\nQue voulez-vous faire ?\n");
        printf("1. Reviser le C (Duree: 2h)\n");
        printf("2. Aller au 809 (Duree: 3h)\n");
        printf("3. Dormir (Avance jusqu'a 8h le lendemain)\n");
        printf("Choix : ");
        
        int choix;
        scanf("%d", &choix);
        
        // Résolution du choix
        if (choix == 1) {
            n.etudes += 10;
            fast_forward(&n, 120, "Revisions acharnees");
        } 
        else if (choix == 2) {
            n.bonheur += 20;
            n.argent -= 30;
            fast_forward(&n, 180, "Grosse soiree au 809");
        } 
        else if (choix == 3) {
            // Calculer combien de minutes il reste jusqu'à 8h demain
            int minutes_restantes_aujourdhui = (24 - horloge_jeu.heure) * 60 - horloge_jeu.minute;
            int minutes_sommeil = minutes_restantes_aujourdhui + (8 * 60);
            
            n.energie = 100; // Le sommeil restaure l'énergie
            fast_forward(&n, minutes_sommeil, "Sommeil reparateur");
        }
        else {
            printf("Choix invalide.\n");
        }
    }
    
    // 3. FIN DE PARTIE (Game Over ou Victoire)
    if (n.sante_mentale <= 0) {
        printf("\nSYSTEM FAILURE : Neil a fait un burnout total.\n");
    } else if (n.etudes <= 0) {
        printf("\nGAME OVER : Renvoi definitif de l'ISEN.\n");
    } else {
        printf("\nFELICITATIONS : Neil a survecu au mois !\n");
    }

    return 0;
}