#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <pwd.h>

#define MAX_BUF_LEN 2048

int main(int argc, char *argv[]) {
    // TIMER POUR SAVOIR DEPUIS COMBIEN DE TEMPS LE PROGRAMME TOURNE
    time_t debut;
    time(&debut);

    if(argc != 2) {
        printf("Usage : port\n");
        exit(EXIT_FAILURE);
    }
    
    int status;
    const char* port = argv[1];

    char hostname[1024];
    char display_name[1024];

    hostname[1023] = '\0';
    gethostname(hostname, sizeof(hostname));

    struct addrinfo hints_name, *res_name;
    memset(&hints_name, 0, sizeof(hints_name));
    hints_name.ai_family = AF_INET;
    hints_name.ai_flags = AI_CANONNAME; // C'est ce flag qui récupère le vrai nom

    if (getaddrinfo(hostname, NULL, &hints_name, &res_name) == 0) {
        if (res_name->ai_canonname) {
            strncpy(display_name, res_name->ai_canonname, sizeof(display_name)-1);
        } else {
            strcpy(display_name, hostname);
        }
        freeaddrinfo(res_name);
    } else {
        strcpy(display_name, hostname);
    }

    struct addrinfo filtre;
    struct addrinfo *serviceinfo;

    memset(&filtre, 0 , sizeof(filtre));

    filtre.ai_socktype = SOCK_DGRAM;
    filtre.ai_family = AF_INET;
    filtre.ai_flags = AI_PASSIVE;

    if((status = getaddrinfo(NULL, port, &filtre, &serviceinfo)) !=0) {
        fprintf(stderr, "Error getaddrinfo : %s\n", gai_strerror(status));
        freeaddrinfo(serviceinfo);
        exit(EXIT_FAILURE);
    }

    int so = socket(serviceinfo->ai_family, 
            serviceinfo->ai_socktype,
            serviceinfo->ai_protocol);

    if(so < 0) {
        perror("Error socket");
        freeaddrinfo(serviceinfo);
        exit(EXIT_FAILURE);
    }
        
    // EVITER LE MESSAGE DE CON SUR LE BIND
    int yes=1;
    setsockopt(so, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    if(bind(so, serviceinfo->ai_addr, serviceinfo->ai_addrlen)==-1) {
        perror("Error bind");
        freeaddrinfo(serviceinfo);
        exit(EXIT_FAILURE);
    }
    while(1) {
        char buf[MAX_BUF_LEN];
        struct sockaddr_storage from;
        socklen_t len_from = sizeof(from);

        int recvf = recvfrom(so, buf, MAX_BUF_LEN-1, 0, (struct sockaddr *)&from, &len_from);

        if(recvf < 0) {
            perror("Error recvfrom");
            freeaddrinfo(serviceinfo);
            exit(EXIT_FAILURE);
        }
    
        char ip_from[INET_ADDRSTRLEN];
        struct sockaddr_in *from_in = (struct sockaddr_in *)&from;
        inet_ntop(AF_INET, &from_in->sin_addr, ip_from, INET_ADDRSTRLEN);

        printf("Message reçu de %s :\n%s\n", ip_from, buf);

        time_t fin;
        time(&fin);

        double sec_ecoule = difftime(fin, debut);
    
        char date_str[64];
        struct tm *info = localtime(&fin);
        strftime(date_str, sizeof(date_str), "%d/%m/%Y %H:%M", info);

        struct passwd *pws;
        pws = getpwuid(geteuid());
    
        char message_to_server[MAX_BUF_LEN];

        snprintf(message_to_server, MAX_BUF_LEN, 
            "Serveur servWTF, lancé depuis %.0f secondes par %s\n"
            "Vous êtes sur %s, IP/PORT\n"
            "Nous sommes le %s, vous retardez de NOMBRE SECONDE RETARDEMENTs.\n"
            "---------------\n"
            "Contenu du fichier /etc/motd :\n"
            ".................", 
            sec_ecoule, 
            pws->pw_name,
            display_name,
            date_str
        );

        sendto(so, message_to_server, strlen(message_to_server)+1, 0, (struct sockaddr*)&from, sizeof(from));
    }

    freeaddrinfo(serviceinfo);
    close(so);
    
    return EXIT_SUCCESS;
}
