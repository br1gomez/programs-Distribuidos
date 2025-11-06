#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>      
#include <arpa/inet.h>   
#include <sys/socket.h>

#define PORT 3000
#define MAX_NUMBERS 50   

typedef struct {
    int numbers[MAX_NUMBERS];
    int count;
    char operation[5]; // "sum", "sub", "mul", "div"
} CalcPetition;

typedef struct {
    double result;
    char error[100];
} CalcResponse;


int main() {
    int sock_fd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;

    CalcPetition request;
    CalcResponse response;

    // Socket With Datagram.
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Server address
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; 
    serv_addr.sin_port = htons(PORT);

    // Bind
    if (bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("SERVER: Calculadora UDP en el puerto %d\n", PORT);

    // Server loop
    while (1) {
        cli_len = sizeof(cli_addr);

        // Waiting for a request
        printf("\nEsperando una petición\n");
        if (recvfrom(sock_fd, &request, sizeof(CalcPetition), 0,
                     (struct sockaddr*)&cli_addr, &cli_len) < 0) {
            perror("recvfrom");
            continue; // Go to next iteration
        }

        printf("Petición recibida de %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
        printf("Operación: %s, Números: %d\n", request.operation, request.count);
        bzero(&response, sizeof(CalcResponse));
        
        if (request.count == 0) {
            strcpy(response.error, "Error: No se enviaron números.");
        } else {
            // First number
            double res = request.numbers[0];
            int error = 0; 
            for (int i = 1; i < request.count; i++) {
                if (strcmp(request.operation, "sum") == 0) {
                    res += request.numbers[i];
                } else if (strcmp(request.operation, "sub") == 0) {
                    res -= request.numbers[i];
                } else if (strcmp(request.operation, "mul") == 0) {
                    res *= request.numbers[i];
                } else if (strcmp(request.operation, "div") == 0) {
                    if (request.numbers[i] == 0) {
                        strcpy(response.error, "Error: División por cero.");
                        error = 1;
                        break;
                    }
                    res /= request.numbers[i];
                }
            }
            if (!error && strcmp(response.error, "") == 0) {
                strcpy(response.error, "OK");
                response.result = res;
        }
        
        // Respond to client
        if (sendto(sock_fd, &response, sizeof(CalcResponse), 0,
                   (struct sockaddr*)&cli_addr, cli_len) < 0) {
            perror("sendto");
        }

        printf("SERVER: Respuesta enviada, resultado: %f\n", response.result);
    }

    close(sock_fd);
    return 0;
}}