#include "algos.h"

void add_random_tile(uint16_t grid[GRID_SIZE][GRID_SIZE]) {
    int empty_cells[GRID_SIZE * GRID_SIZE][2];
    int count = 0;

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (grid[i][j] == 0) {
                empty_cells[count][0] = i;
                empty_cells[count][1] = j;
                count++;
            }
        }
    }

    if (count > 0) {
        int r = rand() % count;
        int val = (rand() % 10 == 0) ? 4 : 2; // 10% de chance d'avoir 4
        grid[empty_cells[r][0]][empty_cells[r][1]] = val;
    }
}

int check_game_over(uint16_t grid[GRID_SIZE][GRID_SIZE]) {
    // Chercher une case vide
    for (int i = 0; i < GRID_SIZE; i++)
        for (int j = 0; j < GRID_SIZE; j++)
            if (grid[i][j] == 0) return 0;

    // Chercher une fusion possible
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (i < GRID_SIZE - 1 && grid[i][j] == grid[i + 1][j]) return 0;
            if (j < GRID_SIZE - 1 && grid[i][j] == grid[i][j + 1]) return 0;
        }
    }
    return 1; // Perdu
}
