/*
Programar una aplicacicón que cree un proceso hijo a partir de un proceso padre,
el hijo creado a su vez creará tres hilos. Cada uno de los tres hilos creará dos hilos más. 
Cada uno de los hilos creados imprimirá en pantalla "Hola mundo" si se trata de un hilo terminal o los idenitificadores
de los hilos creados si se trata de un proceso o hilo padre
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>

//Funciones que ejecutarán los hilos
void *hilo_nivel2(void *arg);
void *hilo_terminal(void *arg);

int main(){
  pid_t pid_hijo;
  printf("Proceso padre con PID: %d\n", getpid());
  //Creación del proceso hijo
  printf("Creando proceso hijo.\n");
  pid_hijo = fork();

  if(pid_hijo < 0){
    perror("Error al crear el proceso hijo.\n");
    exit(EXIT_FAILURE);
  }else if(pid_hijo == 0){
    //Código del proceso hijo
    printf("Soy el proceso hijo con PID: %d y el de mi padre es: %d\n", getpid(), getppid());
    //Creación de los hilos en el proceso hijo
    pthread_t hilos[3];
    for(int i = 0; i < 3; i++){
      pthread_create(&hilos[i], NULL, hilo_nivel2, NULL);
    }
    printf("Soy el proceso hijo y cree 3 hilos con IDs:\n");
    for(int i = 0; i < 3; i++){
      printf("Hilo %d: %lu\n", i, hilos[i]);
    }
    //Esperar a que terminen los hilos de nivel 2
    for(int i = 0; i < 3; i++){
      pthread_join(hilos[i], NULL);
    }
    printf("Proceso hijo finalizado.\n");
    exit(EXIT_SUCCESS);
  }else{
    //Código del proceso padre
    printf("Soy el proceso padre y cree un hijo con PID: %d\n", pid_hijo);
    //Esperar a que el proceso hijo termine
    wait(NULL);
    printf("Mi hijo ha terminado. Proceso padre finalizado.\n");
    exit(EXIT_SUCCESS);
  }
  return 0;
}

void *hilo_nivel2(void *arg){
  printf("Hilo de nivel 2 con ID: %lu creado por el proceso PID: %d\n", pthread_self(), getpid());
  //Creación de los hilos terminales
  pthread_t hilos_terminales[2];
  for(int i = 0; i < 2; i++){
    pthread_create(&hilos_terminales[i], NULL, hilo_terminal, NULL);
  }

  printf("Hilo de nivel 2 con ID: %lu creo 2 hilos terminales con IDs:\n", pthread_self());
  for(int i = 0; i < 2; i++){
    printf("Hilo terminal %d: %lu\n", i, hilos_terminales[i]);
  }
  //Esperar a que terminen los hilos terminales
  for(int i = 0; i < 2; i++){
    pthread_join(hilos_terminales[i], NULL);
  }
  return NULL;
}

void *hilo_terminal(void *arg){
  printf("Hilo terminal (%d) %lu: Hola mundo\n", getpid(), pthread_self());
  return NULL;
}
