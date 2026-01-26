#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <termios.h>
#include "../include/shared.h"

#define KEY_UP    65
#define KEY_DOWN  66
#define KEY_RIGHT 67
#define KEY_LEFT  68
#define KEY_Q     113

static int get_keypress(void) {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    
    int move = NONE;
    if (ch == KEY_Q) move = QUIT;
    else if (ch == 27) {
        getchar(); 
        switch(getchar()) {
            case KEY_UP:    move = UP; break;
            case KEY_DOWN:  move = DOWN; break;
            case KEY_RIGHT: move = RIGHT; break;
            case KEY_LEFT:  move = LEFT; break;
        }
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return move;
}

void run_input_process() {
    mkfifo(FIFO_NAME, 0666);
    int fd = open(FIFO_NAME, O_WRONLY);
    if (fd == -1) return;

    int move;
    while (1) {
        move = get_keypress();
        if (move != NONE) {
            write(fd, &move, sizeof(int));
            if (move == QUIT) break;
        }
    }
    close(fd);
}
