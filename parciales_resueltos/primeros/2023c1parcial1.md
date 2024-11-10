## Ejercicio 1

```python
ordenDeSalida = queue([])
esperoASalir = [sem(0)]*N

def carrito(i):
    while True:
        # cargar
        mutexCarga.wait()
        for k in range C:
            subirPersona()
        # seteo mi orden en la queue
        ordenDeSalida.enqueue(i)
        mutexCarga.signal()

        # viajan de manera concurrente
        viajar()
            
        # llegan a la zona de descarga en orden
        mutexDescarga.wait()
        if ordenDeSalida.head != i:
            mutexDescarga.signal()
            esperoASalir[i].wait()
            mutexDescarga.wait()
            for k in range C:
                bajarPersona()
            ordenDeSalida.dequeue()
            esperoASalir[ordenDeSalida.head].signal()
            mutexDescarga.signal()
        else:
            for k in range C:
                bajarPersona()
            ordenDeSalida.dequeue()

            if ordenDeSalida.notEmpty():
                esperoASalir[ordenDeSalida.head].signal()
                mutexDescarga.signal()

        


def personas():
    subir()
    # le aviso al carrito que ya subi
    esperoPersonaSubir.signal()

    esperoPersonaBajar.wait()
    bajar()
```