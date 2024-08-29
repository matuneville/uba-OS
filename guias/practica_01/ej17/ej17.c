#include <stdio.h>   // printf()
#include <stdlib.h>  // exit()
#include <unistd.h>  // fork() pipe() execlp() dup2() close()
#include <sys/wait.h>

// Constants 0 and 1 for READ and WRITE
enum { READ, WRITE };
// Constants 0, 1 and 2 for PROCESSES
enum { PADRE, HIJO_1, HIJO_2 };

// Pipes, variables globales
int pipes[3][2];

void rutina_padre(){
    int msg = 0;
    while(13){
        if(msg >= 50) break;
        printf("Padre envía a Hijo_1 el valor %d,\n", msg);
        // Escribo en pipe
        write(pipes[PADRE][WRITE], &msg, sizeof(msg));
        // Leo de hijo2 msg e incremento
        read(pipes[HIJO_2][READ], &msg, sizeof(msg));
        msg++;
    }
    exit(0);
}

void rutina_hijo1(pid_t pid_padre, pid_t pid_hijo2){
    int msg;
    while(13){
        // Leo de padre msg
        read(pipes[PADRE][READ], &msg, sizeof(msg));
        if(msg > 50) break;
        // Incremento y escribo
        msg++;
        printf("Hijo_1 envía a Hijo_2 el valor %d,\n", msg);
        write(pipes[HIJO_1][WRITE], &msg, sizeof(msg));
    }
    exit(0);
}

void rutina_hijo2(pid_t pid_hijo1, pid_t pid_padre){
    int msg;
    while(msg != 50){
        // Leo de hijo1 msg
        read(pipes[HIJO_1][READ], &msg, sizeof(msg));
        if(msg > 50) break;
        // Incremento y escribo
        msg++;
        printf("Hijo_2 envía a Padre el valor %d,\n", msg);
        write(pipes[HIJO_2][WRITE], &msg, sizeof(msg));
    }
    exit(0);
}

int main(){
    // Creo pipes
    for (int i = 0; i < 3; i++)
        pipe(pipes[i]);
    
    pid_t pid_padre = getppid();
    pid_t pid_hijo1 = fork();
    pid_t pid_hijo2 = fork();

    if(pid_hijo1 < 0 || pid_hijo2 < 0) // si falla algun forkeo
        exit(EXIT_FAILURE);

    if(pid_hijo1 == 0){
        rutina_hijo1(pid_padre, pid_hijo2);
    } else if(pid_hijo2 == 0){
        rutina_hijo2(pid_hijo1, pid_padre);
    } else {
        rutina_padre(pid_hijo1, pid_hijo2);
    }

    return 0;
}