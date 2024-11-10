
### Ejercicio 1

Filesystem basado en inodos:
1. Pseudocodigo para acceder al archivo `A/B/c.txt` teniendo el inodo del directorio A

```python
inode_ext2 dir_inode_B = None
inode_ext2 file_inode_c = None

uint* blocks_A = dir_inode_A->blocks
uint offset = 0

# recorremos todos los bloques de A
for i in range(TOTAL_BLOCKS):
	# """Salvedad""" del ultimo bloque...
	uint block1_addr = get_block_address(i)
	uint block2_addr = get_block_address(i+1)
	
	# cargamos 2 bloques para leer dir entries
	# (para leer un dir entry entero si queda cortado en 2 blocks)
	uchar buffer_blocks[2 * BLOCK_SIZE]
	read_block(block1_addr, buffer_blocks)
	read_block(block2_addr, buffer_blocks + BLOCK_SIZE)

	# recorremos el primer bloque
	while offset < BLOCK_SIZE:
		uchar* dir_entry_ptr = buffer_blocks + offset
		dentry_ext2 dir_entry = (dentry_ext2)dir_entry_ptr

		if dir_entry->name == 'B' and dir_entry->type == DIR:
			# encontre el inodo del directorio B
			dir_inode_B = dir_entry->inode
		offset += dir_entry->record_length

	offset -= BLOCK_SIZE

/*
repito proceso para el directorio B!
pero ahora el if final queda asi:
*/

	# ...
	if dir_entry->name == 'c.txt':
		# encontre el inodo del directorio B
		file_inode_c = dir_entry->inode
		break

return file_inode_c
```

2. Cuántas escrituras a disco requiere renombrar `/home/juan/listado.txt` a `/home/maria/archivo.txt` ?
	- Tenemos que recorrer nuestro filesystem: arrancamos por el inodo directorio /root/, buscamos el directorio /home/, y una vez en él buscamos entre sus bloques el dir entry al inodo directorio `/juan/` y le cambiamos su nombre a `maria` al dir entry (1 escritura a disco). Nuevamente, recorremos el directorio ahora llamado `/maria/` hasta encontrar el dir entry del archivo `listado.txt`, lo renombramos a `archivo.txt` (2 escrituras a disco) y listo. Total: 2.

3. Sí, haciendo que uno de los dir entries de un inodo sea el de un archivo que sea el mismo. O sea, que apunte a al mismo archivo. O algo asi xdd


### Ejercicio 2

```c
int main(void) {
    cambiarPassword();
    return 0;
}

void cambiarPassword() {
    int numeroDeUsuario = obtenerUsuarioActual();

    char password[250];
    char passwordConfirmacion[250];
    printf("Ingrese su password actual");
    fgets(password, 250, stdin);

    if (hash(password) == hashDePasswordActual(numeroDeUsuario)) {
        printf("Ingrese su nueva password");
        fgets(password, 250, stdin);
        printf("Confirme su nueva password");
        gets(passwordConfirmacion);

        if (sonIguales(password, passwordConfirmacion)) {
            actualizarPassword(numeroDeUsuario, password);
        }
    }
}
```

```
0x0000             -
 _________________
|_________________|
|____passConf_____| [0,...,7]
...
|____passConf_____| [,...,256]
|______pass_______| [0,...,7]
...
|______pass_______| [,...,256]
|_____num_usr_____| 
|_______EBP_______|
|_EIP (ret addr)__|

0xFFFF             +
```


1. Explicar problema de seguridad y cómo explotarlo
   - Problema: posible buffer-overflow en stack. 
   		Puede ser explotado para cambiarle la contraseña a otro usuario (e incluso modificar la direccion de return)
   - cómo explotarlo?
		la forma de explotarlo es escribiendo un payload largo en 'gets(passwordConfirmacion);' tq escribimos la pass que queremos,
		reescribimos la pass que pusimos antes y reescribimos el num_usr tq esto nos permite cambiar la contraseña de otro usuario
		con una que queramos nosotros. De esta manera, podemos obtener acceso a la cuenta de cualquier usuario ya que le podemos
		cambiar la contraseña aprovechando este exploit.

  
2. Que hay que hacer con las contraseñas para explotar la vulnerabilidad
   - Si ingresa el usuario en el `gets()` algo de largo 256*2+8 bytes, pisamos la contraseña a confirmar y la anterior (debe ser dos veces la misma de largo 256 así ingresamos al if), y luego, en los otros 8B, agregar el número de usuario al que queramos ""hackear"". El programa ejecutará `actualizarPassword(numeroDeUsuario, password);`,, modificándole la contraseña que ingresamos nosotros, no a nuestro usuario, sino al que modificamos ilegalmente en el mediante el overflow.


### Ejercicio 3

Considere los siguientes elementos:

1. La pantalla táctil de un celular con Android.
   - a) No tiene sentido, ya que una pantalla debe dar sensación de rapidez de respuesta. O sea, requiere una instantánea escritura y lectura
   - b) Interrupciones es la mejor opción en este caso, ya que las interacciones no son constantes como para requerir un polling (ejemplo, mientras miramos un video de yt no usamos tanto el táctil)

2. Un disco virtual, que permite a los usuarios guardar archivos y sincronizarlos en una carpeta remota mediante la red (al estilo Dropbox, Google Drive).
   - a) Sí, tiene sentido si el usuario tiende a subir grandes cantidades de archivo de manera continua en poco tiempo
   - b) Probablemente algun tipo de interrupcion mediante notificaciones virtuales cuando se realizan determinadas acciones para una sincronización correcta. Además de que se sincronice cada vez que se abre la aplicación desde un dispositivo.

3. Un GPU (Unidad de Procesamiento Gráfico) que se utiliza para multiplicaciones de matrices de cálculos científicos.
   - a) Depende: en contextos de procesamiento intensivo que no requiere pasar a CPU o a memoria de manera rápida lo procesado por la GPU, un buffer es una buena opción (ejemplo, cuando se entrena un modelo de deeplearning), o lo mismo para renderización de videos. En cambio, para renderización en tiempo real como videojuegos, se requiere escribir rápido a pantalla, por lo que la rápida escritura/lectura entre GPU y pantalla es indispensable y no habria que usar un buffer.
   - b) Se utiliza DMA para la E/S de la GPU porque permite transferir grandes volúmenes de datos entre la memoria principal y la GPU sin la intervención constante de la CPU, lo cual libera recursos de la CPU y mejora el rendimiento al hacer las transferencias de datos más rápidas y eficientes.

En base a ellos, responda y justifique las siguientes preguntas, para cada caso:

a) ¿Tiene sentido que el driver use un buffer para almacenar una mayor cantidad de entrada del usuario o la salida antes de enviarla / recibirla del dispositivo, o conviene intercambiarla en cuanto esté disponible?  

b) ¿Qué mecanismo de entrada/salida utilizaría?
