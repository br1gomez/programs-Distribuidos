#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#define PUERTO 3000
#define DIR_SERV "127.0.0.1"
#define TAM_MAX  1024

int main(){
  int desc_socket, tam_struct;
  struct sockaddr_in dest;
  char buffer[TAM_MAX];
  char *mem="Iniciando";

  /*Abre socket tipo flujo o STREAM*/
  if((desc_socket=socket(AF_INET,SOCK_DGRAM,0))<0){
    perror("Socket");
    exit(errno);
  }

  /*Inicio de puerto y dirección de servidor*/
  bzero(&dest,sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(PUERTO);
  if(inet_aton(DIR_SERV,&dest.sin_addr.s_addr)==0){
    perror(DIR_SERV);
    exit(errno);
  }

  /*Envia mensaje de inicio*/
  //sendto(desc_socket,mem,sizeof(mem)+1,0,(struct sockaddr*)&dest,sizeof(dest));
  //Correcion: 
  sendto(desc_socket,mem,strlen(mem)+1,0,(struct sockaddr*)&dest,sizeof(dest));
  /*Recibe mensaje*/
  bzero(buffer,TAM_MAX);
  tam_struct=sizeof(dest);
  recvfrom(desc_socket,buffer,sizeof(buffer),0,(struct sockaddr*)&dest,&tam_struct);
  printf("%s",buffer);

  /*Cierra conexión*/
  close(desc_socket);
  return 0;

}