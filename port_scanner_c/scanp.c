#include <string.h>
#include "scanp.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>

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
    int sockfd, status;
    struct addrinfo filtre, *serviceinfo;

    memset(&filtre, 0, sizeof(filtre));
    filtre.ai_family = AF_UNSPEC;
    filtre.ai_socktype = SOCK_STREAM;

    for(int i = port_start; i<port_end; i++) {
        char i_str[16];
        snprintf(i_str, sizeof(i_str), "%d", i);
        if ((status = getaddrinfo(ip, i_str, &filtre, &serviceinfo)) != 0) {
            fprintf(stderr, "gai error: %s\n", gai_strerror(status));
            exit(EXIT_FAILURE);
        }

        if ((sockfd = socket(serviceinfo->ai_family, serviceinfo->ai_socktype, 0)) == -1) {
            perror("Socket creation failed");
            continue;
        }
        
        if(connect(sockfd, serviceinfo->ai_addr, serviceinfo->ai_addrlen)==0) {
            printfp_open(i);
        }

        close(sockfd);

        freeaddrinfo(serviceinfo);
    }

    return EXIT_SUCCESS;
}
