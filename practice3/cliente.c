#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <unistd.h>

#define PORT 3000
#define TAM_MAX 1024

int main(int argc, char *argv[]){//Recibe la IP del servidor como argumento
  int desc_socket, tam_struct;
  struct sockaddr_in dest;
  char buffer[TAM_MAX];
  char *men = "Saludos desde el cliente";

  if(argc != 2){
    fprintf(stderr, "Se debe colocar la IP del servidor como argumento\n");
    exit(EXIT_FAILURE);
  }
  printf("CLIENT: Inciando cliente UDP.\n");
  printf("CLIENT: Conectando al servidor: %s\n", argv[1]);

  //UDP socket
  desc_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if(desc_socket < 0){
    perror("socket");
    exit(errno);
  }
  //Server Address 
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(PORT);
  if(inet_aton(argv[1], &dest.sin_addr) == 0){
    perror(argv[1]);
    exit(errno);
  }
  //Send message
  sendto(desc_socket, men, strlen(men)+1,0,(struct sockaddr*)&dest, sizeof(dest));
  printf("CLIENT: mensaje enviado al server\n");

  //Reply from server 
  bzero(buffer, TAM_MAX);
  tam_struct = sizeof(dest);
  recvfrom(desc_socket, buffer, sizeof(buffer),0,(struct sockaddr*)&dest, (socklen_t*)&tam_struct);
  printf("CLIENT: mensaje recibido: %s\n", buffer);
  close(desc_socket);
  return 0;
}
