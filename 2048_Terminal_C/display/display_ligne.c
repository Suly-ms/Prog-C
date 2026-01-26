#include <stdio.h>
#include "display_ligne.h"

// Affiche une ligne de séparation 
static void print_separator(void) {
    printf("+");
    for (int i = 0; i < GRID_SIZE; i++) {
        printf("------+");
    }
    printf("\n");
}

// Affiche une rangée de nombres
static void display_row(const uint16_t *row) {
    printf("|");
    for (int i = 0; i < GRID_SIZE; i++) {
        if (row[i] == 0) {
            printf("  .   |"); 
        } else {
            printf(" %4d |", row[i]);
        }
    }
    printf("\n");
}

// C'est le "main" pour l'affichage
void display_game(const game_data *game) {
    printf("\nSCORE: %zu\n", game->score);

    print_separator(); 

    for (int i = 0; i < GRID_SIZE; i++) {
        display_row(game->grid[i]);
        print_separator();
    }
    printf("\n");
}
