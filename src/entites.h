#ifndef ENTITES_H
#define ENTITES_H

// Lieux possibles
typedef enum { PARTOUT, CROUS, FOYER809, APPART, RUE, ISEN, ENTREPRISE } Lieu;
// Types de relations
typedef enum { SOLO, DRAGUE, COUPLE, PIRATE } StatutAmour;

// Le catalogue d'actions (chargé depuis actions.txt)
typedef struct {
    int id;
    char desc[128];
    int m_san, m_etu, m_bon, req_am, cible; 
    float m_arg;
    Lieu lieu;
    int duree_minutes; // NOUVEAU : Combien de temps prend l'action ?
} Action;

// La structure principale du joueur
typedef struct {
    char nom[32];
    
    // Jauges (0 à 100)
    int sante_mentale; // Ancien "sante"
    int energie;       // NOUVEAU : Pour la fatigue et les nuits blanches
    int faim;          // NOUVEAU : Besoin primaire
    int etudes;        // Compétence scolaire (C-SKILLS)
    int bonheur;       // Social / Fun
    
    // Autres stats
    float argent;
    StatutAmour etat_ngoc;
    StatutAmour etat_charlotte;
    int suspicion;     // Paranoïa / Karma
    Lieu lieu_actuel;
    
} Neil;

#endif