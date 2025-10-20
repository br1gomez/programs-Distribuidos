#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *funcionHilo(void *args);

void main(void){
  pthread_t id_hilo;
  int arg[2]={2, 3};
  printf("Creación del hilo...\n");
  pthread_create(&id_hilo, NULL, funcionHilo, (void *)arg);
  printf("Hilo creado, esperando su finalización ...\n");
  pthread_join(id_hilo, NULL);
  printf("Hilo finalizado.\n valor 1: %i\n valor 2: %i\n", *arg, *(arg+1));
}

void *funcionHilo(void *argu){
  int *args = (int *) argu;
  printf("Valor 1: %i\n Valor 2: %i\n", *args, *(args+1));
  *args = 7;
  *(args+1) = 8;
  pthread_exit(NULL);
}