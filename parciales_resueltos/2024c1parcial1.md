## Ejercicio 1

![](../enunciados/Pasted%20image%2020240920203124.png)

Tenemos 3 procesos:
- p1: Toma string como input de usuario, se lo envía a p2 y p3
- p2: Al string recibido lo invierte y se lo envía a p3 
- p3: Tomando los strings (el original de p1 y el invertido de p2), los compara e imprime en pantalla si es o no un palíndromo

Asumo que inicia la ejecución del programa en p1 (como si fuera un `main`):

```c
p2(int[2] pipeP1P2, int[2] pipeP2P3):
    char* palabra;
    read(pipeP1P2[PIPE_READ], &palabra, sizeof(palabra));

    palabra_invertida = invertir(palabra);

    write(pipeP2P3[PIPE_WRITE], &palabra_invertida, sizeof(palabra_invertida));
    exit(0);

p3(int[2] pipeP1P3, int[2] pipeP2P3):
    char* palabra_original;
    read(pipeP1P3[PIPE_READ], &palabra, sizeof(palabra_original));

    char* palabra_invertida;
    read(pipeP2P3[PIPE_READ], &palabra, sizeof(palabra_invertida));

    int N = len(palabra_original);
    for(int i = 0; i < N; i++)
        if(palabra_original[i] != palabra_invertida[N-1-i])
            print("No es palíndromo");
            exit(0);

    print("Es palindromo");
    exit(0);

main():
    char* palabra = take_input();

    int pipeP1P2[2] = pipe();
    int pipeP1P3[2] = pipe();
    int pipeP2P3[2] = pipe();

    pid_t pid_p2 = fork();
    if (pid_p2 < 0) exit(FAILLURE);
    if (pid_p2 == 0)
        p2(pipeP1P2, pipeP2P3);

    pid_t pid_p3 = fork();
    if (pid_p3 < 0) exit(FAILLURE);
    if (pid_p3 == 0)
        p3(pipeP1P3, pipeP2P3);
    
    // espero a que terminen y los killeo
    wait(pid_p2);
    wait(pid_p3);

    kill(pid_p2, 9);
    kill(pid_p3, 9);

    return;
```

## Ejercicio 2

![](../enunciados/Pasted%20image%2020240920203146.png)

### Inciso a)

Veamos un ejemplo con números:
- $\alpha = 0.5,\ \beta = 1$
- $P_t(i) = 2$
- Si el proceso está en espera, $P_{t+1}(i) = P_t(i) + \alpha P_t(i) = 2+0.5 \times 2 = 3$
- Si el proceso se está ejecutando, $P_{t+1}(i) = P_t(i) + \beta P_t(i) = 2+1 \times 2 = 4$

Se ve claramente que si $\beta > \alpha$, la prioridad del proceso $i$ en el siguiente momento (en $t+1$) será mayor si se está ejecutando. Esto es un sinsentido, ya que si un proceso se está ejecutando, y en el siguiente momento sigue en ejecución, vemos que su no influye en nada su cambio de prioridad hacia arriba, y esto aumentará para $t+2,\ t+3,\ ...$, ocupando, en caso de que el proceso se ejecute constantemente, tiempo infinito en CPU. Esto lleva al starvation de los procesos que están en espera.  Y en caso de que un proceso esté acotado por CPU y requiera mucho tiempo de ejecución, también se verá favorecido, misma explicación que antes.

Además, el desalojo no servirá de nada en esta implementación de scheduling por prioridad si asumimos que $P_0(i)$ es igual para todo proceso $i$ (o cercano), ya que así nunca un proceso nuevo llegará a tener mayor prioridad que el que esté ocupando CPU (pues $\alpha$ y $\beta$ son iguales para todo proceso). En caso de que el proceso en ejecución se bloquee por E/S, el siguiente proceso que pase a ejecutarse sufrirá del mismo problema.

### Inciso b)

En este caso, los procesos que estén en espera de ser ejecutados por CPU aumentarán de prioridad a medida que pase el tiempo. Esto es conocido como _aging_ (creo). También, los procesos de E/S se verán favorecidos, ya que al bloquearse constantemente, suelen pasar a espera a menudo y más tiempo que los procesos que no son de E/S, haciendo que cuando ocurra esto, su nivel de prioridad comience a aumentar.

## Ejercicio 3

![](../enunciados/Pasted%20image%2020240920203218.png)

```python
string path
sem permisoInstalar = sem(0)
sem permisoSeguirTesteando = sem(0)
int cantTests
int cantTestsGenerados
sem mutex = sem(1)
bool sigoTesteando = True
bool sigoTesteandoRestantes = True
list testsGenerados = []
sem permisoEscribirTests = sem(0)

Coordinador():
	path = compilarAplicacion()
	# los generadores ya pueden instalarla
	permisoInstalar.signal(N)

	cantTests = determinarCantCasosDeTest(path, N)

	# ahora ya puedo darle permiso a los N generadores que sigan testeando
	permisoSeguirTesteando.signal(N)

	permisoEscribirTests.wait()
	escribirTests(testsGenerados)

Generador(id):
	permisoInstalar.wait()
	# ya puedo instalar la app	
	instalarApicación(id, path)

	# ahora, sé que como mínimo habrá N tests a generar, por lo que genero hasta N
	while True:
		mutex.wait
		if not sigoTestando
			mutex.signal()
			break
		testsGenerados.append(generarTest())
		cantTestsGenerados++
		if cantTestsGenerados == N
			sigoTesteando = False
		mutex.signal()

	# ahora tengo que esperar a saber cual es la cantidad de tests que tengo que hacer
	# para llegar al total de los requeridos
	permisoSeguirTesteando.wait()
	while True:
		mutex.wait
		if not sigoTestandoRestantes
			mutex.signal()
			break
		testsGenerados.append(generarTest())
		cantTestsGenerados++
		if cantTestsGenerados == cantTests
			sigoTesteandoRestantes = False
			# le doy permiso al coordinador de escribir los tests
			permisoEscribirTests.signal()
		mutex.signal()

```

## Ejercicio 4

![](../enunciados/Pasted%20image%2020240920203238.png)

