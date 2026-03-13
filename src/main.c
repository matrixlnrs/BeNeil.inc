#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> 
#include "entites.h"
#include "moteur_temps.h"
#include "ui.h"
#include "fichiers.h"

int main() {
    srand((unsigned int)time(NULL)); 
    
    // 1. CHARGEMENT DES ACTIONS
    Action catalogue_actions[100];
    int nb_actions = charger_actions(catalogue_actions);
    
    if (nb_actions == 0) {
        printf("ERREUR CRITIQUE : Impossible de charger actions.txt ou fichier vide.\n");
        return 1;
    }
    
    int tw = get_terminal_width();
    Neil n; 
    
    // ==========================================
    // 2. LE MENU PRINCIPAL
    // ==========================================
    printf("\033[H\033[J\n\n"); 
    print_padding(tw, 44); printf(CYAN B_TL); for(int i=0; i<42; i++) printf(B_H); printf(B_TR "\n");
    print_padding(tw, 44); printf(B_V YELLOW BOLD "           PROJET BE NEIL.INC             " CYAN B_V "\n");
    print_padding(tw, 44); printf(B_BL); for(int i=0; i<42; i++) printf(B_H); printf(B_BR "\n\n" RESET);
    
    print_padding(tw, 30); printf("1. Nouvelle Partie\n");
    print_padding(tw, 30); printf("2. Charger une Partie\n");
    print_padding(tw, 30); printf("3. Quitter\n\n");
    print_padding(tw, 30); printf("Choix > ");
    
    int choix_menu;
    if (scanf("%d", &choix_menu) != 1) return 0;
    
    if (choix_menu == 3) {
        printf("\nAu revoir !\n");
        return 0;
    }
    
    if (choix_menu == 2) {
        if (charger_partie(&n)) {
            printf("\n");
            taper_texte("Sauvegarde trouvee. Reprise de la simulation...", GREEN, tw);
            sleep(1); 
        } else {
            printf("\n");
            taper_texte("Aucune sauvegarde trouvee. Creation d'une nouvelle partie...", RED, tw);
            sleep(2);
            choix_menu = 1; 
        }
    }
    
    if (choix_menu == 1) {
        strcpy(n.nom, "Neil");
        n.sante_mentale = 80;
        n.energie = 100;
        n.faim = 0;
        n.etudes = 20;     
        n.bonheur = 50;    
        n.argent = 250.0;
        n.etat_ngoc = SOLO;
        n.etat_charlotte = SOLO;
        n.suspicion = 0;
        n.lieu_actuel = APPART;
        
        // Initialisation des cooldowns
        n.dernier_jour_mcdo = 0;
        n.dernier_jour_dm_ngoc = 0;
        n.dernier_jour_dm_charlotte = 0;
        
        init_horloge(); 
    }

    // --- ECRAN DE TRANSITION ---
    afficher_ui(&n); 
    
    printf("\n");
    print_padding(tw, 50);
    if (choix_menu == 1) {
        printf(CYAN BOLD "Bienvenue dans l'aventure. Appuyez sur ENTREE pour commencer..." RESET "\n");
    } else {
        printf(CYAN BOLD "De retour aux affaires. Appuyez sur ENTREE pour reprendre..." RESET "\n");
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
    
    // ==========================================
    // 3. BOUCLE PRINCIPALE DU JEU
    // ==========================================
    while (horloge_jeu.jour <= 2 && n.sante_mentale > 0 && n.etudes > 0) {
        
        int jour_semaine = (horloge_jeu.jour - 1) % 7 + 1;
        
        // --- BLOCS OBLIGATOIRES ---
        if (jour_semaine <= 5 && horloge_jeu.heure == 8) {
            if (jour_semaine <= 2) {
                n.lieu_actuel = ISEN;
                n.etudes += 15;        
                n.sante_mentale -= 10; 
                if (n.etudes > 100) n.etudes = 100;
                
                fast_forward(&n, 8 * 60, "Cours a l'ISEN", 0); 
            } else {
                n.lieu_actuel = ENTREPRISE;
                n.argent += 60.0;      
                n.sante_mentale -= 15; 
                
                fast_forward(&n, 8 * 60, "Alternance au bureau", 0);
            }
            continue; 
        }
        
        // --- TEMPS LIBRE ---
        afficher_ui(&n);
        
        print_padding(tw, 40); 
        printf(BOLD UNDERLINE "ACTIONS DISPONIBLES :" RESET "\n\n");
        
        int map_choix[100];
        int nb_choix_affiches = 0;
        
        for (int i = 0; i < nb_actions; i++) {
            Action act = catalogue_actions[i];
            int peut_afficher = 0;
            
            // Conditions de base (Statut Amoureux)
            if (act.cible == 0) peut_afficher = 1;
            else if (act.cible == 1 && (int)n.etat_ngoc >= act.req_am) peut_afficher = 1;
            else if (act.cible == 2 && (int)n.etat_charlotte >= act.req_am) peut_afficher = 1;
            else if (act.cible == 3 && ((int)n.etat_ngoc >= COUPLE && (int)n.etat_charlotte >= DRAGUE)) peut_afficher = 1;
            
            // Condition de lieu
            if (peut_afficher) {
                if (act.lieu != PARTOUT && act.lieu != n.lieu_actuel) {
                    peut_afficher = 0; 
                }
            }

            // --- FILTRES SPECIFIQUES (GAME DESIGN / COOLDOWNS) ---
            if (peut_afficher) {
                // 1. Soirée étudiante (ID 10) : Cachée entre 6h et 21h59
                if (act.id == 10 && (horloge_jeu.heure >= 6 && horloge_jeu.heure < 22)) peut_afficher = 0;
                
                // 2. Actions limitées à 1 fois par jour
                if (act.id == 8 && n.dernier_jour_mcdo == horloge_jeu.jour) peut_afficher = 0;
                if (act.id == 11 && n.dernier_jour_dm_ngoc == horloge_jeu.jour) peut_afficher = 0;
                if (act.id == 21 && n.dernier_jour_dm_charlotte == horloge_jeu.jour) peut_afficher = 0;
            }

            // Affichage final si toutes les conditions sont remplies
            if (peut_afficher) {
                print_padding(tw, 62);
                printf(" %2d. %-38s [%+.0f€]\n", nb_choix_affiches + 1, act.desc, act.m_arg);
                map_choix[nb_choix_affiches] = i; 
                nb_choix_affiches++;
            }
        }
        
        // Options standards
        print_padding(tw, 62); 
        printf(CYAN " %2d. Dormir jusqu'au lendemain matin\n" RESET, nb_choix_affiches + 1);
        print_padding(tw, 62); 
        printf(YELLOW " %2d. Se deplacer (Changer de lieu)\n" RESET, nb_choix_affiches + 2);
        print_padding(tw, 62); 
        printf(MAGENTA " %2d. Sauvegarder et Quitter le jeu\n" RESET, nb_choix_affiches + 3);
        
        print_padding(tw, 40); 
        printf("\nChoix > ");
        
        int choix;
        if (scanf("%d", &choix) != 1) {
            while(getchar() != '\n'); 
            continue;
        }
        
        // --- RESOLUTION DU CHOIX ---
        if (choix > 0 && choix <= nb_choix_affiches) {
            Action action_choisie = catalogue_actions[map_choix[choix - 1]];
            
            // Application des statistiques
            n.sante_mentale += action_choisie.m_san;
            n.energie += action_choisie.m_ene; // ENERGIE INTEGREE ICI !
            n.etudes += action_choisie.m_etu;
            n.bonheur += action_choisie.m_bon;
            n.argent += action_choisie.m_arg;
            
            // Mise à jour des traqueurs journaliers
            if (action_choisie.id == 8)  n.dernier_jour_mcdo = horloge_jeu.jour;
            if (action_choisie.id == 11) n.dernier_jour_dm_ngoc = horloge_jeu.jour;
            if (action_choisie.id == 21) n.dernier_jour_dm_charlotte = horloge_jeu.jour;
            
            // Romance
            if (action_choisie.cible == 1 && (int)n.etat_ngoc == action_choisie.req_am) n.etat_ngoc++;
            if (action_choisie.cible == 2 && (int)n.etat_charlotte == action_choisie.req_am) n.etat_charlotte++;
            
            // Suspicion (Karma)
            if (action_choisie.cible == 1 && n.etat_charlotte >= DRAGUE) n.suspicion += 10; 
            else if (action_choisie.cible == 2 && n.etat_ngoc >= DRAGUE) n.suspicion += 10; 
            else if (action_choisie.cible == 3) n.suspicion += 25;
            
            // Sécurité (Bordures min/max des jauges)
            if (n.sante_mentale > 100) n.sante_mentale = 100;
            if (n.sante_mentale < 0) n.sante_mentale = 0;
            if (n.energie > 100) n.energie = 100;
            if (n.energie < 0) n.energie = 0;
            if (n.etudes > 100) n.etudes = 100;
            if (n.etudes < 0) n.etudes = 0;
            if (n.bonheur > 100) n.bonheur = 100;
            if (n.bonheur < 0) n.bonheur = 0;
            if (n.suspicion > 100) n.suspicion = 100;
            
            fast_forward(&n, action_choisie.duree_minutes, action_choisie.desc, 0);
        }
        else if (choix == nb_choix_affiches + 1) {
            n.lieu_actuel = APPART;
            int minutes_sommeil = 0;
            if (horloge_jeu.heure < 8) minutes_sommeil = (8 - horloge_jeu.heure) * 60 - horloge_jeu.minute;
            else minutes_sommeil = (24 - horloge_jeu.heure) * 60 - horloge_jeu.minute + (8 * 60);
            fast_forward(&n, minutes_sommeil, "Sommeil reparateur", 1);
        }
        else if (choix == nb_choix_affiches + 2) {
            printf("\n");
            print_padding(tw, 40); printf(BOLD "Ou voulez-vous aller ?\n" RESET);
            print_padding(tw, 40); printf("1: CROUS | 2: 809 | 3: APPART | 4: VILLE\n");
            print_padding(tw, 40); printf("Destination > ");
            int dest;
            if (scanf("%d", &dest) == 1 && dest >= 1 && dest <= 4) n.lieu_actuel = dest; 
            else while(getchar() != '\n');
        }
        else if (choix == nb_choix_affiches + 3) {
            sauvegarder_partie(&n);
            printf("\n");
            taper_texte("Partie sauvegardee avec succes. A bientot !", GREEN BOLD, tw);
            return 0; 
        }
    }
    
    // ==========================================
    // 4. ECRAN DE FIN
    // ==========================================
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

    remove("neil_save.dat"); 
    return 0;
} 