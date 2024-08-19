#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(){
    // Inicia Abraham
    printf("Soy Abraham Simpson\n");

    // creo hijo Homero
    pid_t pid_homero = fork();

    // si pid == 0, es Homero
    if (pid_homero == 0){
        printf("Soy Homero Simpson\n");

        // creo hijos Bart, Lisa y Maggie
        pid_t pid_bart = fork();
        // si pid == 0 es hijo de Homero
        if (pid_bart == 0){
            printf("Soy Bart Simpson\n");
            exit(0);
        } // espero a que termine Bart
        wait(NULL);

        pid_t pid_lisa = fork();
        if (pid_lisa == 0){
            printf("Soy Lisa Simpson\n");
            exit(0);
        }
        wait(NULL);

        pid_t pid_maggie = fork();
        if (pid_maggie == 0){
            printf("Soy Maggie Simpson\n");
            exit(0);
        }
        wait(NULL);

        // termina Homero, sus hijos ya terminaron
        printf("Termina Homero Simpson\n");
        exit(0);
    }

    // si no, es Abraham, y espera a que termine Homero para finalizar
    else {
        wait(NULL);
        printf("Termina Abraham Simpson\n");
        exit(0);
    }

    return 0;
}