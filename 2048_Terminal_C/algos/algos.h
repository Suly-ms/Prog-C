#ifndef ALGOS_H
#define ALGOS_H

#include <stdint.h>
#include <stdlib.h>
#include "../include/shared.h"

// Note: On utilise ici une structure locale pour la compatibilité avec display_ligne
typedef struct {
    uint16_t grid[GRID_SIZE][GRID_SIZE];
    size_t score;
} game_data;

// Ajoute une tuile (2 ou 4) au hasard
void add_random_tile(uint16_t grid[GRID_SIZE][GRID_SIZE]);

// Vérifie si la partie est finie
int check_game_over(uint16_t grid[GRID_SIZE][GRID_SIZE]);

#endif
