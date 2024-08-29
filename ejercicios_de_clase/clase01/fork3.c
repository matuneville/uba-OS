#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]){
	pid_t pid = fork();
	//si no hay error, pid vale 0 para el hijo
	//y el valor del process id del hijo para el padre
	if (pid == 0) {
		//hijo Julieta 
		write(1,"Soy Julieta\n", 12);
		sleep(1);
		// creo jennifer						
  		pid_t pid = fork();

		if (pid == 0) {
			//hijo - Jennifer 
			write(1, "Soy Jennifer\n", 14);
			sleep(1);
		}
		else{ // si soy julieta, espero a que termine jennifer
			int status;
			if(wait(&status) < 0){perror("wait");exit(-1);}
		}

	} else {
		write(1, "Soy Juan\n", 10);
		sleep(1);						
		// Creo a Jorge, pero antes espero a julieta
		int status;
		// Si termino con errores
		if(wait(&status) < 0){perror("wait");exit(-1);}
		// Si termino "normalmente" (no recibió señal)
  		pid_t pid = fork();
		if (pid == 0) {
			write(1, "Soy Jorge\n", 10);
			sleep(1);						
		}	
  }
  exit(EXIT_SUCCESS); //cada uno finaliza su proceso
}
