#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 3000
#define TAM_MAX 1024

int main(){
  int desc_socket, tam_dest;
  struct sockaddr_in direc, dest; //Server & client IP addresses
  char *buffer = "Saludos desde el servidor";
  char men[TAM_MAX];
  ssize_t n_bytes;

  printf("SERVER: Iniciando servidor UDP en el puerto %d\n", PORT);

  //UDP socket
  desc_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if(desc_socket < 0){
    perror("socket");
    exit(errno);
  }

  //Server Address structure
  bzero(&direc, sizeof(direc));
  direc.sin_family = AF_INET;
  direc.sin_port = htons(PORT);
  direc.sin_addr.s_addr = INADDR_ANY;

  //Bind
  if(bind(desc_socket, (struct sockaddr*)&direc, sizeof(direc)) <0){
    perror("bind");
    close(desc_socket);
    exit(errno);
  }
  printf("SERVER: esperando un mensaje.\n");
  tam_dest = sizeof(dest);
  n_bytes = recvfrom(desc_socket, men, sizeof(men), 0, (struct sockaddr*)&dest,(socklen_t*)&tam_dest);

  if(n_bytes < 0){
    perror("recvfrom");
    close(desc_socket);
    exit(errno);
  } else {
    men[n_bytes] = '\0';
    printf("SERVER: mensaje recibido: %s desde %s:%d\n", 
      men, 
      inet_ntoa(dest.sin_addr),
      ntohs(dest.sin_port));
  }
  //Reply to client
  sendto(desc_socket, buffer, strlen(buffer) +1, 0, (struct sockaddr*)&dest, sizeof(dest));
  printf("SERVER: mensaje enviado, cerrando el servidor. \n");
  close(desc_socket);
  return 0;
}
