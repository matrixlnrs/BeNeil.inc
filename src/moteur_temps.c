#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // pour usleep()
#include "moteur_temps.h"
#include "ui.h" // Pour utiliser animation_flash et taper_texte pendant l'imprévu

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

// LE GENERATEUR DE CHAOS (L'Interro Surprise)
void declencher_imprevu(Neil *n) {
    int tw = get_terminal_width();
    
    // On met en pause la simulation avec un gros effet visuel
    animation_flash(2);
    printf("\033[H\033[J\n\n"); // Efface l'écran
    taper_texte("!!! ATTENTION : INTERRO SURPRISE DE C !!!", RED BOLD, tw);
    printf("\n");

    // On tire une question au hasard (entre 0 et 2)
    int q = rand() % 3; 
    int reponse;

    if (q == 0) {
        print_padding(tw, 60); printf("Le prof vous fixe : 'Que fait la fonction malloc ?'\n\n");
        print_padding(tw, 60); printf(" 1. Elle libere la memoire\n");
        print_padding(tw, 60); printf(" 2. Elle alloue de la memoire dynamiquement\n");
        print_padding(tw, 60); printf(" 3. Elle copie une chaine de caracteres\n");
        print_padding(tw, 60); printf("\nVotre reponse > ");
        
        if(scanf("%d", &reponse) != 1) while(getchar() != '\n');
        
        if (reponse == 2) {
            taper_texte("\nBonne reponse ! Le jury hoche la tete. (+15 C-SKILLS)", GREEN BOLD, tw);
            n->etudes += 15;
        } else {
            taper_texte("\nFaux ! Le jury est consterne. (-15 MENTAL)", RED BOLD, tw);
            n->sante_mentale -= 15;
        }
    } 
    else if (q == 1) {
        print_padding(tw, 60); printf("Question au tableau : 'Quel est le format pour afficher un entier avec printf ?'\n\n");
        print_padding(tw, 60); printf(" 1. %%f\n");
        print_padding(tw, 60); printf(" 2. %%c\n");
        print_padding(tw, 60); printf(" 3. %%d\n");
        print_padding(tw, 60); printf("\nVotre reponse > ");
        
        if(scanf("%d", &reponse) != 1) while(getchar() != '\n');
        
        if (reponse == 3) {
            taper_texte("\nExactement. Vous maitrisez les bases. (+10 C-SKILLS)", GREEN BOLD, tw);
            n->etudes += 10;
        } else {
            taper_texte("\nAie... Erreur de syntaxe. (-15 MENTAL)", RED BOLD, tw);
            n->sante_mentale -= 15;
        }
    }
    else {
        print_padding(tw, 60); printf("Question piege : 'A quoi sert le mot cle extern ?'\n\n");
        print_padding(tw, 60); printf(" 1. A declarer une variable definie dans un autre fichier\n");
        print_padding(tw, 60); printf(" 2. A quitter le programme immediatement\n");
        print_padding(tw, 60); printf(" 3. A creer une boucle infinie\n");
        print_padding(tw, 60); printf("\nVotre reponse > ");
        
        if(scanf("%d", &reponse) != 1) while(getchar() != '\n');
        
        if (reponse == 1) {
            taper_texte("\nParfait ! Belle maitrise de l'architecture. (+20 C-SKILLS)", GREEN BOLD, tw);
            n->etudes += 20;
        } else {
            taper_texte("\nCompilation echouee... (-15 MENTAL)", RED BOLD, tw);
            n->sante_mentale -= 15;
        }
    }
    
    // Sécurité pour ne pas dépasser les 100% de stats
    if (n->etudes > 100) n->etudes = 100;
    
    sleep(2); // On laisse le joueur lire le résultat
    printf("\033[H\033[J\n\n"); // On nettoie avant de reprendre l'ellipse
}

// L'ALGORITHME DE COLLISION SOCIALE (Le Drama)
void verifier_collision_sociale(Neil *n) {
    // 1. Condition : Neil doit fréquenter les deux en même temps !
    if (n->etat_ngoc >= DRAGUE && n->etat_charlotte >= DRAGUE) {
        
        int risque_base = 0;
        
        // 2. Modificateur de Lieu
        if (n->lieu_actuel == FOYER809) {
            risque_base = 20; // Danger absolu en soirée
        } else if (n->lieu_actuel == CROUS || n->lieu_actuel == VILLE) {
            risque_base = 5;  // Danger modéré en public
        } else if (n->lieu_actuel == APPART) {
            risque_base = 1;  // Danger faible à domicile (visite surprise)
        }
        
        // 3. On ajoute la suspicion accumulée par les mensonges
        int risque_total = risque_base + n->suspicion;
        
        // 4. Le lancer de dés (de 0 à 99)
        int de_karma = rand() % 100;
        
        // Si le dé tombe sous le pourcentage de risque... BUSTED.
        if (risque_total > 0 && de_karma < risque_total) {
            int tw = get_terminal_width();
            animation_flash(4);
            printf("\033[H\033[J\n\n");
            taper_texte("!!! COLLISION SOCIALE DETECTEE !!!", MAGENTA BOLD, tw);
            printf("\n");
            
            // --- LE TEXTE S'ADAPTE PARFAITEMENT AU LIEU ---
            if (n->lieu_actuel == APPART) {
                // Cas 1 : À la maison
                print_padding(tw, 60);
                printf("DING DONG. Quelqu'un sonne a la porte de l'appart...\n");
                print_padding(tw, 60);
                printf("Ngoc ET Charlotte ont eu l'idee de vous faire une surprise.\n");
                print_padding(tw, 60);
                printf("Elles se croisent sur le palier. Le silence est lourd.\n");
            } 
            else if (n->lieu_actuel == FOYER809) { 
                // Cas 2 : En boîte de nuit
                print_padding(tw, 60);
                printf("Le temps s'arrete. La musique du 809 semble s'assourdir.\n");
                print_padding(tw, 60);
                printf("Ngoc et Charlotte viennent de se croiser pres du bar.\n");
                print_padding(tw, 60);
                printf("La soiree vient de prendre une tournure glaciale.\n");
            } 
            else {
                // Cas 3 : Partout ailleurs (ISEN, Rue, CROUS, Entreprise)
                print_padding(tw, 60);
                printf("Le temps s'arrete. Les bruits de fond se figent.\n");
                print_padding(tw, 60);
                printf("Le karma a frappe : Ngoc et Charlotte sont au meme endroit.\n");
                print_padding(tw, 60);
                printf("Elles viennent de se croiser juste devant vous.\n");
            }
            
            print_padding(tw, 60);
            printf("Elles vous regardent simultanement avec degout.\n\n");
            
            // Les conséquences nucléaires
            taper_texte("-> GAME OVER SOCIAL : Vos relations sont detruites.", RED BOLD, tw);
            taper_texte("-> -50 SOCIAL | -40 MENTAL", RED BOLD, tw);
            
            n->bonheur -= 50;
            n->sante_mentale -= 40;
            
            // Retour à la case départ pour les deux
            n->etat_ngoc = SOLO;
            n->etat_charlotte = SOLO;
            n->suspicion = 0; // Au moins, il n'y a plus de secret !
            
            sleep(4); // Pause dramatique pour laisser le joueur pleurer
            printf("\033[H\033[J\n\n");
        }
    }
}

// 6. L'ELLIPSE TEMPORELLE (Un peu comme dans "Youtuber's Life")
void fast_forward(Neil* n, int minutes_a_passer, const char* nom_activite, int est_repos) {
    int tw = get_terminal_width();
    
    for (int i = 0; i < minutes_a_passer; i += 15) {
        int pas = (minutes_a_passer - i >= 15) ? 15 : (minutes_a_passer - i);
        avancer_horloge_un_pas(pas);
        
        // Mise à jour des stats
        if (est_repos == 1) {
            n->energie += 4; 
            if (n->energie > 100) n->energie = 100;
        } else {
            n->energie -= 1; 
            n->faim += 2;
        }

        // --- ANIMATION VISUELLE DU TEMPS (Style Youtuber's Life) ---
        printf("\033[H\033[J\n\n\n"); // Nettoie l'écran et se place en haut
        print_padding(tw, 44); printf(CYAN "============================================\n" RESET);
        print_padding(tw, 44); printf(BOLD "         >>> TEMPS ACCELERE <<<         \n" RESET);
        print_padding(tw, 44); printf(YELLOW "   %-36s\n" RESET, nom_activite);
        print_padding(tw, 44); printf(CYAN "============================================\n\n" RESET);

        // La grosse horloge
        print_padding(tw, 16); printf(BOLD "  JOUR %02d\n" RESET, horloge_jeu.jour);
        print_padding(tw, 16); printf(BOLD MAGENTA "  %02d : %02d\n" RESET, horloge_jeu.heure, horloge_jeu.minute);
        printf("\n");

        // La barre de progression
        int progress = ((i + pas) * 100) / minutes_a_passer;
        print_padding(tw, 44); printf(" Progression : [");
        for(int p = 0; p < 20; p++) {
            if (p < progress / 5) printf(GREEN "█" RESET);
            else printf("\033[90m░" RESET);
        }
        printf("] %d%%\n\n", progress);

        fflush(stdout); // Force l'affichage immédiat dans le terminal
        usleep(150000); // Pause de 0.15 seconde pour voir le temps défiler !
        // -----------------------------------------------------------

        // VÉRIFICATION 1 : L'Échéancier classique
        while (tete_echeancier != NULL && tete_echeancier->declenchement_absolu <= horloge_jeu.temps_absolu) {
            executer_prochain_evenement(n);
        }
        
        // VÉRIFICATION 2 : Urgence Fatigue
        if (est_repos == 0 && n->energie <= 0) {
            printf("\n");
            print_padding(tw, 40); printf(RED BOLD "!!! URGENCE : Neil s'effondre de fatigue !!!\n" RESET);
            n->energie = 0;
            n->sante_mentale -= 30;
            sleep(2); // Laisse le temps de lire
            break; 
        }

        // VÉRIFICATION 3 : LE QCM ALÉATOIRE (Seulement à l'ISEN et pendant les cours !)
        if (est_repos == 0 && n->lieu_actuel == 5 && horloge_jeu.heure >= 8 && horloge_jeu.heure < 16) {
            int de_chaos = rand() % 100;
            if (de_chaos < 2) { 
                declencher_imprevu(n); 
            }
        }

        // VÉRIFICATION 4 : LA COLLISION SOCIALE
        if (est_repos == 0) {
            verifier_collision_sociale(n);
        }
    }
}