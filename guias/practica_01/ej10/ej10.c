#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>

// segun el strace, el ejecutable se llama programa

int main(){
    // primer hay un clone que devuelve pid 10552, el del hijo, Julieta
    pid_t pid_julieta = fork();

    // luego Juan (10551) printea su nombre, lo mismo Julieta
    // para eso lo dividimos en distintos scopes
    
    if (pid_julieta == 0){
        // estamos en Julieta
        write(1, "Soy Julieta\n", 12);
        // luego hace un sleep 1 sec
        sleep(1);
        // luego hay un clone que devuelve pid 10557, el del hijo de Julieta, Jennifer
        pid_t pid_jennifer = fork();

        if (pid_jennifer == 0){
            // estamos en Jennifer
            // escribe su nomnre y luego hace un sleep 1 sec
            write(1, "Soy Jennifer\n\0", 14);
            sleep(1);
            // luego termina exitosamente
            exit(0);
        } else {
            // estamos en Julieta
            // hace un exit exitoso
            exit(0);
        }
    } else {
        // estamos en Juan
        write(1, "Soy Juan\n\0", 10);
        // luego hace un sleep 1 sec
        sleep(1);
        // luego hace un wait que devuelve 10552, que en wait4 es el
        // pid del hijo que termina con exito, o sea, julieta
        waitpid(pid_julieta, NULL, 0);

        // luego clonea de nuevo y obtiene hijo 10558, Jorge
        pid_t pid_jorge = fork();

        if (pid_jorge == 0){
            // estamos en Jorge
            // printea su nombre y luego duerme 1 sec
            write(1, "Soy Jorge\n", 10);
            sleep(1);
            // luego termina exitosamente
            exit(0);
        } else {
            // luego de forkear, 10551 termina exitosamente
            exit(0);
        }
    }
}