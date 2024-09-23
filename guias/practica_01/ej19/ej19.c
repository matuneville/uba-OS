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