#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 3000
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sock_fd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    ssize_t n_bytes;
    char *ip_servidor, *filename;

    if (argc != 3) {
        fprintf(stderr, "Se necesita la ip del server y el nombre del archivo\n");
        exit(EXIT_FAILURE);
    }
    ip_servidor = argv[1];
    filename = argv[2];

    // Open local file
    FILE *file_to_send = fopen(filename, "rb"); //to read binary
    if (file_to_send == NULL) {
        fprintf(stderr, "CLIENT: no se puede abrir el archivo local '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    // SOCK_STREAM para TCP
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    printf("CLIENT: Iniciando modo TCP\n");

    // Server address
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_aton(ip_servidor, &serv_addr.sin_addr) == 0) {
        fprintf(stderr, "CLIENT: ERROR: Dirección IP inválida '%s'\n", ip_servidor);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    
    printf("CLIENT TCP: Conectado al servidor %s\n", ip_servidor);

    // Filename send
    send(sock_fd, filename, strlen(filename) + 1, 0);

    // Asking for write permission
    bzero(buffer, BUFFER_SIZE);
    recv(sock_fd, buffer, BUFFER_SIZE, 0);

    if (strcmp(buffer, "OK") != 0) {
        fprintf(stderr, "CLIENT: Sin permisos, cerrando\n");
    } else {
        printf("CLIENT: Permiso OK. Enviando archivo %s \n", filename);

        // Sending file
        while ((n_bytes = fread(buffer, 1, BUFFER_SIZE, file_to_send)) > 0) {
            if (send(sock_fd, buffer, n_bytes, 0) < 0) {
                perror("CLIENT: ERROR enviando archivo");
                break;
            }
        }
        printf("CLIENT TCP: Transferencia de archivo completada.\n");
    }

    fclose(file_to_send);
    close(sock_fd); 
    return 0;
}