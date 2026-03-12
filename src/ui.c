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

    printf("\n");

    // FOOTER
    print_padding(tw, cw); printf(B_ML); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_MR "\n");
    print_padding(tw, cw); 
    printf(B_V GREEN " CASH: %7.2f€ " RESET "| " MAGENTA "FAIM: %d%% " RESET "| " YELLOW "Lieu: %-11d " CYAN B_V "\n", 
           n->argent, n->faim, n->lieu_actuel);
    print_padding(tw, cw); printf(B_BL); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_BR "\n\n" RESET);
}