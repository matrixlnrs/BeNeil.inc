# BeNeil.inc : Student Simulator

Ce projet est un simulateur de vie étudiante développé en Langage C, modélisant le quotidien d'un apprenti à l'ISEN. Ce projet sera soutenu le 13/03/2026 (Coefficient 40%).

---

## Objectif du projet:

L'objectif est de gérer les ressources de Neil (Santé Mentale, Études, Vie Sociale et Argent) pour valider son semestre sans atteindre le burnout. Le jeu utilise un système d'événements discrets basé sur des bases de données externes.

## Installation et Compilation

Le projet utilise un Makefile pour simplifier la gestion des fichiers sources.

* Pour compiler : `make`
* Pour lancer le jeu : `./beneil`

## Contrôles et Raccourcis

L'interface est exclusivement textuelle (Terminal) :

* Entrée : Valider un choix ou passer au jour suivant.
* Chiffres (1, 2, 3...) : Sélectionner une action dans le menu dynamique.

## Système de Jauges

* Mental : Si elle tombe à 0%, Neil subit un Burnout.
* C-Skills : Représente la réussite académique. À 0%, Neil est exclu de l'école.
* Social : Gère la stabilité des relations (Le roster avec Ngoc et Charlotte).
* Cash : Ton budget de survie. Attention aux restos chic et aux cadeaux imprévus !