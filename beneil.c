#include "beneil.h"

char* noms_lieux[] = {"PARTOUT", "CROUS", "809", "APPART", "RUE"};

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

void taper_texte(char* texte, char* couleur, int tw) {
    print_padding(tw, (int)strlen(texte));
    printf("%s", couleur);
    for (int i = 0; texte[i] != '\0'; i++) {
        printf("%c", texte[i]);
        fflush(stdout);
        usleep(25000);
    }
    printf(RESET "\n");
}

void sauvegarder_partie(Neil n) {
    FILE *f = fopen(FICHIER_SAVE, "wb");
    if (f) {
        fwrite(&n, sizeof(Neil), 1, f);
        fclose(f);
    }
}

int charger_partie(Neil *n) {
    FILE *f = fopen(FICHIER_SAVE, "rb");
    if (f) {
        fread(n, sizeof(Neil), 1, f);
        fclose(f);
        return 1;
    }
    return 0;
}

void animation_flash(int iterations) {
    for (int i = 0; i < iterations; i++) {
        printf("\033[47m\033[H\033[J");
        fflush(stdout); usleep(40000);
        printf(RESET "\033[H\033[J");
        fflush(stdout); usleep(40000);
    }
}

void animation_shake(int intensite) {
    for (int i = 0; i < intensite; i++) {
        printf(" "); fflush(stdout); usleep(20000);
        printf("\b\b"); fflush(stdout); usleep(20000);
    }
}

void barre_master(int val, char* label, char* col, int tw) {
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

void afficher_ui(Neil n) {
    int tw = get_terminal_width();
    int cw = 62;
    printf("\033[H\033[J\n\n");

    print_padding(tw, cw); printf(CYAN B_TL); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_TR "\n");
    print_padding(tw, cw); printf(B_V BOLD YELLOW "  %-42s  JOUR %02d/30  " CYAN B_V "\n", "PROJET BE NEIL.INC", n.jour);
    print_padding(tw, cw); printf(B_ML); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_MR "\n");

    printf("\n");
    barre_master(n.sante, "MENTAL", MAGENTA, tw);
    barre_master(n.etudes, "C-SKILLS", CYAN, tw);
    barre_master(n.bonheur, "SOCIAL", YELLOW, tw);
    
    if ((int)n.etat_ngoc >= (int)DRAGUE && (int)n.etat_charlotte >= (int)DRAGUE) 
        barre_master(n.suspicion, "PARANOIA", RED, tw);

    printf("\n");

    print_padding(tw, cw); printf(B_ML); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_MR "\n");
    print_padding(tw, cw); 
    printf(B_V GREEN "  CASH: %7.2f€ " RESET "| " MAGENTA "L: %d/3 S: %d/3 " RESET "| " YELLOW "Lieu: %-8s " CYAN B_V "\n", 
           n.argent, (int)n.etat_ngoc, (int)n.etat_charlotte, noms_lieux[n.lieu_actuel]);
    print_padding(tw, cw); printf(B_BL); for(int i=0; i<cw-2; i++) printf(B_H); printf(B_BR "\n\n" RESET);
}

void cinematique_collision(Neil *n) {
    int tw = get_terminal_width();
    animation_flash(3);
    taper_texte("!!! ALERTE : INTERSECTION NON GEREE !!!", RED BOLD, tw);
    animation_shake(20);
    taper_texte("Tu es au 809. Ngoc et Charlotte entrent en meme temps.", YELLOW, tw);
    sleep(1);
    taper_texte("LE SYSTEME S'EFFONDRE. ELLES SAVENT TOUT.", RED BOLD, tw);
    
    n->bonheur = 0; 
    n->sante -= 50;
    n->etat_ngoc = SOLO; 
    n->etat_charlotte = SOLO; 
    n->suspicion = 0;
    sleep(3);
}

void check_collision(Neil *n) {
    if (!(((int)n->etat_ngoc >= (int)COUPLE && (int)n->etat_charlotte >= (int)DRAGUE) || 
          ((int)n->etat_charlotte >= (int)COUPLE && (int)n->etat_ngoc >= (int)DRAGUE))) return;
    
    int proba = 5; 
    if (n->lieu_actuel == 2) proba += 30;
    if (n->lieu_actuel == 1) proba += 10;
    proba += (n->suspicion / 4);

    if ((rand() % 100) < proba) cinematique_collision(n);
}

int charger_actions(Action tab[]) {
    FILE *f = fopen("actions.txt", "r");
    if (!f) return 0;
    int nb = 0; char buf[256];
    while (fgets(buf, 256, f) && nb < MAX_ACT) {
        if (buf[0] == '#' || strlen(buf) < 10) continue;
        sscanf(buf, "%d;%[^;];%d;%d;%d;%f;%d;%d;%d", 
               &tab[nb].id, tab[nb].desc, &tab[nb].m_san, &tab[nb].m_etu, 
               &tab[nb].m_bon, &tab[nb].m_arg, &tab[nb].req_am, &tab[nb].cible, (int*)&tab[nb].lieu);
        nb++;
    }
    fclose(f);
    return nb;
}

int lire_entier(int min, int max, const char* msg, int tw) {
    int c;
    while (1) {
        print_padding(tw, 40); printf("%s", msg);
        if (scanf("%d", &c) == 1 && c >= min && c <= max) {
            while(getchar() != '\n') ; 
            return c;
        }
        while(getchar() != '\n') ;
        print_padding(tw, 40); printf(RED "Entree invalide !\n" RESET);
    }
}

int main() {
    srand((unsigned int)time(NULL));
    int tw = get_terminal_width();
    Neil n;
    
    printf("\033[H\033[J\n\n\n");
    taper_texte("--- BE NEIL.INC : INITIALISATION DU SYSTEME ---", CYAN BOLD, tw);
    printf("\n");
    
    int save_exists = charger_partie(&n);
    int choix_start = 0;

    if (save_exists) {
        print_padding(tw, 50); printf(GREEN BOLD "SAUVEGARDE TROUVEE (Jour %d)\n" RESET, n.jour);
        print_padding(tw, 50); printf("1. Reprendre la piraterie\n");
        print_padding(tw, 50); printf("2. Reset le systeme (Nouvelle partie)\n");
        choix_start = lire_entier(1, 2, "Choix > ", tw);
    } else {
        print_padding(tw, 50); printf(YELLOW "Aucune archive detectee.\n" RESET);
        print_padding(tw, 50); printf("Appuyez sur ENTREE pour generer Neil...");
        getchar();
        choix_start = 2;
    }

    if (choix_start == 2) {
        strcpy(n.nom, "Neil");
        n.sante = 80; n.etudes = 20; n.bonheur = 50; n.argent = 250.0;
        n.jour = 1; n.etat_ngoc = SOLO; n.etat_charlotte = SOLO;
        n.suspicion = 0; n.lieu_actuel = PARTOUT;
        animation_flash(1);
    }

    Action actions[MAX_ACT];
    int nb_act = charger_actions(actions);
    if (nb_act == 0) { printf("ERREUR : Fichier actions.txt introuvable !\n"); return 1; }

    while (n.jour <= JOURS_MAX && n.sante > 0 && n.etudes > 0) {
        tw = get_terminal_width();
        afficher_ui(n);
        int map[MAX_ACT], v = 0;

        print_padding(tw, 40); printf(BOLD UNDERLINE "ACTIONS DISPONIBLES :" RESET "\n\n");
        
        for (int i = 0; i < nb_act; i++) {
            int peut_afficher = 0;
            if (actions[i].cible == 0) peut_afficher = 1;
            else if (actions[i].cible == 1 && (int)n.etat_ngoc >= actions[i].req_am) peut_afficher = 1;
            else if (actions[i].cible == 2 && (int)n.etat_charlotte >= actions[i].req_am) peut_afficher = 1;
            else if (actions[i].cible == 3 && ((int)n.etat_ngoc >= (int)COUPLE && (int)n.etat_charlotte >= (int)DRAGUE)) peut_afficher = 1;

            if (peut_afficher) {
                print_padding(tw, 62);
                printf(" %2d. %-38s [%+.0f€]\n", v + 1, actions[i].desc, actions[i].m_arg);
                map[v++] = i;
            }
        }
        
        print_padding(tw, 62); printf(CYAN " %2d. Sauvegarder et mettre le systeme en veille\n" RESET, v + 1);

        int c = lire_entier(1, v + 1, "\nAction > ", tw) - 1;
        
        if (c == v) {
            sauvegarder_partie(n);
            taper_texte("Progression archivee. Deconnexion...", GREEN, tw);
            return 0;
        }

        Action s = actions[map[c]];
        n.sante += s.m_san; n.etudes += s.m_etu; n.bonheur += s.m_bon;
        n.argent += s.m_arg; n.lieu_actuel = s.lieu;

        if (s.cible == 1 && (int)n.etat_ngoc == s.req_am) n.etat_ngoc++;
        if (s.cible == 2 && (int)n.etat_charlotte == s.req_am) n.etat_charlotte++;
        
        if ((int)n.etat_ngoc >= (int)DRAGUE && (int)n.etat_charlotte >= (int)DRAGUE) n.suspicion += 7;

        check_collision(&n);
        n.jour++;
        
        if (n.sante > 100) n.sante = 100; 
        if (n.etudes > 100) n.etudes = 100;
        
        print_padding(tw, 40); printf("\nAppuyez sur ENTREE pour passer au jour suivant..."); 
        getchar();
    }
    
    if (n.sante <= 0 || n.etudes <= 0) {
        animation_flash(2);
        taper_texte("SYSTEM FAILURE : Neil a succombe au stress ou a l'echec scolaire.", RED BOLD, tw);
    } else {
        taper_texte("FELICITATIONS : Neil a survecu au semestre (et peut-etre a ses ex).", GREEN BOLD, tw);
    }
    
    remove(FICHIER_SAVE);
    return 0;
}