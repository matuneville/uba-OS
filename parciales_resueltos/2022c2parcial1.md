https://www.cubawiki.com.ar/images/f/f7/SO_1p_2c2022_resuelto.pdf

## Ejercicio 1
```python
contador = 0
barrera1 = sem(0)
barreraReset = sem(0)

def proceso():	
	while True:
		# cada funcion la puede ejecutar solo 1 proceso a la vez
		mutex.wait()
		primera_f()
		if ++contador == N:
			barrera1.signal(N)
			contador = 0
		mutex.signal()

		barrera1.wait()

		mutex.wait()
		segunda_f()
		if ++contador == N:
			barrera1.signal(N)
			contador = 0
		mutex.signal()

		barrera1.wait()

		mutex.wait()
		tercera_f()
		if ++contador == N:
			barreraReset.signal(N)
			contador = 0
		mutex.signal()

		barreraReset.wait()
```


## Ejercicio 2

a), e)
```
p: |   1   |   2   |   3   |   4   |   1   |   3   | 4  |   1   |   3   |
t: 0       19      38      57      76     97      118  126      143     174
t:   19/57   19/19   19/71   19/27   40/57   40/71  27/27  57/57  71/71
cola:  1       1       1       1       2       2      2     3       3
		   p2 termina		        todos los procesos bajan
```

b) Turnaround promedio:
- $\frac{143 + 38-1 + 174-2 + 126-3}{4} = 118.75ms$

Waiting time promedio: 
- $\frac{(0+57+29)+(19-1)+(38-2+40+25)+(57-3+42)}{4} = 75.25ms$


## Ejercicio 3

```c
int pipefd1 [2] , pipefd2 [2]; // globales

void MANEJADOR ( int signum ) {
	print ( " Alarma : el tiempo termino " );
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
	printf ( " El hijo envia un mensaje al padre !\ n " ) ;
}
void ESPERAR_AL_PADRE ( void ) {
	// TODO3
	int num;
	read(pipefd1[PIPE_READ], &num, sizeof(int));
	printf ( " El hijo recibe un mensaje desde el padre !\ n " ) ;
}
void HABLAR_AL_HIJO ( void ) {
	// TODO4
	int num;
	write(pipefd1[PIPE_WRITE], &num, sizeof(int));
	printf ( " El padre envia un mensaje al hijo !\ n " ) ;
}
void ESPERAR_AL_HIJO ( void ) {
	// TODO5
	int num;
	read(pipefd2[PIPE_READ], &num, sizeof(int));
	printf ( " El padre recibe un mensaje desde el hijo !\ n " ) ;
}
int main ( int argc , char * argv []) {
	PIPES () ;
	pid_t pid ;
	pid = fork () ;
	// TODO6 . Manejo de senales .
	signal(SIGALARM, MANEJADOR);
	if(pid < 0) exit(FAILURE);
	
	// alarm toma como parametro el nro de segundos
	alarm ( 10 ) ;

	// adicional: chequeamos si el pid no es < 0 para asegurar que no hay error
	if ( pid < 0 ) exit(EXIT_FAILURE);

	if ( pid != 0) {
		close(pipefd1[PIPE_READ]);
		close(pipefd2[PIPE_WRITE]);
		while (1) {
			sleep ( rand () %2 + 1) ;
			HABLAR_AL_HIJO () ;
			ESPERAR_AL_HIJO () ;
		}
	} else {
		close(pipefd1[PIPE_WRITE]);
		close(pipefd2[PIPE_READ]);
		while (1) {
			sleep ( rand () %2 + 1) ;
			ESPERAR_AL_PADRE () ;
			sleep(1);
			HABLAR_AL_PADRE () ;
		}
	}
	return 0;
}
```