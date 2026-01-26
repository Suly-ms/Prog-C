#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>
#include <stddef.h>

#define GRID_SIZE 4
#define FIFO_NAME "/tmp/2048_input_fifo"

// Commandes possibles
enum MOVEMENT { RIGHT, LEFT, UP, DOWN, QUIT, NONE };

typedef struct {
    uint16_t grid[GRID_SIZE][GRID_SIZE];
    size_t score;
    int command;  
    int game_over;  // 1 si perdu
} game_state;

#endif
