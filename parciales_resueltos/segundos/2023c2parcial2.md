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



- Se tiene que leer por entrada estándar, valores enteros positivos, T1, T2.
- Si la balanza detecta que el peso es mayor a T2, entonces hay que detener la cinta porque no soporta más peso. Como esta situación es problemática, hay que avisar por el beeper con el valor EXCEEDED_WEIGHT.
- Si el peso está entre T1 y T2, el motor tiene que ir a una velocidad lenta.
- Si el peso es menor que T1, el motor tiene que ir a una velocidad rápida.
- Si la balanza da 0, se tiene que apagar el motor ya que no hay paquetes que transportar.
- Si la balanza da un resultado negativo, significa que hubo un inconveniente. Hay que volver a leer 10 veces interrumpidamente, una cada 100ms. Asumir que la computadora cuenta con timer interno que podemos usar con interrupciones. Si en alguna de las 10 veces retorna un resultado válido, lo tomamos como el valor correcto. Si en ningún caso nos devuelve algo válido, hay que avisar por el beeper el valor BROKEN_B.  

```c++
//ssize_t write(int fd, const void *buf, size_t count);
//ssize_t read(int fd, void *buf, size_t count);

int CINTA_STOP = 0;
int SLOW_SPEED = 1;
int FAST_SPEED = 2;
sem sem100 = sem(0);

void handler_clock(int signal) {
    sem100.signal();
}

int user_program(t1, t2){
    //  declaramos handler...
    signal(CLOCK, handler_clock);

    int balanza = open("/dev/balanza");
    int cinta = open("/dev/cinta");
    int beeper = open("/dev/beeper");
    int timer = open("/dev/timer");
    
    int peso = 0;
    while(true) {
        
        read(balanza, &peso, sizeof(peso));

        if (peso < 0) {
            int i = 0;
            int ms_to_wait = 100;
            //  retry de 10 veces
            while(i++ < 10){
                sem100.wait();
                
                read(balanza, &peso, sizeof(peso));
                if ( peso > 0 ) break;
            }
            // hubo inconveniente!
            if(i==10) {
                write(beeper, &BROKEN_B, sizeof(BROKEN_B));
                // terminamos
                break;
            }
        }

        if (peso < t1) {
            write(cinta, &FAST_SPEED, sizeof(FAST_SPEED));
        }
        else if (peso > t2) {
            write(cinta, &CINTA_STOP, sizeof(CINTA_STOP));
            write(beeper, &EXCEEDED_WEIGHT, sizeof(EXCEEDED_WEIGHT));
        }
        else if (t1 <= peso <= t2) {
            write(cinta, &LOW_SPEED, sizeof(LOW_SPEED));
        }
        else if(peso == 0){
            write(cinta, &CINTA_STOP, sizeof(CINTA_STOP));
        }
    }
}
```

```c++
//BALANZA:
// - registros:
//    - PESO
//  asumimos que esta siempre prendida...
// - funciones:
//  - init:
//  - read
int balanza_init(){
    return IO_OK;
}

//  copy_to_user(char *to, char *from, uint size)
//  copy_from_user(char *to, char *from, uint size)
int balanza_read(void* data, int size){
    int weight = IN(PESO);
    copy_to_user(data, weight, size);
    return IO_OK;
}

//BEEPER:
//  - registros:
//      - STATUS
//  - funciones:
//      - 
//  - semaforos?
//

void handler_termino_de_sonar(){
    beeper_sem.signal();
}

int beeper_init() {
    //  damos de alta el mutex para lograr exclusion mutua... es decir,
    //  no se pisen las señales de beep.
    sem beeper_sem = sem(1);

}

int beeper_read(void* data, int size){
    //  capaz... simple...
}

int beeper_write(void* data, int size){
    int beeper_signal = 0;
    copy_from_user(&beeper_signal, data, size);

    beeper_sem.wait();
    OUT(STATUS, beeper_signal);
}


// MOTOR DE CINTA:
//  - registros:
//      - VELOCIDAD: enum{SLOW, FAST}
//      - STATUS:    enum{ON, OFF}
//  
//  - funciones:
//      - write
//      - init ?? con semaforos?
//      


// UTIL:
//  - La cinta tiene 3 velocidades
//  - Si la cinta tiene mucho peso entonces hay que bajar la velocidad
//  - Hay un beeper que avisa cuando hay un inconveniente con la balanza
//  - 

```