#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <string.h>
#include <resolv.h>

#define PUERTO 3000
#define TAM_MAX 1024

int main(){
  int desc_socket, tam_dest;
  struct sockaddr_in direc, dest;
  char *buffer = "Saludos a todos desde el servidor\n";
  char men[TAM_MAX];
  printf("Inicio\n");
  desc_socket = socket(AF_INET, SOCK_DGRAM,0);
  bzero(&direc,sizeof(direc));
  direc.sin_family = AF_INET;
  direc.sin_port = htons(PUERTO);
  direc.sin_addr.s_addr = INADDR_ANY;
  bind(desc_socket, (struct sockaddr*)&direc, sizeof(direc));
  tam_dest = sizeof(dest);
  //Se cambia strlen por sizeof para recibir el tamaño del buffer
  recvfrom(desc_socket, men, sizeof(men)+1,0,(struct sockaddr*)&dest, &tam_dest);
  printf("Conectando...\n");
  sendto(desc_socket,buffer,strlen(buffer)+1,0,(struct sockaddr*)&dest,sizeof(dest));
  close(desc_socket);
  return 0;
}