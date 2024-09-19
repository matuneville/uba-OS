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

### Ejercicio 11

a) Schediling Multilevel feedback queues, con dealojo:
```
Cola 1: procesos 1 y 2, llegan en t=0, RR quantum = 1, rafagas 4 y 3
    _____________________________
p:  | 1 | 2 | 1 | 2 | 1 | 2 | 1 |
    ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
t:  0   1   2   3   4   5   6   7
```

```
Cola 2: procesos 3 y 4, llegan en t=0 y t=10, FCFS, rafagas 8 y 5
    ___________________________
p:  |       3       |    4    |
    ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
t:  7              15        20
```


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

#### Resolución

Podemos asumir que los procesos de procesamiento de datos consumen ráfagas cortas de CPU pues hacen pequeñas cuentas, asumiendo que de la lectura de datos se engarca algún controlador de memoria, y durante ese tiempo no sería necesario que el proceso consuma CPU. Por otro lado, los procesos interactivos también tendrán poco consumo de CPU ya que van a pasar más tiempo interactuano en E/S.  

Podemos decir entonces que con un RR de quantum corto (lo suficiente como para que los procesos interactivos no se bloqueen y terminen de ejecutarse en un quantum) tendremos un buen throughput, y buenos wait times para los usuarios (corto tiempo de respuesta), y mientras tanto los procesos de lectura de datos esperaremos que terminen en poco tiempo y el quantum seleccionado sea una opción eficaz. Si utilizaramos un FCFS, corremos riesgo de que los procesos de lectura se queden trabajando con disco (sabiendo que leer y escribir memoria es expensive) y ocupen CPU innecesariamente.


### Ejercicio 16

Aquí tienes el enunciado ajustado para que puedas copiar y pegar en Markdown:

Una seriografía es una técnica para el estudio de los órganos en movimiento. Se realiza utilizando un aparato llamado seriógrafo, que ejecuta varias radiografías por segundo y muestra en una pantalla una serialización digital de estas imágenes, dando como resultado una especie de video.

Existen seriógrafos que permiten editar algunas características de las imágenes a medida que se van generando, mientras se está llevando a cabo el estudio médico. Entre otras cosas, permiten ajustar el brillo y el contraste de las imágenes, y hacer zoom-in y zoom-out. Así, se permite una edición “en vivo” del video.

Se tienen entonces los siguientes procesos:
- Uno que genera las imágenes digitales a partir de los valores resultantes al irradiar al paciente.
- Uno que responde a los botones de ajuste de brillo y contraste.
- Uno que responde a los botones de ajuste de zoom.
¿Qué política de scheduling permite esta toma y edición de imágenes “en vivo” de manera eficiente? Justificar.

#### Resolución

Creo que el enunciado quiere hacer mucho énfasis en que los procesos de edición, que a priori se ven como poco caros en CPU, con ráfagas cortas ya que serían solo ajustar algunos valores, tengan una mayor prioridad que el de renderización de imágenes. Según este esquema, podemos basar el scheduling en dos colas de priodidad:

- La primera, de prioridad 0, que contenga a los procesos de edición de video, y trabajando con un FCFS ya que no habría problema de convoy effect pues los procesos no se los ve que consuman mucho tiempo de CPU.
- La segunda, de prioridad 1, que renderice la imágen a partir de los ajustes de edición de video, que ya fueron procesados al tener mayor prioridad y en consecuencia mejor tiempo de respuesta.

Agrego comentario de Joni: "No necesitamos utilizar una política con desalojo pues si se presiona un botón durante la generación de una imagen no tendría sentido modificar ese parámetro inmediatamente pues obtendríamos, por ejemplo, una imagen partida en donde una porción tiene un brillo y otra porción otro brillo distinto."

### Ejercicio 17

Se tiene un sistema de vigilancia que utiliza cámaras y alarmas. Estos dispositivos se comunican con un servidor que atiende distintos tipos de procesos con una política de scheduling que deberá desarrollarse para este escenario particular.  

Los módulos de procesamiento de video son procesos que leen el stream de video que llega desde una cámara y luego corren un algoritmo de detección de objetos. A su vez, estos procesos persisten las secuencias de video en discos del servidor. Para este tipo de procesos se quiere evitar situaciones de scheduling poco “justas”.  

En caso de detectar patrones considerados riesgosos, el sistema debe alertar a los operadores para que actúen de inmediato. Para este fin, se cuenta con un módulo de alarma que se lanza en un proceso y que gestiona su activación. Es crítico poder garantizar que cualquier alarma se active dentro de un deadline estricto que no puede perderse.  

Por otro lado, el servidor cuenta con suficiente espacio para almacenar temporalmente muchas horas de frames en un formato “crudo” de video de todas las cámaras. Sin embargo, periódicamente se lanzan procesos que levantan grandes volúmenes de video grabados durante el día y le aplican un algoritmo de compresión que permite luego reemplazar las secuencias originales por otras de mucho menor tamaño. Estos procesos son lanzados durante la noche, cuando las áreas se encuentran mucho menos transitadas, por lo que las cámaras se configuran para transmitir sólo en caso de detección de movimiento, así que la carga de procesos de procesamiento activos de video es muy baja y en forma de ráfagas de corta duración.

#### Resolución

Utilizaremos una política de scheduling de colas de prioridad (sin feedback):

- Cola de mayor prioridad, que administrará el disparo de alarma a operadores, ya que deben ser alertados de inmediato, puesto que es pertinente que las alarmas se activen dentro de un deadline estricto. Esta cola debe trabajar con un algoritmo de scheduling para Real Time, posiblemente con una política de Earliest-Deadline-First (mencionado en la teórica muy por arriba). Si no, podría también utilizarse un FCFS, si asumimos que las ráfagas de CPU utilizadas por los módulos de activación de alarmas no son muy largas.

- Cola de prioridad intermedia, que administrará los procesos generados por los módulos de procesamiento de video, que leen el stream de video que llega desde una cámara y luego corren un algoritmo de detección de objetos. Aquí no tengo claro qué politica usar sobre la cola; tal vez un RR o un FCFS funcionaría bien, ya que debemos intercalar de manera eficiente entre el procesamiento de video y la detección de objetos, y si ambos son procesos de duración corta y similar en CPU, las políticas mencionadas tendrían un buen efecto. Otras políticas no funcionarían bien, como el caso de determinar prioridad a alguna de ambos tipos de tareas, o utilizar un Shortest-Job-First, ya que es pertinente la buena detección de movimiento sobre el procesamiento de video en tiempo real, y si uno de ambos tiene prioridad, se estaría trabajando con video viejo, o procesando video sin detectar movimiento. Esto empeoraría si le añadimos desalojo.

- Cola de menor prioridad para almacenamiento de videos y compresión, ya que son tareas periódicas y pesadas que se ejecutarán en momentos que no requieren mucha atención en otros procesos. Acá considero que no es tan pertinente elegir alguna política de scheduling que destaque por sobre otras, teniendo en cuenta que se ejecutan esporádicamente y en momentos en que las colas de mayor prioridad estén "tranquilas". Además, al tener una carja baja y de ráfagas cortas de CPU, no habría peligro de starvation o convoy effect. Vamos a elegir como schedulling un Shortest-Job-First (por decir alguno).

