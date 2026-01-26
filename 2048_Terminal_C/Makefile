# ==========================================
#      MAKEFILE (Architecture Système)
# ==========================================

TARGET   := 2048_project

# --- Dossiers ---
SRC_DIR  := src
ALGO_DIR := algos
DISP_DIR := display
INC_DIR  := include
OBJ_DIR  := obj

# --- Sources ---
# On va chercher les sources dans ., src, algos et display
SRCS     := main.c \
            $(wildcard $(SRC_DIR)/*.c) \
            $(wildcard $(ALGO_DIR)/*.c) \
            $(wildcard $(DISP_DIR)/*.c)

# Transformation en objets (.o)
OBJS     := $(SRCS:%.c=$(OBJ_DIR)/%.o)
DEPS     := $(OBJS:.o=.d)

# --- Compilateur ---
CC       := gcc
# -pthread est OBLIGATOIRE pour les threads
# -I permet d'inclure directement "shared.h" ou "algos.h"
CFLAGS   := -Wall -Wextra -Werror -g -pthread -I. -I$(INC_DIR) -I$(ALGO_DIR) -I$(DISP_DIR)
LDFLAGS  := -pthread

# --- Règles ---

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	@echo "Linking $@"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build successful!"

# Règle générique pour tout compiler en gardant la structure dans obj/
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:
	@rm -rf $(OBJ_DIR)
	@echo "Cleaned objects."

fclean: clean
	@rm -f $(TARGET)
	@echo "Cleaned executable."

re: fclean all

.PHONY: all clean fclean re
