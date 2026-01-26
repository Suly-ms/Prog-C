#include "game.h"

static thread_context ctx;

// thread 1: reçoit les inputs
void* t_main(void* arg) {
    (void)arg;
    int fd = open(FIFO_NAME, O_RDONLY);
    int cmd;
    
    add_random_tile(ctx.current_state.grid);
    add_random_tile(ctx.current_state.grid);

    ctx.has_new_state = 1;

    while (ctx.running && read(fd, &cmd, sizeof(int)) > 0) {
        if (cmd == QUIT) {
            ctx.running = 0;
            break;
        }
        
        // c'est au cas où t_move est déjà occupé, on attend qu'il finisse pour ne pas écraser la commande précédente trop vite.
        while (ctx.has_new_input || ctx.processing_move) {
            if (!ctx.running) break;
            usleep(1000);         }

        if (!ctx.running) break;
        ctx.current_state.command = cmd;
        ctx.has_new_input = 1; 
    }
    close(fd);
    return NULL;
}

// thread 2: logique du jeu
void* t_move(void* arg) {
    (void)arg;
    while (ctx.running) {
        while (!ctx.has_new_input) {
            if (!ctx.running) break;
            usleep(1000); 
        }

        if (!ctx.running) break;

        // On signale qu'on bosse
        ctx.processing_move = 1;
        
        // On "consomme" l'input, on baisse le drapeau
        ctx.has_new_input = 0; 

        int points = apply_move(ctx.current_state.grid, ctx.current_state.command);
        if (points != -1) {
            ctx.current_state.score += points;
            add_random_tile(ctx.current_state.grid);
        }
        
        // On a fini le calcul
        ctx.processing_move = 0;

        // On signale à t_goal qu'il peut afficher
        ctx.has_new_state = 1;
    }
    return NULL;
}

static int check_win(uint16_t grid[GRID_SIZE][GRID_SIZE]) {
    for(int i=0; i<GRID_SIZE; i++)
        for(int j=0; j<GRID_SIZE; j++)
            if (grid[i][j] == 2048) return 1;
    return 0;
}

// thread 3: vérif et envoi sur l'affichage
void* t_goal(void* arg) {
    (void)arg;
    while (ctx.running) {
        // c'est pour attendre que t_move (ou t_main) nous dise "c'est prêt"
        while (!ctx.has_new_state) {
            if (!ctx.running) break;
            usleep(1000);
        }

        if (!ctx.running) break;

        // Logique pour la fin de jeu
        if (check_win(ctx.current_state.grid)) {
             printf("VICTOIRE ! VOUS AVEZ FAIT 2048 !\n");
             ctx.current_state.game_over = 1;
             ctx.running = 0;
        }

        if (check_game_over(ctx.current_state.grid)) {
            ctx.current_state.game_over = 1;
            ctx.running = 0;
        }

        // Envoi vers l'affichage
        write(ctx.pipe_write_fd, &ctx.current_state, sizeof(game_state));
        
        // On a fini de traiter cet état, on baisse le drapeau
        ctx.has_new_state = 0;
    }
    return NULL;
}

void run_game_process(int pipe_write_fd) {
    srand(time(NULL));
    ctx.pipe_write_fd = pipe_write_fd;
    ctx.running = 1;
    
    ctx.has_new_input = 0;
    ctx.has_new_state = 0;
    ctx.processing_move = 0;

    ctx.current_state.score = 0;
    
    pthread_t t1, t2, t3;
    pthread_create(&t1, NULL, t_main, NULL);
    pthread_create(&t2, NULL, t_move, NULL);
    pthread_create(&t3, NULL, t_goal, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL); 
    pthread_join(t3, NULL);
    
    close(pipe_write_fd);
}
