#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "ui.h"
#include "moteur_temps.h" // Pour lire horloge_jeu

int get_terminal_width() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void print_padding(int largeur_term, int largeur_contenu) {
    int padding = (largeur_term - largeur_contenu) / 2;
    if (padding < 0) padding = 0;
    for (int i = 0; i < padding; i++) printf(" ");
}

void taper_texte(const char* texte, const char* couleur, int tw) {
    print_padding(tw, (int)strlen(texte));
    printf("%s", couleur);
    for (int i = 0; texte[i] != '\0'; i++) {
        printf("%c", texte[i]);
        fflush(stdout);
        usleep(25000); // Vitesse de la machine à écrire
    }
    printf(RESET "\n");
}

void animation_flash(int iterations) {
    for (int i = 0; i < iterations; i++) {
        printf("\033[47m\033[H\033[J");
        fflush(stdout); usleep(40000);
        printf(RESET "\033[H\033[J");
        fflush(stdout); usleep(40000);
    }
}

void barre_master(int val, const char* label, const char* col, int tw) {
    int b_size = 30; 
    print_padding(tw, 60); 
    printf(" %s %-10s ", col, label);
    printf(BOLD "[");
    for (int i = 0; i < b_size; i++) {
        if (i < (val * b_size) / 100) {
            if (val <= 25) printf(RED "█");
            else if (val <= 60) printf(YELLOW "█");
            else printf(GREEN "█");
        } else printf("\033[90m░");
    }
    printf(RESET BOLD "] %d%%\n" RESET, val);
}

const char* noms_lieux[] = {
    "PARTOUT", 
    "CROUS", 
    "809", 
    "APPART", 
    "RUE", 
    "ISEN", 
    "ENTREPRISE"
};

const char* noms_statuts[] = {
    "SOLO", 
    "DRAGUE", 
    "COUPLE", 
    "PIRATE"
};

void afficher_ui(Neil *n) {
    int tw = get_terminal_width();
    int cw = 64;
    printf("\033[H\033[J\n\n"); // Nettoie l'écran

    // HEADER
    print_padding(tw, cw); printf(CYAN B_TL); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_TR "\n");
    print_padding(tw, cw); 
    printf(B_V BOLD YELLOW "  %-29s  JOUR %02d - %02d:%02d  " CYAN B_V "\n", 
           "PROJET BE NEIL.INC", horloge_jeu.jour, horloge_jeu.heure, horloge_jeu.minute);
    print_padding(tw, cw); printf(B_ML); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_MR "\n");

    printf("\n");
    
    // LES JAUGES (Maintenant avec C-SKILLS et ENERGIE !)
    barre_master(n->sante_mentale, "MENTAL", MAGENTA, tw);
    barre_master(n->energie, "ENERGIE", YELLOW, tw);
    barre_master(n->etudes, "C-SKILLS", CYAN, tw);
    barre_master(n->bonheur, "SOCIAL", GREEN, tw);

    // --- LE PANNEAU SOCIAL ---
    print_padding(tw, cw); printf(B_ML); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_MR "\n");
    print_padding(tw, cw); 
    // Formatage précis pour faire 62 caractères de large à l'intérieur des bordures
    printf(B_V "  " MAGENTA "NGOC:" RESET " %-10s | " CYAN "CHARLOTTE:" RESET " %-10s | " RED "SUSPICION:" RESET " %-3d   " CYAN B_V "\n", 
           noms_statuts[n->etat_ngoc], noms_statuts[n->etat_charlotte], n->suspicion);
    // -----------------------------------

    // FOOTER (Avec le nom du lieu en toutes lettres !)
    print_padding(tw, cw); printf(B_ML); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_MR "\n");
    print_padding(tw, cw); 
    printf(B_V GREEN " CASH: %7.2f€ " RESET "| " MAGENTA "FAIM: %d%% " RESET "| " YELLOW "Lieu: %-11s " CYAN B_V "\n", 
           n->argent, n->faim, noms_lieux[n->lieu_actuel]); // <-- ICI on met le %s et le tableau
    print_padding(tw, cw); printf(B_BL); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_BR "\n\n" RESET);
}

void afficher_ecran_fin(Neil *n) {
    int tw = get_terminal_width();
    int cw = 64; // Largeur du cadre
    
    // Nettoyage et effet dramatique
    printf("\033[H\033[J\n\n");
    animation_flash(2);
    
    // Calcul du score (une formule secrète d'ingénieur !)
    int score_total = n->etudes + n->sante_mentale + n->bonheur + (int)(n->argent / 10);
    
    char titre_fin[64];
    char desc_fin[256];
    char couleur_titre[16];
    
    // --- L'ARBRE DES FINS POSSIBLES ---
    
    // 1. La Fin Secrète (La Piraterie Parfaite)
    if ((int)n->etat_ngoc >= COUPLE && (int)n->etat_charlotte >= COUPLE && n->suspicion < 100) {
        strcpy(titre_fin, "PIRATE LEGENDAIRE");
        strcpy(desc_fin, "Vous avez valide l'ISEN tout en maintenant une double vie parfaite entre Ngoc et Charlotte. Le boss final de la matrice.");
        strcpy(couleur_titre, MAGENTA);
        score_total += 500; // Gros bonus de complétion
    } 
    // 2. La Fin Scolaire
    else if (n->etudes >= 85 && n->sante_mentale >= 50) {
        strcpy(titre_fin, "INGENIEUR D'ELITE");
        strcpy(desc_fin, "Major de promo a l'ISEN. Le C n'a plus de secrets pour vous. Par contre, votre vie sociale est un desert absolu.");
        strcpy(couleur_titre, CYAN);
    } 
    // 3. La Fin Sociale
    else if (n->bonheur >= 85 && n->etudes >= 20) {
        strcpy(titre_fin, "LE ROI DU 809");
        strcpy(desc_fin, "Vous avez survecu a l'ecole de justesse, mais vous etes une legende de la nuit. Tout le monde connait votre nom au 809.");
        strcpy(couleur_titre, GREEN);
    } 
    // 4. La Fin par Défaut
    else {
        strcpy(titre_fin, "SURVIVANT DE L'EXTREME");
        strcpy(desc_fin, "Vous avez passe le mois en evitant le pire. Pas de gloire, pas de drame, juste la routine d'un etudiant epuise.");
        strcpy(couleur_titre, YELLOW);
    }

    // --- L'AFFICHAGE DU BILAN ---
    print_padding(tw, cw); printf(YELLOW B_TL); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_TR "\n");
    print_padding(tw, cw); printf(B_V "               BILAN DU MOIS D'ALTERNANCE               " B_V "\n");
    print_padding(tw, cw); printf(B_ML); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_MR "\n");
    
    print_padding(tw, cw); printf(B_V " Titre obtenu : %s%-39s" YELLOW B_V "\n", couleur_titre, titre_fin);
    print_padding(tw, cw); printf(B_V " Score Final  : " BOLD "%-39d" RESET YELLOW B_V "\n", score_total);
    print_padding(tw, cw); printf(B_ML); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_MR "\n");
    
    printf("\n");
    print_padding(tw, cw); printf(">> %s\n\n", desc_fin);
    
    print_padding(tw, cw); printf("Stats Finales : C-SKILLS [%d%%] | MENTAL [%d%%] | SOCIAL [%d%%]\n", 
           n->etudes, n->sante_mentale, n->bonheur);
    printf("\n");
}