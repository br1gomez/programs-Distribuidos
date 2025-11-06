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


// Give Ip and operation with numbers
int main(int argc, char *argv[]) {
    int sock_fd;
    struct sockaddr_in serv_addr;
    CalcPetition request;
    CalcResponse response;

    if (argc < 5) {
        fprintf(stderr, "Se debe de colocar la IP, operacion sum, sub, mul o div y los numeros\n");
        exit(EXIT_FAILURE);
    }
    
    char *ip_servidor = argv[1];
    char *op = argv[2];
    int num_count = argc - 3;

    if (num_count > MAX_NUMBERS) {
        fprintf(stderr, "Error: maximo de numeros es %d\n", MAX_NUMBERS);
        exit(EXIT_FAILURE);
    }
    
    //Scoket UDP
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Server address
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_aton(ip_servidor, &serv_addr.sin_addr) == 0) {
        perror("IP");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Structure of petition
    bzero(&request, sizeof(CalcPetition));
    strcpy(request.operation, op);
    request.count = num_count;
    
    printf("Enviando petición de '%s' con %d números: ", op, num_count);
    for (int i = 0; i < num_count; i++) {
        request.numbers[i] = atoi(argv[i + 3]);
        printf("%d ", request.numbers[i]);
    }
    printf("\n");

    // Full peittion send
    if (sendto(sock_fd, &request, sizeof(CalcPetition), 0,
               (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("sendto");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    
    // Waitinf for respone
    bzero(&response, sizeof(CalcResponse));
    if (recvfrom(sock_fd, &response, sizeof(CalcResponse), 0, NULL, NULL) < 0) {
        perror("recvfrom");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Result
    if (strcmp(response.error, "OK") == 0) {
        printf("\nSERVER:\n");
        printf("Resultado: %f\n", response.result);
    } else {
        fprintf(stderr, "\nSERVER: ERROR\n");
        fprintf(stderr, "%s\n", response.error);
    }

    close(sock_fd);
    return 0;
}