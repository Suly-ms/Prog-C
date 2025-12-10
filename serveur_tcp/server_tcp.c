#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>

#define BUF_SIZE 2048

const char *get_mime_type(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot) return "application/octet-stream";
    if (strcmp(dot, ".html") == 0) return "text/html";
    if (strcmp(dot, ".png") == 0) return "image/png";
    if (strcmp(dot, ".txt") == 0) return "text/plain";
    return "application/octet-stream";
}

void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    free(arg);
    pthread_detach(pthread_self());

    char msg_recu[BUF_SIZE];
    int bytes_received = recv(client_sock, msg_recu, BUF_SIZE - 1, 0);

    if (bytes_received <= 0) {
        close(client_sock);
        return NULL;
    }

    msg_recu[bytes_received] = '\0';
    
    char method[16], path[256], protocol[16];
    sscanf(msg_recu, "%s %s %s", method, path, protocol);

    char *filename = path + 1;
    if (strlen(filename) == 0) filename = "index.html";

    char header[BUF_SIZE];
    struct stat file_stat;
    const char *mime_type;
    long file_size;

    if (stat(filename, &file_stat) == -1) {
        snprintf(header, sizeof(header),
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 0\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n");
        if (send(client_sock, header, strlen(header), 0) == -1) {
            perror("send");
        }
    } 
    else if (access(filename, R_OK) == -1) {
        snprintf(header, sizeof(header),
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Length: 0\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n");
        if (send(client_sock, header, strlen(header), 0) == -1) {
            perror("send");
        }
    } 
    else {
        int file_fd = open(filename, O_RDONLY);
        mime_type = get_mime_type(filename);
        file_size = file_stat.st_size;

        snprintf(header, sizeof(header),
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: %ld\r\n"
            "Content-Type: %s\r\n"
            "Content-Disposition: inline; filename=\"%s\"\r\n"
            "\r\n",
            file_size, mime_type, filename);
        if (send(client_sock, header, strlen(header), 0) == -1) {
            perror("send");
        }

        int bytes_read;
        char file_buffer[1024];
        while ((bytes_read = read(file_fd, file_buffer, sizeof(file_buffer))) > 0) {
            send(client_sock, file_buffer, bytes_read, 0);
        }
        close(file_fd);
        printf(" -> 200 OK (%ld octets envoyés)\n", file_stat.st_size);
    }

    close(client_sock);
    return NULL;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Utilisation : %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints, *res;
    int sockfd, status;
    int yes = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(NULL, argv[1], &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        perror("socket");
        return 1;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        return 1;
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        return 1;
    }
    freeaddrinfo(res);

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        return 1;
    }

    printf("Listening on port %s...\n", argv[1]);

    while(1) {
        struct sockaddr_storage client_addr;
        socklen_t addr_size = sizeof(client_addr);
        
        int *new_sock = malloc(sizeof(int));
        if (!new_sock) continue;

        *new_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

        if (*new_sock == -1) {
            perror("accept");
            free(new_sock);
            continue;
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, new_sock) != 0) {
            perror("pthread_create");
            close(*new_sock);
            free(new_sock);
        }
    }

    close(sockfd);
    return 0;
}
