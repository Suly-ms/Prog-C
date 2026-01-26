#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "include/shared.h"

// Prototypes (pour éviter d'avoir des headers .h pour les process)
void run_input_process();
void run_view_process(int fd);
void run_game_process(int fd);

int main() {
    int pipe_anon[2];
    if (pipe(pipe_anon) == -1) { perror("Pipe failed"); return 1; }

    // 1. Processus View
    if (fork() == 0) {
        close(pipe_anon[1]);
        run_view_process(pipe_anon[0]);
        exit(0);
    }

    // 2. Processus Game
    if (fork() == 0) {
        close(pipe_anon[0]);
        run_game_process(pipe_anon[1]);
        exit(0);
    }

    // 3. Processus Input (Père)
    close(pipe_anon[0]);
    close(pipe_anon[1]);
    
    // Petite pause pour laisser le temps aux fils de s'init
    usleep(100000); 
    
    run_input_process();

    wait(NULL);
    wait(NULL);
    
    // Nettoyage
    unlink(FIFO_NAME);
    printf("Programme terminé.\n");
    return 0;
}
