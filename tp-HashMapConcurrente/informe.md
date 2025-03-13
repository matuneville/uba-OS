# informe

## Ejercicio 1

En el informe, respondan brevemente y justifiquen: Qué significa que la lista sea atómica? Si un programa utiliza esta lista atómica ¿queda protegido de incurrir en condiciones de carrera? ¿Cómo hace su implementación de insertar para cumplir la propiedad de atomicidad?

- que la lista sea atomica significa que para sus operaciones atómicas, el resultado obtenido de manera concurrente será el mismo que el que se debería obtener de manera secuencial, previniendo así que otro thread escriba/lea al mismo tiempo que se está modificando el valor en cuestión, asegurando así un resultado correcto.

- Si un programa utiliza esta lista atómica ¿queda protegido de incurrir en condiciones de carrera?¿Cómo hace su implementación de insertar para cumplir la propiedad de atomicidad? Sí, ya que la zona en la que se modifica el valor de la cabeza está protegida por un mutex, resultando en que no puede modificarse en dos procesos al mismo tiempo. Un caso que daría lugar a race conditions si no estuviera el mutex, sería si dos threads entran a la vez a las lineas de if else, modificando a la vez la propia cabeza de la lista.

utilizamos la clase mutex tq nos permite proteger el insert de multiples accesos simultaneos por distintos hilos.




