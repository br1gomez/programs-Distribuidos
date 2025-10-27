/*Programa para crear un proceso hijo a partir de un proceso padre
el hijo creado a su vez creara tres procesos hijos mas
a su vez cada uno de los tres procesos creara dos procesos mas, cada uno de los procesos
imprimira en pantalla el pid de su padre si se trata de un hijo terminal o los pids de sus hijos creados si se trata de un 
proceso padre*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    pid_t pid_p1;
    int i;

    printf("NIVEL 0: Soy el proceso padre principal con PID %d \n", getpid());

    //Create first child (level 1)
    pid_p1 = fork();
    if (pid_p1 == 0) {

        pid_t pids_p2[3];
        //Creating three children (Nivel 2)
        for(i=0;i<3;i++){
            pids_p2[i] = fork();
            if(pids_p2[i] == 0){
                //Child process (Nivel 2)
                printf("NIVEL 1: Soy el primer hijo con PID %d y cree un hijo %d, con pid: %d\n", getppid(),i+1, getpid());

                pid_t pids_p3[2];
                wait(NULL); 
                //Creating two children (Nivel 3)
                for(int j=0;j<2;j++){
                    pids_p3[j] = fork();
                    if(pids_p3[j] == 0){
                        //Child process (Nivel 3)
                        printf("NIVEL 2: Soy el hijo con PID %d y mi padre es %d\n", getpid(), getppid());
                        wait(NULL);
                        exit(0); 
                    }else if(pids_p3[j] < 0){
                        //Fork failed
                        perror("Error al crear el hijo de nivel 3");
                        exit(1);
                    }
                }
                // Wait for all level 3 children to finish
                for(int j=0;j<2;j++){
                    wait(NULL);
                }//Something happens to creating third level 2 children
                //its only sometimes it shows ppid = 1485
                exit(0); // Exit after creating level 2 child

            }else if(pids_p2[i] < 0){
                //Fork failed
                perror("Error al crear el hijo de nivel 2");
                exit(1);
            }
        }
    }else if(pid_p1 < 0){
        //Fork failed
        perror("Error al crear el hijo de nivel 1");
        exit(1);
    }
    
}

/*int main(){
    //Create main process
    pid_t pid_p1;
    int i;

    printf("NIVEL 0: Soy el proceso padre principal con PID %d\n", getpid());

    //Create first child (level 1)
    pid_p1 = fork();
    if (pid_p1 == 0) {
        //Child process
        printf("NIVEL 1: Soy el primer hijo con PID %d y mi padre es %d\n", getpid(), getppid());

        pid_t pids_p2[3];
        //Creating three children (Nivel 2)
        for(i=0;i<3;i++){
            pids_p2[i] = fork();
            if(pids_p2[i] == 0){
                //Child process (Nivel 2)
                printf("NIVEL 2: Soy el hijo %d con PID %d y mi padre es %d\n", i+1, getpid(), getppid());

                pid_t pids_p3[2];
                //Creating two children (Nivel 3)
                for(int j=0;j<2;j++){
                    pids_p3[j] = fork();
                    if(pids_p3[j] == 0){
                        //Child process (Nivel 3)
                        printf("NIVEL 3: Soy el hijo %d del proceso %d con PID %d y mi padre es %d\n", j+1, i+1, getpid(), getppid());
                        exit(0); // Exit after creating terminal child
                    }else if(pids_p3[j] < 0){
                        //Fork failed
                        perror("Error al crear el hijo de nivel 3");
                        exit(1);
                    }
                }
                // Wait for all level 3 children to finish
                for(int j=0;j<2;j++){
                    wait(NULL);
                }
                exit(0); // Exit after creating level 2 child

            }else if(pids_p2[i] < 0){
                //Fork failed
                perror("Error al crear el hijo de nivel 2");
                exit(1);
            }
        }
    }else if(pid_p1 < 0){
        //Fork failed
        perror("Error al crear el hijo de nivel 1");
        exit(1);
    }
    

}*/

