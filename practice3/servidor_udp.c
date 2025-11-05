#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>

#define PORT 3000
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    char buffer[BUFFER_SIZE];
    char filename[256];
    char save_path[512];
    ssize_t n_bytes;
    FILE *received_file;

    // SOCK_DGRAM para UDP
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("SERVER: Iniciando modo UDP en el puerto %d\n", PORT);

    // Server address
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Bind
    if (bind(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    
    mkdir("uploads", 0755);

    // File name receive
    printf("SERVER UDP: Recibiendo nombre de archivo\n");
    n_bytes = recvfrom(sock_fd, filename, 255, 0, (struct sockaddr *)&cli_addr, &cli_len);
    filename[n_bytes] = '\0';
    sprintf(save_path, "uploads/%s", filename);

    // Write permission 
    printf("SERVER: Solicitando permisos de escritura: %s\n", save_path);
    received_file = fopen(save_path, "wb");

    if (received_file == NULL) {
        perror("permisos");
        sendto(sock_fd, "ERR", 3, 0, (struct sockaddr *)&cli_addr, cli_len); // Enviar Error
    } else {
        // Give permisison
        sendto(sock_fd, "OK", 2, 0, (struct sockaddr *)&cli_addr, cli_len);
        printf("SERVER: tienes permisos\n");

        // File receive
        while (1) {
            n_bytes = recvfrom(sock_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&cli_addr, &cli_len);
            if (strncmp(buffer, "EOF_UDP_TRANSFER", 16) == 0) {
                break; // upload finished
            }
            fwrite(buffer, 1, n_bytes, received_file);
        }
        printf("SERVER UDP: Transferencia completada.\n");
        fclose(received_file);
    }
    
    close(sock_fd);
    return 0;
}