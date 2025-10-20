/*
Programe una aplicación que cree tres hilos. El primer hilo se encargará de contabilizar 
las ocurrencias de una cadena dentro de un archivo especifico y devolver el resultado al programa principal,
el segundo hilo copiará los archivos de su directorio actual a un subdirectorio que usted elija devolviendo
al programa principal el número de archivos copiados y el tercer hilo generará un archivo donde reportará los
resultados devueltos por los otros dos hilos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/dir.h>
#include <time.h>

//Funciones que realizarán los hilos
void *buscar(void *arg);
void *copiar(void *arg);
void *reporte(void *arg);

void directorio_archivo();

//Función principal
int main(){
  pthread_t hilo1, hilo2, hilo3;
  int count[2] = {0, 0}; // count[0] para ocurrencias, count[1] para archivos copiados
  //Crear directorio de copia y archivo de prueba
  directorio_archivo();
  printf("Creando hilos 1 y 2 que funionan en paralelo.\n");
  printf("======================================\n");
  //Creación de los hilos
  pthread_create(&hilo1, NULL, buscar, &count[0]);
  pthread_create(&hilo2, NULL, copiar, &count[1]);

  //Esperar a que los hilos 1 y 2 terminen
  pthread_join(hilo1, NULL);
  printf("*******Hilo 1 ha terminado la busqueda.\n");
  pthread_join(hilo2, NULL);
  printf("*******Hilo 2 ha terminado la copia.\n");
  printf("======================================\n");
  printf("Creando hilo 3 para generar el reporte.\n");
  //Creación del hilo 3 para generar el reporte
  pthread_create(&hilo3, NULL, reporte, count);
  //Esperar a que el hilo 3 termine
  pthread_join(hilo3, NULL);
  printf("Hilo 3 ha terminado. Programa finalizado y reporte completo.\n");
  printf("======================================\n");
  printf("Resultados finales:\n");
  printf(" - Ocurrencias encontradas de la cadena: %d\n", count[0]);
  printf(" - Archivos copiados al subdirectorio 'copia': %d\n", count[1]);
  return 0;
}


//Función base
void directorio_archivo(){
  if(mkdir("copia", 0777) == -1){
    perror("Error al crear el directorio de copia.\n");
    exit(EXIT_FAILURE);
  } printf("Directorio copiado de forma exitosa.\n");

  //Creando archivo de prueba
  FILE *arch_to_read = fopen("arch_to_read.txt", "w");
  if(arch_to_read){
    fprintf(arch_to_read, "Este es un archivo de prueba.\n");
    fprintf(arch_to_read, "Se contabilizara la palabra prueba.\n");
    fprintf(arch_to_read, "La palabra prueba aparece varias veces en este archivo de prueba.\n");
    fclose(arch_to_read);
  } else {
    perror("Error al crear el archivo de prueba.\n");
    exit(EXIT_FAILURE);
  }
}

//Función para el primer hilo: buscar ocurrencias de una cadena "prueba"
void *buscar(void *arg){
  FILE *archivo = fopen("arch_to_read.txt", "r");
  if(!archivo){
    perror("Hilo 1: Error al abrir el archivo para buscar.\n");
    pthread_exit(NULL);
  }else {
    char buffer[1000];
    char *cadena = "prueba";
    int *contador = (int *)arg;
    *contador = 0;
    printf("Hilo 1: Buscando ocurrencias de la cadena '%s' en el archivo.\n", cadena);
    while(fgets(buffer, sizeof(buffer), archivo)!= NULL){
      char *p = buffer;
      while((p = strstr(p, cadena)) != NULL){
        (*contador)++;
        p++;
      }
    }

    fclose(archivo);
    printf("Hilo 1: Encontradas %d ocurrencias de la cadena '%s'.\n", *contador, cadena);
    pthread_exit(NULL);
  }
}

//Función para el segundo hilo: copiar archivos al subdirectorio "copia"
void *copiar(void *arg){
  DIR *dir = opendir(".");
  struct stat arch_stat;
  int *copy = (int *)arg;
  *copy = 0;
  if(!dir){
    perror("Hilo 2: Error al abrir el directorio. \n");
    pthread_exit(NULL);
  }else {
    printf("Hilo 2: Copiando archivos al subdirectorio 'copia'.\n");
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
      stat (entry->d_name, &arch_stat);
      if(S_ISREG(arch_stat.st_mode) && strcmp(entry->d_name, "arch_to_read.txt") != 0 && strcmp(entry->d_name, "ejercicio5.c") != 0){
        char destino[256];
        snprintf(destino, sizeof(destino), "", entry->d_name);
        FILE *src = fopen(entry->d_name, "rb");
        FILE *dst = fopen(destino, "wb");
        if(src && dst){
          char buffer[1024];
          size_t bytes;
          while((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0){
            fwrite(buffer, 1, bytes, dst);
          }
          (*copy)++;
          fclose(src);
          fclose(dst);
        } 
      }  
    }
  }
  closedir(dir);
  printf("Hilo 2: Copiados %d archivos al subdirectorio 'copia'.\n", *copy);
  pthread_exit(NULL);
}

//Función para el tercer hilo: generar reporte
void *reporte(void *arg){
  FILE *reporte_arch = fopen("reporte.txt", "w");
  if(!reporte_arch){
    perror("Hilo 3: Error al crear el archivo de reporte.\n");
    pthread_exit(NULL);
  } else {
    int *resultados = (int *)arg;
    fprintf(reporte_arch, "REPORTE DE RESULTADOS\n");
    fprintf(reporte_arch, "RESULTADOS: \n\n");
    fprintf(reporte_arch, "1. Busqueda de la cadena:\n");
    fprintf(reporte_arch, " - Archivo analizado: arch_to_read.txt\n");
    fprintf(reporte_arch, " - Cadena buscada: prueba\n");
    fprintf(reporte_arch, " - Ocurrencias encontradas: %d\n\n", resultados[0]);
    fprintf(reporte_arch, "2. Copia de archivos:\n");
    fprintf(reporte_arch, " - Directorio origen: . (actual)\n");
    fprintf(reporte_arch, " - Directorio destino: copia/\n");
    fprintf(reporte_arch, " - Archivos copiados: %d\n\n", resultados[1]);
    fprintf(reporte_arch, "FIN DEL REPORTE\n");
    printf("Hilo 3: Reporte generado en 'reporte.txt'.\n");
    fclose(reporte_arch);
    pthread_exit(NULL);
  }
}