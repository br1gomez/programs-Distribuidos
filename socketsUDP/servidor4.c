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

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;

    int reloj_cliente;
    int reloj_servidor = 0; 

    // Socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error al crear socket");
        exit(EXIT_FAILURE);
    }
    printf("SERVER: inciando reloj = %d\n", reloj_servidor);

    // Server address
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PUERTO);

    // Bind
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("SERVER: abriendo puerto: %d\n", PUERTO);

    // Client loop
    while (1) {
        cli_len = sizeof(cli_addr);

        // Waiting for petition it never ends so you can create multiple requests at the client side
        //Better solucion i found 
        recvfrom(sockfd, &reloj_cliente, sizeof(int), 0, (struct sockaddr*)&cli_addr, &cli_len);

        // Intern process
        printf("CLIENT: reloj = %d\n", reloj_cliente);
        reloj_servidor = max(reloj_servidor, reloj_cliente);
        reloj_servidor++; // Increment for receive in 1

        printf("RELOJ %d: Recibió petición actualizando reloj.\n", reloj_servidor);
        reloj_servidor++;
        printf("RELOJ %d: Procesando petición\n", reloj_servidor);//TO simule that it actually process something

        // Send responde is a process
        reloj_servidor++; // increment for send
        
        printf("RELOJ %d: Respondió.\n", reloj_servidor);
      
        sendto(sockfd, &reloj_servidor, sizeof(int), 0, (struct sockaddr*)&cli_addr, cli_len);
    }

    close(sockfd);
    return 0;
}