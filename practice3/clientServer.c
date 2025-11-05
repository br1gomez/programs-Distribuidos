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

void modo_cliente(char *ip_servidor){
  printf("Programa en modo cliente\n");
  int desc_socket, tam_struct;
  struct sockaddr_in dest;
  char buffer[TAM_MAX];
  char *men = "Saludos desde el cliente";

  printf("CLIENT: Inciando cliente UDP.\n");
  printf("CLIENT: Conectando al servidor: %s\n", ip_servidor);

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
  if(inet_aton(ip_servidor, &dest.sin_addr) == 0){
    perror(ip_servidor);
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

}

void modo_servidor(){
  printf("Programa en modo servidor\n");
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
}

int main(int argc, char *argv[]){
  if(argc == 2 && strcmp(argv[1], "server") == 0){
    modo_servidor();
  } else if (argc == 3 && strcmp(argv[1], "client") == 0){
    char *ip_servidor = argv[2];
    modo_cliente(ip_servidor);
  }else {
    fprintf(stderr, "Modo de uso:\n");
    fprintf(stderr, " Para modo servidor: %s server\n", argv[0]);
    fprintf(stderr, " Para modo cliente: %s client ipServidor\n", argv[0]);
  }
  return 0;
}

