TARGET = scanp

OUT_DIR = out

SRC = scanp.c

HEADER = scanp.h

CC = gcc
CFLAGS = -Wall -Wextra -Werror

EXECUTABLE = $(OUT_DIR)/$(TARGET)

.PHONY: all clean

all: $(OUT_DIR) $(EXECUTABLE)

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)
	@echo "Dossier $(OUT_DIR)/ créé."

$(EXECUTABLE): $(SRC) $(HEADER)
	$(CC) $(CFLAGS) $(SRC) -o $(EXECUTABLE)
	@echo "Compilation terminée. Exécutable créé : $(EXECUTABLE)"

clean:
	@rm -rf $(OUT_DIR)
	@echo "Nettoyage terminé. Dossier $(OUT_DIR)/ supprimé."
