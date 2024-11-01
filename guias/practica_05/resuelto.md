# Guia 5 - Drivers y E/S

### Ejercicio 1

¿Cuáles de las siguientes opciones describen el concepto de driver? Seleccione las correctas y justifique.
- Es una pieza de software: Sí, un driver es un programa que busca conectar la comunicación entre hardware y otros softwares
- Es una pieza de hardware: No, este podria ser un controlador
- Es parte del SO: Sí, según Baader: SO = Kernel + Drivers
- Dado que el usuario puede cambiarlo, es una aplicación de usuario: No, son programas que corren al nivel de privilegios del kernel, ya que deben poder comunicarse con el hardware
- Es un gestor de interrupciones: No, aunque pueden trabajar con interrupciones
- Tiene conocimiento del dispositivo que controla pero no del SO en el que corre: No, definitivamente sí tienen conocimientos del SO, aunque lo justo y necesario, como la API para su comunicación con el mismo
- Tiene conocimiento del SO en el que corre y del tipo de dispositivo que controla, pero no de las particularidades del modelo específico: No, debe conocer muy bien al hardware que comunicará al SO para saber sus funciones


### Ejercicio 3

```c
semaphore mutex;

int driver_init(){
	// inicializo el semaforo asi solo 1 proceso puede acceder a lectura a la vez
	sema_init(&mutex, 1);
	return IO_OK;
}

int driver_read(int *data){
	// evito que otro proceso pueda leer la tecla mientras estoy yo
	sema_wait(&mutex);

	// ahora hacemos busy waiting esperando al bit0 == 1, o sea, que sea pulsada
	while(True){
		int status_boton = IN(BTN_STATUS);
		if(status_boton & 0x00000001 != 1) continue;
	}

	// limpio el estado de la tecla.
    // 1101b = 0xD
    OUT(BTN_STATUS, status_boton & 0xFFFFFFFD);

    sem_signal(&mutex);

    int result = BTN_PRESSED;
    copy_to_user(data, &result, sizeof(result));

    return IO_OK;
}
```


### Ejercicio 4

```c
semaphore mutex;
semaphore pressed;

void handler_int_7(){
	sem_signal(&pressed);
}

int driver_remove(){
	free_irq(7); return 0;
}

int driver_init(){
	sema_init(&mutex, 1);
	sema_init(&pressed, 0);

	// asocio handler a interrupcion
	request_irq(7, handler_int_7);

	// Para indicar al dispositivo que debe efectuar una nueva interrupción al detectar
	// una nueva pulsación de la tecla, debe guardar la constante BTN_INT en el registro de la tecla.
	OUT(BTN_STATUS, BTN_INT);

	return IO_OK;
}

int driver_read(int *data){
	sema_wait(&mutex);

	// aguardo a que presionen la tecla
	sema_wait(&pressed);

	// restauro el estado de la tecla para que mande otra interurpcion
    OUT(BTN_STATUS, BTN_INT);

    sema_signal(&mutex);

    // aviso al usuario que la tecla fue presionada
    int result = BTN_PRESSED;
    copy_to_user(udata, &result, sizeof(result));

    return IO_OK;
}
```


### Ejercicio 5
Indicar las acciones que debe tomar el administrador de E/S:
- a) cuando se efectúa un `open()`:
	- **Solicitar interrupciones (si aplica)**: Usar `request_irq()` para asociar una función de manejo de interrupciones, si el dispositivo las necesita.
	- **Reservar memoria**: Utilizar `kmalloc()` para asignar espacio de memoria en caso de que el dispositivo necesite estructuras de datos para su manejo.
	- **Inicializar semáforos**: Llamar a `sema_init()` para inicializar un semáforo que permita el acceso seguro al dispositivo en operaciones concurrentes.
	- **Mapear memoria**: Si el dispositivo necesita un mapeo de memoria, llamar a `mem_map()` para hacer accesible la memoria del dispositivo al proceso.
- b) cuando se efectúa un `write()`:
	- **Asegurar acceso exclusivo**: Utilizar `sema_wait()` para bloquear el acceso a otros procesos mientras se escribe.
	- **Escribir al dispositivo**: Llamar a `OUT()` para escribir el dato en el registro de E/S del dispositivo (`BTN_STATUS`, en este caso) para actualizar su estado.
	- **Realizar operaciones adicionales (como resetear)**: Usar `OUT()` con el valor adecuado en el bit correspondiente para resetear el estado, si el dispositivo requiere una limpieza posterior a cada uso.
	- **Liberar el acceso**: Usar `sema_signal()` para liberar el semáforo, permitiendo que otros procesos puedan acceder al dispositivo.


### Ejercicio 7
a)
```c

semaphore mutex;

int driver_init(){
    sema_init(&mutex, 1);
    return IO_OK;
}

int write(int sector, void *data){
	sema_wait(&mutex);

	// aca no entiendo bien como hacerlo asi que me copio de joni
	int pista = sector / cantidad_sectores_por_pista();
    sector = sector % cantidad_sectores_por_pista();

	// si el motor esta apagado, lo prendo
    if (IN(DOR_STATUS) == 0){
	    OUT(DOR_IO, 1);
	}
	// espero un rato a que tome velocidad el disco
    sleep(50);

	// el brazo ahora debe apuntar a la pista correcta
    OUT(ARM, pista);
    // espero a que el brazo termine de moverse y llegue a la pista correcta
    while(IN(ARM_STATUS)!=1){
	    pause();
    };

	// selecciono al sector correcto dentro de la pista
	OUT(SEEK_SECTOR, sector);
	// no tengo ningun registro para comprobar si ya esta listo

	// ahora escribimos la data, primero la traemos a kernel y luego a disco
	int* data_kernel;
	copy_from_user(data_kernel, data, sizeof(data_kernel));
	escribir_datos(data_kernel);
	// espero a que se envie el dato
	while(IN(DATA_REDY)!=1){
		pause();
	}
	// termine, apago el motor y fuerzo la espera de 200ms a que termine de apagarse,
	// antes de lo cual no es posible comenzar nuevas operaciones
	OUT(DOR_IO, 0);
	sleep(200);

	sema_signal(&mutex);
	return IO_OK;
}
```

b)
```c
#define INT_DISCO 6
#define INT_TIMER 7

// esto me lo copio de joni
// Sumamos un tick extra del timer para garantizar que esperamos el tiempo deseado.
// Cuánto tiempo extra esperamos depende que tan lejos estamos de la próxima
// interrupción del timer cuando hacemos wait del semáforo sleep_done.
#define TIMER_PERIOD 50
#define TICKS_MOTOR_ON 50 / TIMER_PERIOD + 1
#define TICKS_MOTOR_OFF 200 / TIMER_PERIOD + 1

int tick_count = 0;

semaphore mutex;
semaphore arm_sem;
semaphore data_sem;
semaphore sleep_timer;

// cuando ocurre la interrupción, veo que registro tomó valor 1 y activo el mutex correspondiente
void handler_disco(){
	if(IN(ARM_STATUS) == 1){
		sema_signal(&arm_sem);
	}
	else{
		sema_signal(&data_sem);
	}
}

// cada 50ms ocurre la interrupcion, el handler hace los sgte:
//     si estabamos esperando, tick_count > 0 -> resto 1 y al llegar a 0 mando señal de tiempo cumplido
void handler_timer(){
    if (tick_count > 0 && --tick_count == 0) {
        sem_signal(&sleep_timer);
    }
}


int driver_init(){
	// inicializo mutexes
    sem_init(&mutex, 1);
    sem_init(&sleep_timer, 0);
    sem_init(&arm_sem, 0);
    sem_init(&data_sem, 0);

	// seteo interrupciones
    request_irq(INT_DISCO, handler_disco);
    request_irq(INT_TIMER, handler_timer);
    
    return IO_OK;
}

int driver_remove(){
	// libero interrupciones
    free_irq(INT_STATUS);
    free_irq(INT_TIMER);
    return IO_OK;
}

int driver_write(){
	sema_wait(&mutex);

	int pista = sector / cantidad_sectores_por_pista();
    sector = sector % cantidad_sectores_por_pista();

	// si el motor esta apagado, lo prendo
    if (IN(DOR_STATUS) == 0){
	    OUT(DOR_IO, 1);
	}
	// espero a que arranque bien y tome velocidad
	tick_count = TICKS_MOTOR_ON; // 100ms
	sema_wait(&sleep_timer);

	// el brazo ahora debe apuntar a la pista correcta, y espero a que se ubique bien
    OUT(ARM, pista);
    sema_wait(&arm_sem);

	// selecciono al sector correcto dentro de la pista
	OUT(SEEK_SECTOR, sector);

	// ahora escribimos la data, primero la traemos a kernel y luego a disco
	int* data_kernel;
	copy_from_user(data_kernel, data, sizeof(data_kernel));
	escribir_datos(data_kernel);
	// espero a que termine de escribirse
	sema_wait(&data_sem);

	// apagamo el motor del disco
    OUT(DOR_IO, 0);
    tick_count = TICKS_MOTOR_OFF; // 250ms
    sem_wait(&sleep_timer);

    sem_signal(&mutex);
    
    return IO_OK;
}
```
