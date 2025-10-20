/*
Programe una aplicación que cree cinco procesos. Cada uno de estos procesos escribirá
en un archivo una palabra de la siguiente oración "Hola esta es mi practica uno". El proceso padre se encargará 
de escribir la uiltima palabra (uno)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void escribir_palabra(const char *nombre_archivo, const char *palabra) {
    FILE *archivo = fopen(nombre_archivo, "a");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }
    fprintf(archivo, "%s ", palabra);
    fclose(archivo);
}

int main(){
  const char* palabras[] = {"Hola", "esta", "es", "mi", "practica", "uno"};
  const char* nombre_archivo = "ejercicio5.txt";
  const int num_palabras = 6;
  pid_t pid;

  //Abriendo archivo en write para sobrescribir si ya existe
  FILE *archivo_inicial = fopen(nombre_archivo, "w");
  if (archivo_inicial == NULL){
    perror("Error al crear el archivo");
    exit(EXIT_FAILURE);
  }
  fclose(archivo_inicial);
  printf("Proceso padre (PID: %d) inicia la creación de procesos hijos.\n", getpid());

  for (int i = 0; i < num_palabras-1; i++) {
      pid = fork();
      if (pid < 0) {
          perror("Error al crear el proceso");
          exit(EXIT_FAILURE);
      } else if (pid == 0) {
          // Proceso hijo
          printf("Proceso hijo (PID: %d) escribiendo la palabra: %s\n", getpid(), palabras[i]);
          escribir_palabra(nombre_archivo, palabras[i]);
          exit(EXIT_SUCCESS);
      } else {
          // Proceso padre espera a que el hijo termine antes de crear el siguiente
          wait(NULL);
      }
  }
  printf("Proceso padre (PID: %d) escribe la ultima palabra: %s\n", getpid(), palabras[num_palabras - 1]);
  escribir_palabra(nombre_archivo, palabras[num_palabras - 1]);

  printf("Todos los procesos han terminado. Verificar el archivo %s.\n", nombre_archivo);


  return 0;
}