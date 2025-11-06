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
    int sock_fd, new_sock;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    char buffer[BUFFER_SIZE];
    char filename[256];
    char save_path[512];
    ssize_t n_bytes;
    FILE *received_file;

    // SOCK_STREAM para TCP
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("SERVER: Iniciando modo TCP en el puerto %d\n", PORT);

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

    // Listen 
    listen(sock_fd, 5);
    printf("SERVER TCP: Escuchando conexiones\n");

    // Accept 
    new_sock = accept(sock_fd, (struct sockaddr *)&cli_addr, &cli_len);
    if (new_sock < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    
    printf("SERVER TCP: Cliente conectado desde %s\n", inet_ntoa(cli_addr.sin_addr));
    
    // Create dir where to save uploads
    mkdir("uploads", 0755);

    // archive name
    bzero(filename, 256);
    recv(new_sock, filename, 255, 0);
    sprintf(save_path, "uploads/%s", filename);

    // Write permission 
    printf("SERVER: Solicitando permisos de escritura en: %s\n", save_path);
    received_file = fopen(save_path, "wb"); //To wirte in binary
    
    if (received_file == NULL) {
        perror("Abrir archivo en servidor");
        send(new_sock, "ERR", 3, 0); 
    } else {
        send(new_sock, "OK", 2, 0);
        printf("SERVER: Tienes permisos.\n");
        while ((n_bytes = recv(new_sock, buffer, BUFFER_SIZE, 0)) > 0) {
            fwrite(buffer, 1, n_bytes, received_file);
        }
        // if n_bytes == 0 transfer is complete 

        printf("SERVER TCP: Transferencia completada.\n");
        fclose(received_file);
    }

    close(new_sock); 
    close(sock_fd);  
    return 0;
}