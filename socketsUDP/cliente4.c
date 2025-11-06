#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PUERTO 3000

int max(int a, int b) {
    return (a > b) ? a : b;
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    
    int reloj_servidor;
    int reloj_cliente = 0; 

    if (argc != 2) {
        fprintf(stderr, "Se necesita la Ip del servidor\n");
        exit(EXIT_FAILURE);
    }
    char *ip_servidor = argv[1];

    // Socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("CLIENT: iniciando con Reloj = %d\n", reloj_cliente);

    // Server address
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PUERTO);
    if (inet_aton(ip_servidor, &serv_addr.sin_addr) == 0) {
        perror("IP");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send is a process so it increments
    reloj_cliente++; 
    
    printf("RELOJ %d: Enviando petición.\n", reloj_cliente);
    
    // Send timestamp to server
    sendto(sockfd, &reloj_cliente, sizeof(int), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // 5. Esperar la respuesta del servidor
    recvfrom(sockfd, &reloj_servidor, sizeof(int), 0, NULL, NULL);
    
    // Receiving data from server
    printf("SERVER: reloj = %d\n", reloj_servidor);
    reloj_cliente = max(reloj_cliente, reloj_servidor);
    reloj_cliente++;

    printf("RELOJ %d: actualizando reloj.\n", reloj_cliente);

    // Sincronization process
    reloj_cliente++; // Increments in 1 fotr sync
    printf("RELOJ %d: sincronizado.\n", reloj_cliente);

    printf("\nReloj final del cliente:  %d\n", reloj_cliente);

    close(sockfd);
    return 0;
}