#ifndef FICHIERS_H
#define FICHIERS_H

#include "entites.h"

// Prototypes des fonctions de gestion de fichiers
int charger_actions(Action tab_actions[]);
void sauvegarder_partie(Neil *n);
int charger_partie(Neil *n);

#endif