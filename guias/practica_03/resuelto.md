# Resueltos práctica 3: Sincronización entre procesos

### Ejercicio 4

Demostrar que, en caso de que las operaciones de semáforos wait() y signal() no se ejecuten atómicamente, entonces se viola la propiedad EXCL (exclusión mutua: un recurso no puede estar asignado a más de un proceso.).  
_Pista: Revise el funcionamiento interno del wait() y del signal() mostrados en clase, el cual no se haría de forma atómica, y luego piense en una traza que muestre lo propuesto._

#### Resolución

Pseudocódigos de `wait()` y `signal()` dados en clase:
```c
wait(s){
    while(s <= 0) sleep();
    s--;
}
```

```c
signal(s) {
    s++;
    if (alguno espera por s)
        awake_alguno();
}
```

Si no fuera atómica, y tenemos, por ejemplo, dos procesos que realizan un `wait()` para luego ejecutar una funcion crítica `fn()`, entonces hay peligro de que cuando el primer proceso entra al código del `wait()`, salte la condición del `while` (porque partimos de que `s=1`) y cuando vaya a hacer la línea `s--`, haya un context switch, el otro proceso en paralelo ejecute todo el código del `wait()`y luego pase a ejecutar `fn()`. Cuando termine su tiempo (por ejemplo, si se acaba su quantum), y vuelva a ejecutarse el primer proceso, tendrá que hacer la línea `s--` y luego ejecutar la función crítica. De esta forma, ocurre un **race condition** donde los procesos trabajaron a la vez con el recurso variable `s` y lo modificaron de manera asincrónica sin estar comunicados.


### Ejercicio 5

El problema es que puede haber algun proceso esperando en el `wait` de la barrera y que luego el proceso que haga el `barrera.signal()` sea el mismo luego atraviesa el `wait` del mismo semáforo, haciendo que el proceso previamente mencionado nunca llegue a atravesar el `wait`. También hace falta arreglar el mutex para evitar una posible race condition con el `if` y su código:

```c
preparado()

mutex.wait()
count = count + 1

if (count == n)
    barrera.signal()
mutex.signal()

barrera.wait()
barrera.signal()

critica()
```

### Ejercicio 6

Básicamente, cada proceso suma al contador, y cuando este sea igual a N, el semáforo suma uno, y esto sólo ocurrirá con aquellos que ejecuten el código del if, al que sólo es posible entrar una vez que todos los N procesos ejecutaron `preparado()` y sumaron al contador. Como ya todos los procesos realizaron esto, el semáforo será mayor que cero. Luego, todos los procesos pueden saltarse el `while` para ejecutar `critica()`.

```c
atomic semaforo = 0
atomic shared counter = 0

proceso():
    preparado()

    counter.inc()
    if (counter.equal(N))
        semaforo.inc()
    
    while(semaforo.equal(0)){}

    critica()
```

a) En mi opinión, es más legible la solución del ejercicio 5.  
b) Es más eficiente la del ejercicio 5 ya que no hacen busy waiting sino que se bloquean yéndose a dormir, evitando consumir recursos del CPU.  
c) La del 5 requiere soporte de código de bajo nivel que interactúe con el SO o hardware. Para la nueva, solo soporte para manejar variables atómicas.

### Ejercicio 7

```c
semaforo S[N] = [0, 0, ..., 0]
global shared i_inicial = None
global shared i_counter = None

iniciar(i): // eso se hace una sola vez, no lo hace cada proceso
    i_inicial = i
    i_counter = i
    S[i].signal() // habilito tarea i

proceso_i():
    S[i_counter].wait()
    /*
    código proceso
    */
    i_counter = (i_counter + 1) % N
    if (i_counter == i_inicial - 1)
        exit(0) // termino proceso y no hago mas signals porque ya se ejecutaron todos

    S[i_counter].signal()
```


### Ejercicio 8

1. Es igual al hecho en clase.
```c
semaforo sem_A = sem(1)
semaforo sem_B = sem(0)
semaforo sem_C = sem(0)

pA(){
    while(true)
        sem_A.wait()
        
        proceso_A()

        sem_B.signal()
}

pB(){
    while(true)
        sem_B.wait()
        
        proceso_B()

        sem_C.signal()
}

pC(){
    while(true)
        sem_C.wait()
        
        proceso_C()

        sem_A.signal()
}
```


2. Este parece que podría fallar si pA se corta cuando hizo un solo signal a pB. Pero en ese caso, supongamos que pB llega a ejecutarse todo en su tiempo, entonces atraviesa el wait, se realiza el proceso B,  el counter resta 1, y cuando tenga qu repetirse el proceso B para luego restar nuevamente el counter y termina el ciclito, no va a poder, ya que se va a quedar esperando en el wait a recibir un nuevo signal. Finalmente, recibiría su segundo signal desde pA y se haría dos veces pB, y recien ahi, le manda signal a pC.

```c
semaforo sem_A = sem(0)
semaforo sem_B = sem(2)
semaforo sem_C = sem(0)

pA(){
    while(true)
        sem_A.wait()
        
        proceso_A()

        sem_B.signal()
        sem_B.signal()
}

pB(){
    while(true)
        counter = 2
        while(counter > 0):
            sem_B.wait()
            
            proceso_B()

            counter--
        sem_C.signal()
}

pC(){
    while(true)
        sem_C.wait()
        
        proceso_C()

        sem_A.signal()
}
```

3.  
```c
semaforo llenos = sem(0)
semaforo vacios = sem(2)

prod_A(){
    while(true)
        vacios.wait()
        vacios.wait()

        proceso_A()

        llenos.signal()
        llenos.signal()
}

prod_B(){
    while(true)
        llenos.wait()

        proceso_B()

        vacios.signal()
}

prod_C(){
    while(true)
        llenos.wait()

        proceso_C()

        vacios.signal()
}
```

### Ejercicio 13

```c
sem mutex[N] = sem(1); // mutex por mesa, utilizado por 1 cliente a la vez

int clientesEnMesa[N] = 0;
int clientesJugaron[N] = 0;

sem barreraJugar[N] = sem(0);
sem barreraAbandonarMesa[N] = sem(0);

cliente(){
    // bloqueante hasta conseguir mesa
    int mesa_i = conseguirMesa();

    // va a la mesa y chequea si son 4
    mutex[mesa_i].wait();
    clientesEnMesa[mesa_i]++;
    if (clientesEnMesa[mesa_i] == 4){
        // habilito a los 4 a jugar
        do 4 times:
            barreraJugar[mesa_i].signal();
    }
    mutex[mesa_i].signal();

    // esperamos a ser 4 para jugar
    barreraJugar[mesa_i].wait();

    // jugar no va dentro del mutex, pueden arrancar a jugar
    // de forma paralela todos los procesos de la mesa
    jugar();

    mutex[mesa_i].wait();
    // si esta acá es porque termino de jugar
    clientesJugaron[mesa_i]++;
    if (clientesJugaron[mesa_i] == 4){
        // los 4 terminaron de jugar, pueden irse de la mesa recien ahora
        do 4 times:
            barreraAbandonarMesa[mesa_i].signal()
        clientesJugaron[mesa_i] = 0;
        clientesEnMesa[mesa_i] = 0;
    }
    mutex[mesa_i].signal();

    barreraAbandonarMesa[mesa_i].wait();
    
    // Esta funcion la tiene que hacer cada uno supongo. Si no, iria
    // dentro del if anterior del mutex
    abandonarMesa(i);
}
```

### Ejercicio 15

```c
sem mutexEsperar[N_especies][2] = sem(0); // un semaforo por cada sexo para cada especie
sem mutexEntrar[N_especies] = sem(1);
int entraron[N_especies] = 0;

subirAlArca(especie_i, sexo){
    mutexEsperar[especie_i][sexo].signal(); // aviso al otro sexo que ya estoy en puerta

    mutexEsperar[especie_i][!sexo].wait();// esperamos al sexo contrario a que esté

    mutexEntrar[especie_i].wait();
    if( !entraron[especie_i]++ ){   // con esto me aseguro que solo el primero del sexo
        entrar(especie_i);          // que entre al mutex ejecute la función entrar(i)
    }
    mutexEntrar[especie_i].signal();
}
```

### Ejercicio 18

```c
sem permisoCargar = sem(0);
sem termineCargar = sem(0);
sem permisoDescargar = sem(0);
sem lavarRopa = sem(0);

LavarropasSamsung(){
	while(1){
		estoyListo();
		for(i in range(10)){
			permisoCargar.signal(); // cargo una prenda de ropa
			termineCargar.wait(); // termina de cargarse, sigo con la sgte
		}
		// lavo una vez que cargue todas las prendas
		lavar();
		// termina de lavar, habilito ropa a descargarse
		puedenDescargarme();
		for(i in range(10))
			permisoDescargar.signal();
		// espero a que se haya descargado toda
		for(i in range(10))
			ropaDescargada.wait();
		// ya se descargo todo, puedo seguir con proxima tanda
	}
}

RopaDeFlores(){
	permisoCargar.wait(); // espero a poder cargar
	entroAlLavarropas();
	termineCargar.signal(); // termine de cargarla, va la sgte

	permisoDescargar.wait(); // espero a que lavarropas lave y pueda salir
	saquenmeDeAquí();
	ropaDescargada.signal(); // aviso que se descargo
}
```

### Ejercicio 19

Es igual al problema de los autos que cruzan un puente en ambas direcciones visto en clase.

#### Inciso A

```c
sem cuerdaEnUso = sem(1); // impide que cruce un mono si hay otro en dirección contraria
sem capacidad = sem(5);
sem mutex = [sem(1), sem(1)] 
int monosCruzando = [0, 0]
int consecutivos = [0, 0]

// dir (direccion) debe ser 0 o 1
monoCruzarCuerdaUngaBunga(dir){

    mutex[dir].wait()
    if (monosCruzando[dir] == 0)
        // es el primer mono en cruzar en dicha direccion
        cuerdaEnUso.wait();
    
    if(consecutivos[dir] >= limiteConsecutivos)
        direccionPrior[dir].wait()

    capacidad.wait();
    // si llegué acá es porque no hay monos de la otra direccion cruzando (o el ultimo en hacerlo ya termino)
    monosCruzando[dir]++;
    consecutivos[dir]++;
    mutex[dir].signal();

    cruzar();

    mutex[dir].wait();
    monosCruzando[dir]--;
    capacidad.signal();
    if (monosCruzando[dir] == 0)
        // si fue el ultimo en cruzar, habilita a la otra direccion
        cuerdaEnUso.signal();
    mutex[dir].signal();

    monosEnDireccion[dir].signal();
}
```

#### Inciso B

```c
sem cuerdaEnUso = sem(1); // impide que cruce un mono si hay otro en dirección contraria
sem monosEnDireccion = [sem(5), sem(5)]
sem mutex = [sem(1), sem(1)] 
int monosCruzando = [0, 0]
int consecutivos = [0, 0]

sem priorizoDireccion = [sem(1), sem(1)]

// dir (direccion) debe ser 0 o 1
monoCruzarCuerdaUngaBunga(dir){

    monosEnDireccion[dir].wait();

    mutex[dir].wait()
    if (monosCruzando[dir] == 0)
        priorizoDireccion[dir].wait();
        cuerdaEnUso.wait();
    monosCruzando[dir]++;
    consecutivos[dir]++;

    mutex[dir].signal();

    cruzar();

    mutex[dir].wait();
    monosCruzando[dir]--;
    if (monosCruzando[dir] == 0)
        // si fue el ultimo en cruzar, habilita a la otra direccion
        cuerdaEnUso.signal();
    mutex[dir].signal();

    monosEnDireccion[dir].signal();
}
```