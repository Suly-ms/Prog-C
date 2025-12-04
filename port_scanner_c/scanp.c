#include "scanp.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

void printfp_open(int port) {
    printf("Open port : %d\n", port);
}

void printfp_close(int port) {
    printf("Close port : %d\n", port);
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        printf("Usage : %s IP PORT_START PORT_END\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *ip = argv[1];
    int port_start = atoi(argv[2]);
    int port_end = atoi(argv[3]);

    int sockfd, result;
    struct sockaddr_in sr_addr;

    sr_addr.sin_family = AF_INET;
    sr_addr.sin_addr.s_addr = inet_addr(ip);

    if (inet_pton(AF_INET, ip, &(sr_addr.sin_addr)) <= 0) {
        perror("Invalid IP address");
        exit(EXIT_FAILURE);
    }

    for(int i = port_start; i<port_end; i++) {
        sr_addr.sin_port = htons(i);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0) {
            perror("Socket creation failed");
            continue;
        }
        
        result = connect(sockfd, (struct sockaddr *)&sr_addr, sizeof(sr_addr));

        if(result == 0) {
            printfp_open(i);
        }
        /*
        else {
            printfp_close(i);
        }*/

        close(sockfd);
    }

    return EXIT_SUCCESS;
}
