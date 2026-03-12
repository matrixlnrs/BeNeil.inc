#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "entites.h"
#include "moteur_temps.h"
#include "ui.h" // Ton module d'interface graphique !

int main() {
    // Initialisation de l'aléatoire (pour les futurs imprévus)
    srand((unsigned int)time(NULL)); 
    
    // 1. INITIALISATION DU JOUEUR
    Neil n;
    
    // Valeurs de départ pour une nouvelle partie
    strcpy(n.nom, "Neil");
    n.sante_mentale = 80;
    n.energie = 100;
    n.faim = 0;
    n.etudes = 20;     // C-SKILLS
    n.bonheur = 50;    // SOCIAL
    n.argent = 250.0;
    n.etat_ngoc = SOLO;
    n.etat_charlotte = SOLO;
    n.suspicion = 0;
    n.lieu_actuel = APPART;
    
    // Démarre l'horloge au Jour 1, 08h00
    init_horloge(); 
    
    // 2. LA BOUCLE PRINCIPALE DU JEU
    // Le jeu tourne tant qu'on n'a pas atteint la fin du mois et que N est en vie/à l'école
    while (horloge_jeu.jour <= 30 && n.sante_mentale > 0 && n.etudes > 0) {
        
        // Calcul du jour de la semaine (1 = Lundi, 7 = Dimanche)
        int jour_semaine = (horloge_jeu.jour - 1) % 7 + 1;
        
        // --- BLOCS OBLIGATOIRES (L'ellipse temporelle) ---
        // Du Lundi au Vendredi, à 8h du matin, Neil n'a pas le choix
        if (jour_semaine <= 5 && horloge_jeu.heure == 8) {
            if (jour_semaine <= 2) {
                // Lundi et Mardi : ISEN
                n.lieu_actuel = ISEN;
                // Le '0' à la fin indique que ce n'est PAS du repos (l'énergie va baisser)
                fast_forward(&n, 8 * 60, "Cours a l'ISEN", 0); 
            } else {
                // Mercredi, Jeudi, Vendredi : Entreprise
                n.lieu_actuel = ENTREPRISE;
                fast_forward(&n, 8 * 60, "Alternance au bureau", 0);
            }
            continue; // On repart au début de la boucle while, il sera désormais 16h
        }
        
        // --- TEMPS LIBRE (Le joueur a la main) ---
        
        // 1. On affiche la belle interface avec les jauges
        afficher_ui(&n);
        
        int tw = get_terminal_width();
        
        // 2. On affiche le menu (Pour l'instant statique, bientôt lié à actions.txt)
        print_padding(tw, 40); 
        printf(BOLD UNDERLINE "ACTIONS DISPONIBLES :" RESET "\n\n");
        
        print_padding(tw, 62); 
        printf(" 1. Reviser le C (Duree: 2h)\n");
        
        print_padding(tw, 62); 
        printf(" 2. Aller au 809 (Duree: 3h)\n");
        
        print_padding(tw, 62); 
        printf(" 3. Dormir (Avance jusqu'a 8h le lendemain)\n");
        
        print_padding(tw, 40); 
        printf("\nChoix > ");
        
        // 3. Lecture du choix du joueur
        int choix;
        if (scanf("%d", &choix) != 1) {
            // Sécurité basique si le joueur tape une lettre au lieu d'un chiffre
            while(getchar() != '\n'); 
            continue;
        }
        
        // 4. Résolution de l'action choisie
        if (choix == 1) {
            n.etudes += 10;
            if(n.etudes > 100) n.etudes = 100;
            fast_forward(&n, 120, "Revisions acharnees", 0);
        } 
        else if (choix == 2) {
            n.bonheur += 20; 
            if(n.bonheur > 100) n.bonheur = 100;
            n.argent -= 30; 
            n.lieu_actuel = FOYER809;
            fast_forward(&n, 180, "Grosse soiree au 809", 0);
        } 
        else if (choix == 3) {
            n.lieu_actuel = APPART;
            // Calcul savant pour savoir combien de temps dormir jusqu'à 8h demain
            int minutes_restantes_aujourdhui = (24 - horloge_jeu.heure) * 60 - horloge_jeu.minute;
            int minutes_sommeil = minutes_restantes_aujourdhui + (8 * 60);
            
            // Le '1' à la fin indique à fast_forward que c'est du repos ! 
            // L'énergie va remonter à 100% sans déclencher de game over.
            fast_forward(&n, minutes_sommeil, "Sommeil reparateur", 1);
        }
    }
    
    // 3. FIN DE PARTIE (Sortie de la boucle while)
    int tw = get_terminal_width();
    printf("\n");
    if (n.sante_mentale <= 0) {
        animation_flash(3);
        taper_texte("SYSTEM FAILURE : Neil a fait un burnout total.", RED BOLD, tw);
    } else if (n.etudes <= 0) {
        animation_flash(3);
        taper_texte("GAME OVER : Renvoi definitif de l'ISEN.", RED BOLD, tw);
    } else {
        taper_texte("FELICITATIONS : Neil a survecu au mois d'alternance !", GREEN BOLD, tw);
    }

    return 0;
}