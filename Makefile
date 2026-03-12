# --- Variables ---
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g
TARGET = beneil

# On dit au Makefile d'aller chercher tous les fichiers .c dans le dossier src/
SRC = $(wildcard src/*.c)

# Pour chaque fichier .c, on crée un fichier objet .o correspondant
OBJ = $(SRC:.c=.o)

# --- Règles ---

# Règle par défaut quand tu tapes juste "make"
all: $(TARGET)

# Comment assembler l'exécutable final à partir des .o
$(TARGET): $(OBJ)
	@echo "Assemblage de $(TARGET)..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)
	@echo "Compilation terminée avec succès !"

# Comment compiler chaque fichier .c en fichier objet .o
%.o: %.c
	@echo "Compilation de $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Règle pour lancer le jeu directement
run: all
	@echo "Lancement de Be Neil.Inc...\n"
	./$(TARGET)

# Nettoyer les fichiers compilés (les .o et l'exécutable)
clean:
	@echo "Nettoyage des fichiers compilés..."
	rm -f src/*.o $(TARGET)

# Tout recompiler de zéro
re: clean all

# Indique que ces mots ne sont pas des noms de fichiers
.PHONY: all run clean re