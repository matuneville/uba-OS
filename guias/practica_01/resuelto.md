# Resueltos práctica 1: Procesos y API del SO

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

### Ejercicio 11

#### a)
```c
void rutina_padre(pid_t pid_hijo){
    int msg = 0;
    while(13){
        // Envio a hijo msg
        bsend(pid_hijo, msg);
        // Recibo de hijo msg e incremento
        msg = breceive(pid_hijo);
        msg++;
    }
}

void rutina_hijo(pid_t pid_padre){
    int msg;
    while(13){
        // Recibo de padre msg
        msg = breceive(pid_padre);
        msg++; // Incremento y envio a padre
        bsend(pid_padre, msg);
    }
}

int main(){
    pid_t pid_padre = get_current_pid();
    pid_t pid_hijo = fork();

    if(pid_hijo < 0) // si falla el forkeo
        exit(EXIT_FAILURE);

    if(pid_hijo == 0){
        rutina_hijo(pid_padre);
    } else {
        rutina_padre(pid_hijo);
    }

    return 0;
}
```

#### b)

```c
void rutina_padre(pid_t pid_hijo1, pid_t pid_hijo2){
    int msg = 0;
    while(msg != 50){
        // Envio a hijo1 msg
        bsend(pid_hijo1, msg);
        // Recibo de hijo2 msg e incremento
        msg = breceive(pid_hijo2);
        msg++;
    }
    exit(0);
}

void rutina_hijo1(pid_t pid_padre, pid_t pid_hijo2){
    int msg;
    while(msg != 50){
        // Recibo de padre msg
        msg = breceive(pid_padre);
        msg++; // Incremento y envio a hijo2
        bsend(pid_hijo2, msg);
    }
    exit(0);
}

void rutina_hijo2(pid_t pid_hijo1, pid_t pid_padre){
    int msg;
    while(msg != 50){
        // Recibo de hijo1 msg
        msg = breceive(pid_hijo1);
        msg++; // Incremento y envio a padre
        bsend(pid_padre, msg);
    }
    exit(0);
}

int main(){
    pid_t pid_padre = get_current_pid();
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
```
Hubiera sido mucho mas limpio si hiciera una funcion para los 3 casos y solo tomara pids de parametro para hacer lo que tiene que hacer, pero me dio fiaca asi que mejor copypastear.

### Ejercicio 12

El problema de intentar seguir esa secuencia de ejecución es que las syscalls `bsend()` y `breceive()` son bloqueantes, y la cola de mensajes es de capacidad 0.  

Como el `proceso_derecha` comienza por ` cómputo_muy_difícil_2();`, y el `proceso_izquierda` comienza por el `bsend()`, este último debe esperar a que el otro proceso ejecute el `breceive()`, por lo que ya desde el comienzo de cada ciclo quedan "desfasados" los procesos y no pueden cada uno seguir la secuencia del enunciado.

### Ejercicio 14

#### a)

```c
void proceso_izquierda() {
    result = 0;
    while (true) {
        while (!send(pid_derecha, &result)) {}
        result = cómputo_muy_difícil_1();
    }
}

void proceso_derecha() {
    int left_result;
    while (true) {
        result = cómputo_muy_difícil_2();

        while (!receive(pid_izquierda, &left_result)) {}
        printf(" %d %d", left_result, result);
    }
}
```

### Ejercicio 16

Hecho en clase:

```c
// Constants 0 and 1 for READ and WRITE
enum { READ, WRITE };

void ejecutar_cmd(char* cmd, char* p) {
  execlp(cmd, cmd, p, NULL);
}

// Debe ejecutar "ls -al"
void ejecutar_hijo_1(int pipe_fd[]) {
  // Cerrar lectura
  close(pipe_fd[READ]);
  // Conectar escritura a stdout
  dup2(pipe_fd[WRITE], STDOUT_FILENO);
  // Ejecutar programa
  ejecutar_cmd("ls", "-al");
}

// Debe ejecutar "wc -l"
void ejecutar_hijo_2(int pipe_fd[]) {
  // Cerrar escritura
  close(pipe_fd[WRITE]);
  // Conectar lectura a stdin
  dup2(pipe_fd[READ], STDIN_FILENO);
  // Ejecutar programa
  ejecutar_cmd("wc", "-l");
}

int main(int argc, char const* argv[]) {
  int pipe_fd[2];
  pipe(pipe_fd);

  if (fork() == 0) {
    ejecutar_hijo_1(pipe_fd);
  }

  if (fork() == 0) {
    ejecutar_hijo_2(pipe_fd);
  }

  // El padre cierra el fd de escritura
  // Solo así hijo_2 va a recibir EOF cuando hijo_1 termina
  close(pipe_fd[WRITE]);

  // Esperamos que terminen los hijos antes de terminar el padre
  wait(NULL);
  wait(NULL);

  return 0;
}
```

### Ejercicio 17

```c
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
```

Output:
```
$ ./ej17 
Padre envía a Hijo_1 el valor 0,
Hijo_1 envía a Hijo_2 el valor 1,
Hijo_2 envía a Padre el valor 2,
Padre envía a Hijo_1 el valor 3,
Hijo_1 envía a Hijo_2 el valor 4,
[...]
Padre envía a Hijo_1 el valor 48,
Hijo_1 envía a Hijo_2 el valor 49,
Hijo_2 envía a Padre el valor 50,
```


### Ejercicio 18

```c
void ejecutarHijo(int i, int pipes[][2]){
    pid_t pid_padre = getppid();
}
```

### Ejercicio 19

Archivo `.c` de ejecutable  `./estrella`:

```c
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void procesoHijo();
void procesoPadre();
void handlerSIGINThijo();
void handlerSIGHUPhijo();
void handlerSIGINTpadre();
void handlerSIGCHLDpadre();

int pipeHijoPadre[2];
pid_t pidHijo;
pid_t pidPadre;

enum PipeEnd {
    PIPE_READ = 0,
    PIPE_WRITE = 1
};

int main(){
	pipe(pipeHijoPadre); // creo pipe, pipe = [3,4]
	pidHijo = fork(); // hijo de pid 6590

	// aca deberia chequear que creacion de pipe y forkeo no hayan fallado...

	if(pidHijo == 0){
		procesoHijo();
	}
	else{
		procesoPadre();
	}
}

void procesoHijo(){
	close(pipeHijoPadre[PIPE_READ]); // cierro pipe[0]
	pidPadre = getppid();
	signal(SIGINT, handlerSIGINThijo);
	signal(SIGHUP, handlerSIGHUPhijo);

	while(13) pause();
}

void procesoPadre(){
	close(pipeHijoPadre[PIPE_WRITE]); // cierro pipe[1]
	signal(SIGINT, handlerSIGINTpadre);
	signal(SIGCHLD, handlerSIGCHLDpadre);
	sleep(1);
	printf("Cuál es el significado de la vida?\n");
	kill(pidHijo, SIGINT);

    while(13) pause();
}

void handlerSIGINThijo(){
	printf("Dejame pensarlo...\n");
	// aca aparecen unos signals que no se si van o no, finjo demencia porque no me cierra
	sleep(5);
	printf("Ya sé el significado de la vida\n");
	char numerito[2] = "42";
	write(pipeHijoPadre[PIPE_WRITE], &numerito, sizeof(numerito));
	kill(pidPadre, SIGINT);
}

void handlerSIGINTpadre(){
	char numerito[2];
	read(pipeHijoPadre[PIPE_READ], &numerito, sizeof(numerito));
	printf("Mirá vos. El significado de la vida es %s\n", numerito);
	printf("Bang bang, estás liquidado perra\n");
	kill(pidHijo, SIGHUP);
	// de nuevo aparecen unos signals, sospecho que es algo retornado al terminar handler
	sleep(10);
}

void handlerSIGHUPhijo(){
	printf("Me voy a mirar crecer las flores desde abajo\n");
	close(pipeHijoPadre[PIPE_WRITE]); // cierro pipe[1]
	exit(0); // esto va a disparar un SIGCHLD en el padre
}

void handlerSIGCHLDpadre(){
	printf("Te voy a buscar en la oscuridad\n");
    exit(EXIT_SUCCESS);
}
```