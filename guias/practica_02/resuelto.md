# Resueltos práctica 2: Scheduling

### Ejercicio 2

Sean P0, P1 y P2 tales que:
- P0 tiene ráfagas cortas de E/S a ciertos dispositivos.
- P1 frecuentemente se bloquea leyendo de la red.
- P2 tiene ráfagas prolongadas de alto consumo de CPU y luego de escritura a disco.

Para planificar estos procesos, ¿convendría usar un algoritmo de Round Robin? ¿convendría usar uno de prioridades? Justifique su respuesta.  

#### Resolución

Si usamos RR, garantizamos un uso justo del CPU para cada proceso dándoles un tiempo fijo a cada uno (quantum). P0 probablemente deba utilizar varios quantums ya que ráfagas cortas de E/S implican largas ráfagas de CPU, y es probable que se bloquee seguido esperando a que las operafiones de E/S terminen. Lo mismo P1, que se bloquea a menudo, y es conveniente no estancarse en el y continuar con los otros procesos para liberar uso de CPU. P2, en cambio, debido a su largo uso de CPU aprovechará al máximo su ejecución y le tomará varios quantums.

Por otro lado, con un scheduling de prioridades, P0 y P1 tendrían mayor prioridad que P2 pues interactúan con E/S. Esto implica que los waiting times de los procesos serán mayores debido a los bloqueos de los dos primeros, y también existirá la posibilidad de starvation del P2 en caso de que los mas prioritarios vuelvan a un estado ready.


### Ejercicio 4
¿Cuáles de los siguientes algoritmos de scheduling pueden resultar en starvation (inanición) y en qué condiciones?
1. Round-robin: No, ya que cada proceso se ejecuta de manera justa con un quantum predeterminado.
2. Por prioridad: Sí, puede haber starvation para un proceso P si siempre está ready algún otro proceso Q tal que la prioridad de Q es mayor que la de P. La adición del _aging_ solucionaría esto.
3. SJF: Sí, pues es un caso particular de scheduling por prioridad. Además, si el scheduling es preemptive, el SO puede interrumpir y reemplazar el proceso actual por otro que tarde menos ráfaga de CPU, empeorando la situación de starvation.
4. SRTF: Sí, es el caso de SJB preemptive de arriba jeje.
5. FIFO: No, pero sí puede ocurrir un efecto convoy si los primeros procesos entrantes requieren prologadas ráfagas de CPU, acaparando el tiempo y haciendo que otros procesos cortos puedan ejecutarse por muy poco tiempo hasta que el proceso largo acapare nuevamente.
6. Colas de multinivel: Sí, puede haber starvation de colas en caso de que siempre haya procesos de una misma cola ready para ejecutarse, haciendo que los procesos que estén en las colas de menor prioridad que la mencionada no se ejecuten.
7. Colas de multinivel con feedback (aging): No, pues esto soluciona el problema mencionado arriba.


### Ejercicio 9

- FCFS:
```c
| P1 |      P2      | P3 |      P4      |
0    1              11   12             22
```
Waiting Time promedio = 0 + 1 + 11 + 12 / 4 = 6  
Turnaround Promedio = 1 + 11 + 12 + 22 / 4 = 11.5  

- RR con quantum 10:
```c
| P1 |      P2      | P3 |      P4      |
0    1              11   12             22
```
Waiting Time promedio = 0 + 1 + 11 + 12 / 4 = 6  
Turnaround Promedio = 1 + 11 + 12 + 22 / 4 = 11.5  


- SJF:
```c
| P1 | P3 |      P2      |      P4      |
0    1    2             12             22
```
Waiting Time promedio = 0 + 1 + 2 + 12 / 4 = 3.75  
Turnaround Promedio = 1 + 2 + 12 + 22 / 4 = 9.25


### Ejercicio 14

Un sistema que atiende tareas interactivas de varias sucursales bancarias está conectado en forma directa a la central policial y, frente a un caso de robo, genera un proceso que activa una alarma en la central.  

Diseñar un algoritmo que permita que, una vez generado ese proceso de alarma, tenga prioridad sobre el resto de las tareas (recordar que pueden generarse distintas alarmas desde distintas sucursales).  

_Nota: Especificar claramente la forma de administración de las colas._  

#### Resolución

Podemos tener un modelo de scheduling basado en colas de prioridad:
- La cola de mayor prioridad, que administrará la ejecución de los procesos que activan alarmas. Trabaja con un First-come First-served (es decir, según el orden de los casos de robo en el tiempo), y se añade un nuevo proceso a la cola ready cuando se dispara ante un caso de robo.
- Una cola de menor prioridad, que administrará los procesos interactivos. Al ser interactivos, trabajarán en ráfagas cortas de CPU, y podemos utilizar, por ejemplo, un Round-Robin tal que los procesos terminen en el quantum correspondiente antes de bloquearse. En caso de que los procesos interactúen más, o menos, (no está aclarado en el enunciado), podríamos tener varias colas de prioridad con feedback que reemplacen a esta misma (manteniendo la de mayor prioridad siendo la de alarmas), en donde los procesos van modificando su cola según la prioridad, determinada por la cantidad de interacciones en una unidad de tiempo.

### Ejercicio 15
Se tiene un sistema donde hay trabajos interactivos y de procesamiento de datos. Los de procesamiento de datos leen archivos inmensos, hacen pequeñas cuentas y los vuelven a grabar.  
Se desea que los usuarios interactivos tengan la sensación de buen tiempo de respuesta, pero sin perjudicar excesivamente el throughput del sistema.  
El scheduler puede funcionar con round-robin o con FCFS. ¿Qué política utilizaría y por qué? Justificar especialmente por qué la política elegida permite cumplir con ambos objetivos del sistema.