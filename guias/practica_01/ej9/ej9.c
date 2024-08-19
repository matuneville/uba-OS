#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

pid_t ping_pid;
pid_t pong_pid;
int count = 0;


// Handler proceso padre (ping)
void ping() {
    if (count++ % 3 == 0 && count != 1) {
        char answer = 0;
        do { // No entendi este ciclo raro de joni pero se lo copie
            printf("Desea continuar? (y/n) ");
            answer = tolower(getchar());
            while (getchar() != '\n');
        } while (answer != 'y' && answer != 'n');
        if (answer == 'n') {
            kill(pong_pid, SIGTERM); // Termino proceso hijo
            exit(EXIT_SUCCESS); // Termino proceso padre
        }
    }

    printf("[%d] ping\n", ping_pid);
    kill(pong_pid, SIGUSR1); // Solicito al proceso hijo que imprima pong
}

// Handler proceso hijo (pong)
void pong() {
    printf("[%d] pong\n", pong_pid);
    kill(ping_pid, SIGUSR1); // Solicito al padre que imprima ping
}

int main(int argc, char const *argv[]){
    ping_pid = getpid();
    pong_pid = fork();

    if (pong_pid < 0)
        exit(EXIT_FAILURE);

    if (pong_pid > 0) { // proceso padre
        sleep(0.5); // Esperar a que el proceso hijo registre su handler
        signal(SIGUSR1, ping); // Configurar el manejador para SIGUSR1
        ping(); // Llamar a ping para iniciar el proceso
    }
    else { // proceso hijo
        pong_pid = getpid();
        signal(SIGUSR1, pong);
    }

    while (1);
}