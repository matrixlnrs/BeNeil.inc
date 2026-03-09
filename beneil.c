#include "beneil.h"

void ecrire_journal(const char* message) {
    FILE *f = fopen(FICHIER_LOG, "a");
    if (f) {
        time_t now = time(NULL);
        char *date = ctime(&now);
        date[strlen(date) - 1] = '\0'; 
        fprintf(f, "[%s] %s\n", date, message);
        fclose(f);
    }
}

int lire_entier(int min, int max, const char* msg) {
    int choix;
    while (1) {
        printf("%s", msg);
        if (scanf("%d", &choix) == 1 && choix >= min && choix <= max) {
            while(getchar() != '\n'); 
            return choix;
        }
        printf(RED "Saisie invalide (min:%d, max:%d).\n" RESET, min, max);
        while(getchar() != '\n');
    }
}

void barre_progression(int val, char* label, char* col) {
    printf("%-12s %s[", label, col);
    int b = (val > 0) ? (val / 5) : 0;
    if (b > 20) b = 20;
    for (int i = 0; i < 20; i++) printf(i < b ? "█" : " ");
    printf("] %d%%%s\n", val, RESET);
}

void sauvegarder_partie(Neil n) {
    FILE *f = fopen(FICHIER_SAVE, "wb");
    if (f) {
        fwrite(&n, sizeof(Neil), 1, f);
        fclose(f);
        ecrire_journal("Sauvegarde binaire effectuee.");
        printf(GREEN "\nPartie sauvegardee !\n" RESET);
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

int charger_actions(Action tab[]) {
    FILE *f = fopen("actions.txt", "r");
    if (!f) return 0;
    int nb = 0;
    char buf[256];
    while (fgets(buf, 256, f) && nb < MAX_ACT) {
        if (buf[0] == '#' || strlen(buf) < 5) continue;
        sscanf(buf, "%d;%[^;];%d;%d;%d;%f;%d;%d;%d", 
               &tab[nb].id, tab[nb].desc, &tab[nb].m_san, &tab[nb].m_etu, 
               &tab[nb].m_bon, &tab[nb].m_arg, &tab[nb].req_am, 
               &tab[nb].cible, (int*)&tab[nb].lieu);
        nb++;
    }
    fclose(f);
    return nb;
}

void check_collision(Neil *n) {
    if (!((n->etat_ngoc >= COUPLE && n->etat_charlotte >= DRAGUE) || 
          (n->etat_charlotte >= COUPLE && n->etat_ngoc >= DRAGUE))) return;

    int proba = 5;
    if (n->lieu_actuel == KFET) proba += 20;
    if (n->lieu_actuel == RUE)  proba += 10;
    proba += (n->suspicion / 4);

    if ((rand() % 100) < proba) {
        printf(RED BOLD "\n!!! DRAMA !!! L'autre cible vient de débarquer au même endroit !\n" RESET);
        ecrire_journal("COLLISION : Neil s'est fait attraper.");
        n->bonheur = 0; n->sante -= 50;
        n->etat_ngoc = SOLO; n->etat_charlotte = SOLO; n->suspicion = 0;
        sleep(2);
    }
}

void executer_evenement_angoctoire(Neil *n) {
    int r = rand() % 100;
    if (r < 10) {
        printf(YELLOW "\n[EVENT] Ton binôme a fini le projet de C pour toi ! (+20 Etudes)\n" RESET);
        n->etudes += 20;
    } else if (r < 20) {
        printf(RED "\n[EVENT] Gastro-entérite foudroyante... (-20 Mental)\n" RESET);
        n->sante -= 20;
    }
}

void afficher_ui(Neil n) {
    printf("\033[H\033[J");
    printf(CYAN "====================================================\n" RESET);
    printf(BOLD " JOUR %d / %d  |  STATUT: %s\n" RESET, 
           n.jour, JOURS_MAX, (n.etat_ngoc >= COUPLE && n.etat_charlotte >= COUPLE ? "PIRATE" : "NORMAL"));
    printf(CYAN "====================================================\n" RESET);
    barre_progression(n.sante, "MENTAL", (n.sante < 25 ? RED : GREEN));
    barre_progression(n.etudes, "C SKILLS", CYAN);
    barre_progression(n.bonheur, "SOCIAL", YELLOW);
    if (n.suspicion > 0) barre_progression(n.suspicion, "PARANOIA", RED);
    printf("\nArgent: %.2f EUR | Lieu: %d\n", n.argent, n.lieu_actuel);
    printf("Relations: Ngoc [%d/3] | Charlotte [%d/3]\n", n.etat_ngoc, n.etat_charlotte);
}

int main() {
    srand(time(NULL));
    Neil n;
    Action actions[MAX_ACT];

    if (!charger_partie(&n)) {
        strcpy(n.nom, "Neil");
        n.sante = 80; n.etudes = 20; n.bonheur = 50; n.argent = 200.0;
        n.jour = 1; n.etat_ngoc = SOLO; n.etat_charlotte = SOLO;
        n.suspicion = 0; n.lieu_actuel = PARTOUT;
    }

    int nb_actions = charger_actions(actions);
    remove(FICHIER_LOG);
    ecrire_journal("Nouvelle session de jeu.");

    while (n.jour <= JOURS_MAX && n.sante > 0 && n.etudes > 0) {
        afficher_ui(n);
        int map[MAX_ACT], v = 0;

        printf("\nActions disponibles :\n");
        for (int i = 0; i < nb_actions; i++) {
            int aff = 0;
            if (actions[i].cible == 0) aff = 1;
            else if (actions[i].cible == 1 && n.etat_ngoc >= actions[i].req_am) aff = 1;
            else if (actions[i].cible == 2 && n.etat_charlotte >= actions[i].req_am) aff = 1;
            else if (actions[i].cible == 3 && (n.etat_ngoc >= COUPLE && n.etat_charlotte >= DRAGUE)) aff = 1;

            if (aff) {
                printf("  %d. %-35s [%.0f€]\n", v + 1, actions[i].desc, actions[i].m_arg);
                map[v++] = i;
            }
        }
        printf("  %d. " YELLOW "Sauvegarder et Quitter\n" RESET, v + 1);

        int c = lire_entier(1, v + 1, "\nChoix > ") - 1;
        if (c == v) { sauvegarder_partie(n); return 0; }

        Action s = actions[map[c]];
        n.sante += s.m_san; n.etudes += s.m_etu; n.bonheur += s.m_bon; 
        n.argent += s.m_arg; n.lieu_actuel = s.lieu;

        if (s.cible == 1 && n.etat_ngoc == s.req_am) n.etat_ngoc++;
        if (s.cible == 2 && n.etat_charlotte == s.req_am) n.etat_charlotte++;
        if (n.etat_ngoc >= DRAGUE && n.etat_charlotte >= DRAGUE) n.suspicion += 8;

        executer_evenement_angoctoire(&n);
        check_collision(&n);
        n.jour++;

        if (n.sante > 100) n.sante = 100;
        if (n.etudes > 100) n.etudes = 100;
        
        printf("\nAppuyez sur Entree..."); getchar();
    }

    printf(RED "\n--- FIN DE SIMULATION ---\n" RESET);
    ecrire_journal("Fin de partie.");
    return 0;
}