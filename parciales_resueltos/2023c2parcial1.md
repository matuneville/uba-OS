Parcial de cubawiki: https://www.cubawiki.com.ar/images/a/ab/SisOp_1parcial_03-10-23_enunciado.pdf

## Ejercicio 1

```python
usuariosCompraron = 0
usariosEntraronAComprar = atom(0)
permisoComprar = sem(10)
mutexComprar = sem(1)
barreraBloqueada = False
personasEnBarrera = 0

def comprasTaylorSwift():
	
	if barreraBloqueada:
		personasEnBarrera += 1
		barreraHacerCola.wait()
	
	# justo este es el espacio de codigo en donde estaran las personas esperando a comprar,
	# y la barrera de arriba la cerraremos cuando se descongestione la fila asi ninguno se cola hasta
	# que 10 de aca entraron a comprar
	
	permisoComprar.wait()

	usuariosEntraronAComprar.getAndInc()
	# permitimos que compren de a muchos a la vez
	comprar_ticket()
	usuariosCompraron.getAndInc()

	mutexComprar.wait()
	if usuariosCompraron == 1 and usuariosEntraronAComprar == 10:
		# si soy el primero en entrar a comprar, y ya todos los 10 entraron a esta seccion
		# abro la barrera para que hagan fila muchas personas, y asi ninguno llegó a colarse
		barreraBloqueada = False
		# dejo pasar a todos los que estaban bloqueados para que pasen a la cola de compra
		barreraHacerCola.signal(personasEnBarrera)
		personasEnBarrera = 0

	if usuariosCompraron == 10:
		# si fui el ultimo en comprar, antes de permitir que compren 10 mas de la fila, debo
		# cerrar la barrera asi mientras aquellos entran, ninguno que no estaba en la fila se puede colar
		barreraBloqueada = True
		permisoComprar.signal(10)
		usuariosCompraron = 0
		usuariosEntraronAComprar = 0
	mutexComprar.signal()
```

## Ejercicio 2

Sabemos que los procesos real-time (movimientos de NPCs, objetos del juego, escenario, etc.) aparecen con menos frecuencia, y a su vez, tenemos que hacer que los procesos interactivos (con I/O) del jugador sigan teniendo buen tiempo de respuesta. Para esto, planteamos un scheduling de política Multilevel Queue:
- Cola 0 de mayor prioridad: acá se administrarán los procesos RT del juego (sabemos que siempre los procesos RT son de máxima prioridad, al cual le siguen los interactivos). Como este tipo de procesos tienen restricciones de tiempo fijas, hacemos que trabaje con una política de Earliest-Deadline First, ya que deben devolver resultados dentro de sus limitaciones. En caso de que no sepamos su deadline, podemos hacer que trabaje con un FCFS, ya que, en mi opinión, es lógico que los procesos se ejecuten en el orden en que fueron disparados (ya que, por ej., en movimientos de un oponente, esto es importante).
- Cola 1 de menor prioridad: acá se administrarán los procesos interactivos del usuario. Como trabajan con I/O, tendrán ráfagas cortas de CPU ya que bloquearán a menudo, por lo que podemos hacer que trabaje con un Round-Robin de un quantum de un tiempo promedio con el que tarden en trabajar en CPU estos procesos. Con esto, empezaremos a generar resultados de manera temprana y justa para cada proceso interactivo del jugador (baja latencia), dando una buena sensación de tiempo de respuesta, lo cual es bueno en caso de que, por ejemplo, se cliquee muchas veces a la vez, o se toquen muchas teclas en un periodo de tiempo muy corto.
Como los procesos RT del juego aparecen con poca regularidad, no es probable que la cola 1 sufra de starvation, por lo que los procesos interactivos del jugador no quedarán retrasados respecto a los RT y no habrá una sensación de lento tiempo de respuesta.  
En caso de que sea posible que se disparen muchos procesos del juego en un corto periodo de tiempo, podemos hacer que el scheduler trabaje con un feedback, haciendo que suban de prioridad los procesos de I/O en dicho momento (caso hipotético, un enemigo realiza una acción de ataque múltiples veces al jugador, sería necesario en ese mismo momento también obtener respuesta pronta de I/O).

## Ejercicio 4

### Inciso A

```c
void procesoPadre(int n, int m){
	// aca arranca el padre
	int pipes[n][2];
	pid_t hijos[n];

	for(int i = 0; i < n; i++)
		pipe(pipes[i]);

	// creamos hijos
	for(int i = 0; i < n; i++){
		hijo[i] = fork();
		if(hijo[i] < 0)
			exit(FAILURE);
		if(hijo[i] == 0)
			procesoHijo(pipes[i]);
	}

	// les pasamos los datos necesarios
	for(int i = 0; i < n; i++)
		write(pipes[i][PIPE_WRITE], &m, sizeof(int));

	// esperamos a que procesen hijos, y ahora debemos leer sus resultados
	int labos[n];
	for(int i = 0; i < n; i++)
		read(pipes[i][PIPE_READ], &labos[i], sizeof(int));

	// printeo resultados
	// el i-ésimo pid de hijo coincide con el labo asignado a dicho estudiante hijo,
	// por como estan hechos los ciclos for
	for(int i = 0; i < n; i++)
		printf("Hijo: %d, labo: %d\n", hijos[i], labos[i]);
		// en realidad esto lo podria haber hecho en el for de arriba, pero asi queda mas modularizado xd

	// cierro pipes
	for(int i = 0; i < n; i++)
		close(pipes[i][PIPE_READ]);
		close(pipes[i][PIPE_WRITE]);

	return;
}


void procesoHijo(int pipe[2]){
	int m;
	// leemos el numero que nos pasa el padre
	read(pipe[PIPE_READ], &m, sizeof(int));

	// obtengo mi pid
	pid_t miPid = getpid();

	// obtengo mi labo
	int labo = dameLabo(miPid, m);

	// se lo paso al padre
	write(pipe[PIPE_WRITE], &labo, sizeof(int));

	// ciero pipes y termino
	close(pipes[PIPE_READ]);
	close(pipes[PIPE_WRITE]);

	exit(0);
}
```

### Inciso B

fiaca :p