#include "move.h"

// Tasse une ligne vers la gauche
// Retourne 1 si mouvement, 0 sinon
static int slide_array(uint16_t *line) {
    int count = 0; 
    int moved = 0;
    
    for (int i = 0; i < GRID_SIZE; i++) {
        if (line[i] != 0) {
            if (i != count) {
                line[count] = line[i];
                line[i] = 0;
                moved = 1;
            }
            count++;
        }
    }
    return moved;
}

// Fusionne une ligne vers la gauche
// Retourne les points gagnés
static int merge_array(uint16_t *line) {
    int points = 0;
    for (int i = 0; i < GRID_SIZE - 1; i++) {
        if (line[i] != 0 && line[i] == line[i+1]) {
            line[i] *= 2;      
            line[i+1] = 0;     
            points += line[i]; 
            i++; 
        }
    }
    return points;
}

static void get_line(uint16_t grid[GRID_SIZE][GRID_SIZE], uint16_t *buffer, int index, int direction) {
    for (int i = 0; i < GRID_SIZE; i++) {
        switch(direction) {
            case LEFT:  buffer[i] = grid[index][i]; break;
            case RIGHT: buffer[i] = grid[index][GRID_SIZE - 1 - i]; break;
            case UP:    buffer[i] = grid[i][index]; break;
            case DOWN:  buffer[i] = grid[GRID_SIZE - 1 - i][index]; break;
        }
    }
}

static void set_line(uint16_t grid[GRID_SIZE][GRID_SIZE], uint16_t *buffer, int index, int direction) {
    for (int i = 0; i < GRID_SIZE; i++) {
        switch(direction) {
            case LEFT:  grid[index][i] = buffer[i]; break;
            case RIGHT: grid[index][GRID_SIZE - 1 - i] = buffer[i]; break;
            case UP:    grid[i][index] = buffer[i]; break;
            case DOWN:  grid[GRID_SIZE - 1 - i][index] = buffer[i]; break;
        }
    }
}

int apply_move(uint16_t grid[GRID_SIZE][GRID_SIZE], int direction) {
    int total_points = 0;
    int moved_global = 0;
    uint16_t buffer[GRID_SIZE];

    for (int i = 0; i < GRID_SIZE; i++) {
        get_line(grid, buffer, i, direction);

        int m1 = slide_array(buffer);
        int pts = merge_array(buffer);
        int m2 = slide_array(buffer);

        if (m1 || m2 || pts > 0) moved_global = 1;
        total_points += pts;

        set_line(grid, buffer, i, direction);
    }

    // Retourne les points si ça a bougé, sinon -1
    return moved_global ? total_points : -1;
}
