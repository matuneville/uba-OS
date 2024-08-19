# Resueltos práctica 1

### Ejercicio 1

Un **context switch** en un sistema operativo guarda el estado del proceso actual en su PCB (Process Control Block), selecciona el siguiente proceso a ejecutar y restaura su estado desde su respectivo PCB. El PCB contiene toda la información necesaria para gestionar el proceso, como registros, estado y recursos asignados.

### Ejercicio 2

```c
struct PCB {
    int STAT; // valores posibles KE_RUNNING, KE_READY, KE_BLOCKED, KE_NEW
    int P_ID; // process ID
    int PC; // valor del PC del proceso al ser desalojado
    int RO; // valor del registro R0 al ser desalojado
    //...
    int R15; // valor del registro R15 al ser desalojado
    int CPU_TIME // tiempo de ejecución del proceso
}
```

```c
void Ke_context_switch(PCB* pcb_0, PCB* pcb_1){
    // Actualizo el PCB del proceso a desalojar
    pcb_0->STAT = KE_READY;
    pcb_0->R0 = R0;
    //...
    pcb_0->R15 = R15;
    pcb_0->CPU_TIME += ke_current_user_time();

    ke_reset_current_user_time();

    // Actualizo el PCB del proceso entrante
    pcb_1->STAT = KE_RUNNNING;
    R0 = pcb_1->R0
    //...
    R15 = pcb_1->R15

    set_current_process(pcb_1->P_ID);
    ret();
}
```

### Ejercicio 3

La diferencia clave entre una **system call** y una **llamada a función de biblioteca** que utiliza una system call (como `printf`) es que la primera es una función del kernel que permite a un programa de usuario solicitar servicios del sistema operativo, mientras que la segunda es una función de biblioteca que puede invocar una system call para realizar su tarea. Por ejemplo, `printf` es una función de biblioteca que eventualmente llama a la system call `write` para enviar datos a la salida estándar.


### Ejercicios 5/7

```c
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
```

Output:
```
$ ./ej5
Soy Abraham Simpson
Soy Homero Simpson
Soy Bart Simpson
Soy Lisa Simpson
Soy Maggie Simpson
Termina Homero Simpson
Termina Abraham Simpson
```

### Ejercicio 8

Cuando se forkea, la variable `dato` en el proceso hijo queda inicializada con 0 y se modifica en su espacio de memoria. Este proceso hijo modifica a la variable dejándola con valor 3 e imprimiendo 1,2,3, y terminando luego el proceso. Pero el proceso padre no ve su variable modificada por el proceso hijo ya que, como dijimos, cada uno lo modifica en su propio espacio de memoria. Por lo tanto, el proceso padre imprimirá tres veces el variable de `dato` inicial, que es 0, y luego terminará.

### Ejercicio 9

```c
pid_t ping_pid;
pid_t pong_pid;
int count = 0;


// Handler proceso padre (ping)
void ping() {
    if (count++ % 3 == 0 && count != 1) {
        char answer = 0;
        do { // No entendi este ciclo raro de joni pero se lo copie
            printf("Continue [y/n]? ");
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
```

Output:
```
$ ./ej9 
[41797] ping
[41798] pong
[41797] ping
[41798] pong
[41797] ping
[41798] pong
Desea continuar? (y/n) y
[41797] ping
[41798] pong
[41797] ping
[41798] pong
[41797] ping
[41798] pong
Desea continuar? (y/n) n
```

### Ejercicio 10

```c
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
```

Output:
```
$ ./programa 
Soy Juan
Soy Julieta
Soy Jennifer
Soy Jorge
```