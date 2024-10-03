#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

enum PipeEnd {
    PIPE_READ = 0,
    PIPE_WRITE = 1
};

int pipefd1 [2] , pipefd2 [2]; // globales

void MANEJADOR ( int signum ) {
	printf ( " Alarma : el tiempo termino \n" );
	// TODO1
	exit(EXIT_SUCCESS);
}

void PIPES ( void ) {
	if ( pipe ( pipefd1 ) < 0 || pipe ( pipefd2 ) < 0){
		perror ( " pipe " ) ;
		exit ( EXIT_FAILURE ) ;
	}
}
void HABLAR_AL_PADRE ( void ) {
	// TODO2
	int num;
	write(pipefd2[PIPE_WRITE], &num, sizeof(int));
	printf ( "3. El hijo envia un mensaje al padre !\n " ) ;
}
void ESPERAR_AL_PADRE ( void ) {
	// TODO3
	int num;
	read(pipefd1[PIPE_READ], &num, sizeof(int));
	printf ( "2. El hijo recibe un mensaje desde el padre !\n " ) ;
}
void HABLAR_AL_HIJO ( void ) {
	// TODO4
	int num;
	write(pipefd1[PIPE_WRITE], &num, sizeof(int));
	printf ( "1. El padre envia un mensaje al hijo !\n " ) ;
}
void ESPERAR_AL_HIJO ( void ) {
	// TODO5
	int num;
	read(pipefd2[PIPE_READ], &num, sizeof(int));
	printf ( "4. El padre recibe un mensaje desde el hijo !\n " ) ;
}
int main ( int argc , char * argv []) {
	PIPES () ;
	pid_t pid ;
	pid = fork () ;
	// TODO6 . Manejo de senales .
	signal(SIGALRM, MANEJADOR);
	if(pid < 0) exit(EXIT_FAILURE);
	
	// alarm toma como parametro el nro de segundos
	alarm ( 10 ) ;

	// adicional: chequeamos si el pid no es < 0 para asegurar que no hay error
	if ( pid < 0 ) exit(EXIT_FAILURE);

	if ( pid != 0) {
		close(pipefd1[PIPE_READ]);
		close(pipefd2[PIPE_WRITE]);
		while (1) {
			//sleep ( rand () %2 + 1) ;
			HABLAR_AL_HIJO () ;
			ESPERAR_AL_HIJO () ;
		}
	} else {
		close(pipefd1[PIPE_WRITE]);
		close(pipefd2[PIPE_READ]);
		while (1) {
			//sleep ( rand () %2 + 1) ;
			ESPERAR_AL_PADRE () ;
			//sleep(1);
			HABLAR_AL_PADRE () ;
		}
	}
	return 0;
}