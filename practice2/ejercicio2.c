#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *funcionHilo(void *args);
void main(void){
  pthread_t id_hilo;
  printf("Creación del hilo...\n");
  pthread_create(&id_hilo, NULL, funcionHilo, NULL);
  printf("Hilo creado, esperando su finalización ...\n");
  pthread_join(id_hilo, NULL);
  printf("Hilo finalizado.\n");
}

void *funcionHilo(void *args){
  printf("Dentro del hilo...\n");
  pthread_exit(NULL);
}