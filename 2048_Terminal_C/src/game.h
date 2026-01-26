#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include "../include/shared.h"
#include "../algos/algos.h"
#include "../algos/move.h"

typedef struct {
    game_state current_state;
    int pipe_write_fd;
    
    volatile int has_new_input;  
    volatile int has_new_state;      
    volatile int processing_move;    
    volatile int running;        
} thread_context;
