### Ejercicio 1
a)
```c++

// la idea es hacerlo recursivo: recorro path, imprimo cada archivo (sea archiv o file)
// - si encuentro files, no hago nada
// - si encuentro dirs, llamo a mi_ls_r(dir)
void my_ls_r(char* path){
	// obtengo primer bloque de tabla de dir entries
	uint first_block = get_first_block_from_path(path);
	// recorro la tabla de dir entries a traves de todos sus bloques
	char* buffer_block = malloc(BLOCK_SIZE);
	while(first_block != EOF){
		// traigo el bloque de dir entries de disco a memoria
		read_block(buffer_block, first_block);
		// recorro el bloque de dir entries
		FATDirEntry* dir_entry_table = (FATDirEntry*) buffer;
		for(int i = 0; i < BLOCK_SIZE / FAT_DIR_ENTRY_SIZE; i++){
			// printeo el nombre
			printf("%s\n", dir_entry_table[i].filename);
			if(dir_entry_table[i].attribute == 0x2){
				// es directorio
				my_ls_r(concat(path, dir_entry_table[i].filename));
			}
		}
		first_block = FAT[first_block];
	}
}

```

### Ejercicio 2

El doctor Augusto Losano, experto en tecnología aplicada a logística, empezó a trabajar en BaaderEnvios, una nueva empresa de distribución de paquetes. Inspirados en el diseño de otras empresas, se está construyendo una central inteligente de transferencias cerca de Ciudad Universitaria. Diferentes camiones recogen los paquetes de sus clientes, llegan a la central y descargan su contenido. Estos paquetes son puestos en cintas transportadoras, donde diferentes personas, al final de su recorrido, agarran los paquetes y los clasifican.  

La cinta transportadora tiene un motor bastante simple y discreto (marca Sunooza Corporation) que tiene la capacidad de regular la velocidad a la cual se mueve la cinta. Cuenta con 3 modos: 0 para nula velocidad, 1 para baja velocidad y 2 para velocidad rápida. También se dispone de una balanza que mide cuánto peso está soportando la cinta transportadora actualmente.  

Un detalle que notó el doctor Augusto, es que si la cinta contiene demasiado peso, se debe regular la velocidad del motor para no forzarlo demasiado.  

Además, desde la última invasión (hace unos meses) de anfibios saboteadores provenientes de los pabellones de la facultad, muchos dispositivos sufrieron daños, y la balanza, cada cierto tiempo, devuelve valores incorrectos. La empresa tiene un dispositivo viejo (un beeper) que avisa a los técnicos cuando se produce un problema para que vengan a revisarlo lo antes posible.  

El doctor Augusto quiere poder coordinar los diferentes dispositivos mediante software con una computadora con Linux y nos encarga a nosotros la tarea.  

Nos pide diseñar un driver para cada dispositivo (balanza, beeper, motor) que cumpla las siguiente características:

- Se tiene que leer por entrada estándar, valores enteros positivos, T1, T2.
- Si la balanza detecta que el peso es mayor a T2, entonces hay que detener la cinta porque no soporta más peso. Como esta situación es problemática, hay que avisar por el beeper con el valor EXCEEDED_WEIGHT.
- Si el peso está entre T1 y T2, el motor tiene que ir a una velocidad lenta.
- Si el peso es menor que T1, el motor tiene que ir a una velocidad rápida.
- Si la balanza da 0, se tiene que apagar el motor ya que no hay paquetes que transportar.
- Si la balanza da un resultado negativo, significa que hubo un inconveniente. Hay que volver a leer 10 veces interrumpidamente, una cada 100ms. Asumir que la computadora cuenta con timer interno que podemos usar con interrupciones. Si en alguna de las 10 veces retorna un resultado válido, lo tomamos como el valor correcto. Si en ningún caso nos devuelve algo válido, hay que avisar por el beeper el valor BROKEN_B.  

El acceso a los dispositivos es lento y consume mucha energía, así que se quiere minimizar los accesos. No podemos usar sleep() ni funciones similares.

Se pide:

a) Proponer un diseño, indicando los registros que tendría cada dispositivo, junto con su utilidad. Indicar y justificar el tipo de interacción que deberá soportar cada dispositivo (interrupciones, polling, etc.). Detallar cuántos drivers deberán ser implementados, qué dispositivo/s manejará cada driver, y qué funciones soportarán los mismos, describiendo su comportamiento en lenguaje natural.  

b) A partir del diseño del punto anterior, escribir los drivers correspondientes y el software de usuario. Escribir en pseudocódigo (estilo C) las funciones mínimas necesarias para poder satisfacer el objetivo planteado. El código deberá ser sintácticamente válido y respetar las buenas prácticas mencionadas durante las clases. Por simplicidad, siempre que esto no impacte en la solución, se permitirá omitir el chequeo de errores. Todas las decisiones implementadas deberán estar debidamente justificadas.

```c++
//ssize_t write(int fd, const void *buf, size_t count);
//ssize_t read(int fd, void *buf, size_t count);

int CINTA_STOP = 0;
int SLOW_SPEED = 1;
int FAST_SPEED = 2;
sem sem100 = sem(0);

int user_program(t1, t2){

    int balanza = open("/dev/balanza");
    int cinta = open("/dev/cinta");
    int beeper = open("/dev/beeper");
    
    int peso = 0;
    while(true) {
        
        int balanza_ret = read(balanza, &peso, sizeof(peso));

		// chequeo si obtuvo peso invalido
        if (balanza_ret == IO_ERROR){
	        write(beeper, &BROKEN_B, sizeof(BROKEN_B));
	        break;
        }

		// si el peso es válido, chequeo qué hacer
        if (peso < t1){
            write(cinta, &FAST_SPEED, sizeof(FAST_SPEED));
        }
        else if (peso > t2){
            write(cinta, &CINTA_STOP, sizeof(CINTA_STOP));
            write(beeper, &EXCEEDED_WEIGHT, sizeof(EXCEEDED_WEIGHT));
        }
        else if (t1 <= peso <= t2){
            write(cinta, &LOW_SPEED, sizeof(LOW_SPEED));
        }
        else if(peso == 0){
            write(cinta, &CINTA_STOP, sizeof(CINTA_STOP));
        }
    }

	return 0;
}
```

```c++
/* ############ Balanza ############*/
// registros: PESO
// variable: time

int handler_timer_100ms(){
	// handler de interrupcion que se dispara cada 100ms
	sem_timer_100ms.signal();
	time -= 100;
	return IO_OK;
}

int balanza_init(){
	request_iqr(IQR_TIMER, handler_timer_100ms);
	sem sem_timer_100ms = sema_init(0);
    return IO_OK;
}

int balanza_remove(){
	free_iqr(IQR_TIMER);
	sem mtx_balanza = sema_init(1);
	return IO_OK;
}


int balanza_read(void* data, int size){
	mtx_balanza.wait();
    int peso = IN(PESO);

	// chequeo si es invalido
	if(peso < 0){
		int i = 0;
		int ms_to_wait = 100;
		//  retry de 10 veces
		while(i++ < 10){
			sem_timer_100ms.wait();
			peso = IN(PESO);
			if (peso > 0) break;
		}
		if(i==10){
			// hubo inconveniente!
			return IO_ERROR;
		}
    }

	// si no hubo error, le escribimos el peso
    copy_to_user(data, peso, size);
    mtx_balanza.signal();
    return IO_OK;
}


/* ############ Beeper ############*/
// registros: STATUS: BROKEN_B, EXCEEDED_WEIGHT

void handler_termino_de_sonar(){
    beeper_sem.signal();
    return IO_OK;
}

int beeper_init(){
    //  damos de alta el mutex para que no se pisen las señales de beep
    sem beeper_sem = sema_init(1);
	request_iqr(INT_BEEP, handler_termino_de_sonar);
	return IO_OK;
}

int beeper_remove(){
	free_iqr(IQR_BEEP);
	return IO_OK;
}

int beeper_write(void* data, int size){
    int beeper_signal = 0;
    copy_from_user(&beeper_signal, data, size);
	// espero a que termine de sonar
    beeper_sem.wait();
    OUT(STATUS, beeper_signal);

	return IO_OK;
}


/* ############ Cinta ############*/
// registros: STATUS: ON, OFF
//            VELOCIDAD: STOP, SLOW, FAST

int cinta_init(){
	// arranca ya prendida
	sem mtx_cinta = sema_init(1);
	return IO_OK;
}

int cinta_write(void* data, int size){
    int cinta_speed = 0;
    mtx_cinta.wait();
    copy_from_user(&cinta_speed, data, size);

	if(IN(STATUS) == OFF && cinta_speed != CINTA_STOP){
		OUT(STATUS, ON);
	}

	OUT(VELOCIDAD, cinta_speed);
	
	if(cinta_speed == CINTA_STOP){
		OUT(STATUS, OFF);
	}
	mtx_cinta.signal();

	return IO_OK;
}

```