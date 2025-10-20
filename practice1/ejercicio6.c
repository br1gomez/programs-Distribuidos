/*
Ejercicio 6: 
Simule una red de anillo unidireccional a traves de procesos. Utilice la línea de comando para indicar el 
numero de procesos que se crearán sobre el anillo. Despues de la creación del anillo, 
cada procesos se identifica con un ID de proceso y el ID del proceso padre. Cada proceso pasará un token
y cuando el token llegue a cada nodo verificará si el mensaje contenido en el token es para él, si el 
mensaje es para ese proceso imprimirá el mensaje en la pantalla, en otro caso lo pasará al siguiente proceso.
El mensaje así como el formato del token es a su elección. 
Utilice tuberías como aproximación de solución para simular los enlaces de comunicación.
Asuma tres ciclos de recorrido del token y entonces concluirá la aplicación.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

//Estructura del token
typedef struct{
  int id_destino;
  int vueltas_restantes;
  char mensaje[100];
} Token;

void nodo_proceso(int id, int num_procesos, int pipes[][2]);

int main(int argc, char *argv[]){
  if(argc != 2){
    fprintf(stderr, " %s Necesita argumentos\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  int num_procesos = atoi(argv[1]);
  if (num_procesos <= 1){
    fprintf(stderr, "El numero de procesos debe ser mayor que 1.\n");
    exit(EXIT_FAILURE);
  }

  printf("Padre (PID: %d) creando %d procesos en el anillo.\n", getpid(), num_procesos);

  // Crear tuberías
  int pipes[num_procesos][2];
  for (int i = 0; i < num_procesos; i++){
    if(pipe(pipes[i]) == -1){
      perror("Error al crear la tubería");
      exit(EXIT_FAILURE);
    }
  }

  // Crear procesos
  for (int i = 0; i < num_procesos; i++){
    pid_t pid = fork();
    if (pid < 0){
      perror("Error al crear el proceso");
      exit(EXIT_FAILURE);
    } else if (pid == 0){
      // Proceso hijo
      nodo_proceso(i, num_procesos, pipes);
      exit(EXIT_SUCCESS);
    }
  }

  // Padre cierra tuberias que no usa
  for (int i = 0; i < num_procesos; i++){
    close(pipes[i][0]);
    close(pipes[i][1]);
  }

  // Esperar a que todos los procesos hijos terminen
  for(int i = 0; i < num_procesos; i++){
    wait(NULL);
  }

  printf("Todos los procesos hijos han terminado.\n");

  return 0;
}

void nodo_proceso(int id, int num_procesos, int pipes[][2]){
  //cada nodo id escribe en pipes[id] y lee de pipes[id-1]
  int read_pipe = pipes[(id + num_procesos - 1) % num_procesos][0];
  int write_pipe = pipes[id][1];

  // Cerrar extremos no usados de las tuberías
  for (int i = 0; i < num_procesos; i++){
    if (i != id){
      close(pipes[i][1]);
    } if (i != (id + num_procesos - 1) % num_procesos){
      close(pipes[i][0]);
    }
  }

  printf("Nodo %d creado (PID: %d, PPID: %d)\n", id, getpid(), getppid());
  fflush(stdout);

  Token token;
  //El nodo 0 inicia el token
  if(id == 0){
    int destino = 2; //Por ejemplo, el mensaje es para el nodo 2
    token.id_destino = destino; 
    token.vueltas_restantes = 3; //Tres ciclos
    strcpy(token.mensaje, "Te envian mensaje desde el nodo 0");
    printf("Nodo %d enviando token inicial al nodo %d, vueltas restantes: %d\n", id, token.id_destino, token.vueltas_restantes);
    write(write_pipe, &token, sizeof(Token));
  }
  // Ciclo de procesamiento del token
  while (read(read_pipe, &token, sizeof(Token)) > 0){
    printf("Nodo %d recibio token (Destino: %d, Vueltas restantes: %d, Mensaje: %s)\n", id, token.id_destino, token.vueltas_restantes, token.mensaje);
    fflush(stdout);

    // Verificar si el mensaje es para este nodo
    if(token.id_destino == id){
      printf("Nodo %d: Mensaje recibido: %s\n", id, token.mensaje);
      fflush(stdout);
    }

    // Decrementar vueltas si el nodo 0 recibe el token
    if(id == 0){
      token.vueltas_restantes--;
      if(token.vueltas_restantes == 0){
        printf("\nNodo %d: El token ha completado todas las vueltas. Terminando proceso.\n", id);
        fflush(stdout);
        break;
      } printf("\n***Nodo %d: El token completó un ciclo, vueltas restantes: %d\n", id, token.vueltas_restantes);
    }

    //En caso de que finalice
    if(token.vueltas_restantes <= 0){
      printf("Nodo %d: Terminando proceso.\n", id);
      write(write_pipe, &token, sizeof(Token));
      break;
    } write(write_pipe, &token, sizeof(Token));
  }
  // Cerrar tuberías
  close(read_pipe);
  close(write_pipe);

}