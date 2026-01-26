#include <stdio.h>
#include <unistd.h>
#include "../include/shared.h"
#include "../algos/algos.h"
#include "../display/display_ligne.h"

void run_view_process(int pipe_read_fd) {
    game_state state;
    game_data data_wrapper;

    printf("\033[H\033[J"); 

    while (read(pipe_read_fd, &state, sizeof(game_state)) > 0) {
        if (state.game_over) {
            printf("\n--- GAME OVER ---\n");
            break;
        }
        
        data_wrapper.score = state.score;
        for(int i=0; i<GRID_SIZE; i++)
            for(int j=0; j<GRID_SIZE; j++)
                data_wrapper.grid[i][j] = state.grid[i][j];

        printf("\033[H"); 
        display_game(&data_wrapper); 
        printf("Commandes : Flèches pour jouer, 'q' pour quitter.\n");
    }
    close(pipe_read_fd);
}
