#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

#define LENBUF 1024

void gestionnaire_alarme() {
    printf("\nAlarme déclenché car aucun message reçu\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    if(argc != 3) {
        printf("Usage : %s ip port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    sa.sa_handler = gestionnaire_alarme;

    char hostname[1024];
    hostname[1023] = '\0';
    
    if (gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname");
        exit(EXIT_FAILURE);
    }
    
    printf("Nom : %s\n", hostname);

    if(sigaction(SIGALRM, &sa, NULL) == -1) {
        perror("Erreur configuration signal");
        exit(EXIT_FAILURE);
    }
    
    int status;
    struct addrinfo filtre;
    struct addrinfo *serviceinfo;
    
    const char* ip = argv[1];
    const char* port = argv[2];

    int so;
    serviceinfo = NULL;
    memset(&filtre, 0, sizeof(filtre));
    
    filtre.ai_family = AF_INET;
    filtre.ai_socktype = SOCK_DGRAM;
    
    if((status = getaddrinfo(ip, port, &filtre, &serviceinfo)) != 0) {
        fprintf(stderr, "%s : machine nom déclarée\n", ip);
        exit(EXIT_FAILURE);
    }

    so = socket(serviceinfo->ai_family,
            serviceinfo->ai_socktype,
            serviceinfo->ai_protocol);
    
    if(so < 0) {
        perror("Problème création socket client");
        freeaddrinfo(serviceinfo);
        exit(EXIT_FAILURE);
    }
    
    // RECUPERATION DE L'HEURE ACTUELLE
    time_t heure;
    time(&heure);
    char msg[LENBUF];

    struct tm *info = localtime(&heure);
    strftime(msg, LENBUF, "%H:%M:%S", info);    
    sendto(so, msg, strlen(msg)+1, 0, serviceinfo->ai_addr, serviceinfo->ai_addrlen);
    
    char buf[LENBUF];
    socklen_t len_from;
    struct sockaddr_storage from;
    
    len_from = sizeof(from);
    
    // ACTIVE L'ALARME DE 5sec
    alarm(5);

    ssize_t recvf = recvfrom(so, buf, LENBUF-1, 0, (struct sockaddr *)&from, &len_from);
    
    // DESACTIVE L'ALARME SI ON A PU ENVOYER LE MESSAGE
    alarm(0);

    if(recvf < 0) {
        perror("Error recvfrom");
        freeaddrinfo(serviceinfo);
        exit(EXIT_FAILURE);
    }

    buf[recvf] = '\0'; 

    printf("Message du serveur :\n%s\n", buf);

    freeaddrinfo(serviceinfo);
    close(so);

    return EXIT_SUCCESS;
}
