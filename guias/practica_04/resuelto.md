# Guía 4: File systems

### Ejercicio 2

Info de disco:
- Capacidad: 128 GB
- Bloques: 8 KB
Table entries : 24 B

#### Inciso A

- Almacenamiento disco: `128 GB = 128 * 1024 MB = 128 * 1024 * 1024  KB = 134.217.728`
- Cantidad de bloques en total en el disco: `134.217.728 KB / 8 KB = 16.777.216`
- Cantidad de bloques / tamaño de entries: `16.777.216 * 24 B = 402.653.184 B = 384 MB`
- Tamaño tabla en bloques : `384 * 1024 KB / 8 KB = 49.152`

384 MB será el tamaño de ocupará la tabla en disco, unos 49.152 bloques.

#### Inciso B
- Un archivo ocupa 10 MB
- Bloques necesarios para almacenar un archivo: `10 * 1024 KB / 8 KB (el techo del resultado) = 1280`
- Bloques para almacenar datos en disco: `Bloques en disco - Bloques de tabla = 16.777.216 - 49.152 = 16.728.064
- Bloques para almacenar datos / bloques para guardar un archivo: `16.728.064 // 1280 (el piso) = 13.068`

Es posible almacenar 13.068 archivos de 10 MB.

#### Inciso C

El mapeo de bloques es el siguiente: `20 -> 21 -> 22 -> 3 -> 4 -> 5 -> 0 -> EOF`.  
Por lo tanto, nuestro archivo ocupa 7 bloques. Es decir, ocupará `7 * 8 KB = 56 KB`.


### Ejercicio 3

Sistema similar a `ext2`:
- Bloques de 4 KB
- Direccionamiento a bloques de disco de 8 B
- Cada inodo cuenta con:
	- 5 entradas directas
	- 2 entradas indirectas simples
	- 1 entrada indirecta doble

#### Inciso A

El archivo es representado por un inodo, con el que podemos mapear:
- Las 5 entradas directas nos mapean a 5 bloques de datos, o sea, `5 * 4 KB = 20 KB`
- Una entrada indirecta simple nos mapea a 1 bloque de datos, que a su vez es utilizado para mapear a otros bloques de datos. En este bloque tendremos `Tamaño bloque / tamaño direccionamiento a bloque = 4 * 1024 B / 8 B = 512` direcciones a bloques. Es decir, estas 512 entradas nos mapearán a 512 bloques de datos, que son `512 * 4 KB = 2048 KB` totales, mapeados por una sola entrada indirecta, por lo que con 2 entradas indirectas simples tendremos `4096 KB` de datos a ocupar.
- Una entrada indirecta doble nos mapea a 1 bloque que contiene direcciones de bloques indirectos simples. Vimos que en un bloque caben 512 direcciones, y que una dirección indirecta simple nos da 2048 KB, por lo que el bloque referenciado por la entrada indirecta doble nos da un total de `512 * 2048 KB = 1.048.576 KB = 1024 MB = 1 GB`.  

Por lo tanto, el tamaño máximo de archivos soportado por este sistema es de `1 GB + 4096 KB + 20 KB = 1.052.692 KB ~= 1.00393 GB`, es decir, el máximo tamaño referenciado por un inodo.

#### Inciso B

Vamos a hacer cuentas solo considerando el espacio de bloques de datos (ignorando bloques ocupados por directorios e inodos).  
Tenemos que 50% del disco solo utiliza 2 KB, otro 25% solo 4 KB, y el restante 25% solo 8 KB:
- 50% del disco utiliza `2 KB * 100 / 4 KB* = 50%` de cada bloque. Es decir, el 50% del disco desperdicia 50% de espacio de cada bloque, lo que nos lleva a que el 25% del almacenamiento está siendo desperdiciado.
- 25% del almacenamiento es para archivos de 4 KB (tamaño de bloque), y el restante para 8 KB (tamaño de 2 bloques), por lo que acá no hay desperdicio.

En total, se desperdicia 25% del almacenamiento total.

#### Inciso C

Tenemos que procesar un archivo de 5 MB = 5120 KB:
- Las 5 entradas directas nos permiten almacenar 20 KB, por lo que tenemos 5 bloques ocupados acá para almacenar los primeros 20 KB del archivo.
- Nos restan 5100 KB para almacenar. Las dos entradas indirectas simples nos mapean en total a 4096 KB, y ocuparán `2 * bloques referenciados por cada entrada indirecta = 2 * 512 = 1024` bloques ocupados con esto, + los 2 bloques utilizados para guardar todas estas entradas.
- Nos sobran 1004 KB para almacenar. Necesitamos una entrada indirecta doble, que nos mapee a un bloque con entradas indirectas simples. Cada una de estas últimas nos llevan a 2048 KB, por lo que con una sola entrada indirecta simple sería suficiente, la cual nos debe llevar a `1004 KB / 4 KB (techo del resultado) = 251` bloques para almacenar lo restante. Entonces tenemos 1 bloque (referenciado por la entrada indirecta doble) + 1 bloque (referenciado por una entrada indirecta simple) + 251 bloques (a los que nos llevan las primeras 251 direcciones de las 512 disponibles en el bloque previo). Por lo que tenemos 253 bloques utilizados en esta parte final.

Total: 5 + 2 + 1024 + 253 = 1284 bloques de datos utilizados por el archivo.

### Ejercicio 4

#### Inciso A

Debemos leer el bloque en el que se encuentra el inodo (depende si lo tenemos en memoria o almacenamiento), y luego, como el archivo es de 40 KB y los bloques son de 4 KB, alcanza con leer 10 de los 12 bloques apuntados directamente desde el inodo. Por lo tanto, hay que leer 11 o 10 bloques.

#### Inciso B

- 1 bloque para inodo
- 12 bloques directos (`12 * 4 KB = 48 KB`)
Nos resta mapear a 32 KB, o sea, 8 bloques:
- 1 bloque indirecto simple
- 8 bloques desde el indirecto simple
Total: 22 bloques (21 si el inodo se encuentra en memoria).

### Ejercicio 7

1. Es importante que puedan crearse enlaces simbólicos: **Sistema basado en inodos**, ya que permite el uso de *soft links (symbolic links)*, permitiendo que un archivo apunte a otro sin tener su contenido duplicado en almacenamiento. FAT no permite esto.
2. Es importante que la cantidad de sectores utilizados para guardar estructuras auxiliares sea acotada, independientemente del tamaño del disco: **Sistema basado en inodos**, ya que la tabla de FAT, como direcciona a todo el disco, su tamaño será proporcional al tamaño del mismo, según la granularidad de bloques. En cambio, un sistema basado en inodos se encuentra acotado a un inodo por archivo existente, y cada uno tiene un tamaño acotado.
3. Es importante que el tamaño máximo de archivo sólo esté limitado por el tamaño del disco: **FAT** suena como una opción más lógica para este caso, ya que permite que toda entrada de su tabla direccione a los bloques de un archivo que ocupe todo el disco. Por otro lado, con inodos solo podemos mapear a archivos que ocupen una cantidad de bloques limitado por la cantidad de bloques a la que pueda direccionar el inodo, pudiendo ser menor a la cantidad de bloques de datos del disco.
4. Es importante que la cantidad de memoria principal ocupada por estructuras del file system en un instante dado sea (a lo sumo) lineal en la cantidad de archivos abiertos en ese momento: **Sistema basado en inodos**. Esta opción nos permite tener en memoria los inodos de los archivos abiertos en el momento. En cambio, un sistema FAT requiere tener la tabla en memoria, cuyo tamaño es proporcional al del disco, y no lineal en cuanto a cantidad de archivos abiertos.

### Ejercicio 10

```c++
struct dir_entry{
	int first_block_address;
    int file_size;
    int name_size;
    char* name;
}

char* cargar_archivo(string directorios[]{
	# obtengo datos de tabla de directorio de root
	raw_data = root_table();

	# obtengo tabla de dir entries
	dir_entry* dir_table = parse_dir_entries(raw_data);

	for(dir in directorios){
		for(dir_entry in dir_table){
			blocks[];
			current_address = dir_entry->first_block_address;
			
			while(current_address != EOF){
				blocks.pushback(current_address);
				current_address = FAT_entry(current_address);
			}
			raw_data = read_blocks(blocks);
			
			if(i < directorios.length - 1){
                dir_table = parse_directory_entries(raw_data);
                break;
            } 
            else {
                file_data = malloc(dir_entry.file_size);
                memcpy(file_data, raw_data, dir_entry.file_size);
                return file_data;
            }
		}
	}
	return NULL;
}
```