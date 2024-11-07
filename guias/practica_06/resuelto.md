# Práctica 7: Protección y seguridad

### Ejercicio 1

```c
void saludo(void) {
    char nombre[80];
    printf("Ingrese su nombre: ");
    gets(nombre);
    printf("Hola %s!\n", nombre);
}
```

`gets():` get a string from standard input.  
Signature: `char *gets(char *s);`

DESCRIPTION
       Never use this function.

       gets()  reads  a  line from stdin into the buffer pointed to by s until
       either a terminating newline or EOF, which it replaces with a null byte
       ('\0').  No check for buffer overrun is performed (see BUGS below).
       
a) ¿Dónde se introduce el problema de seguridad?   
El usuario puede ingresar un nombre muy largo (mayor a 80 chars) y escribir en memoria fuera del buffer.

b) ¿Qué datos del programa (registros, variables, direcciones de memoria) pueden ser controladas por el usuario?  
Con esta implementación, el usuario puede provocar un buffer _overrun_ u _overflow_ y pisar los valores de variables pusheadas en pila, además de valores más importantes como la eventual dirección de retorno de `saludo()`.

c) ¿Es posible sobreescribir la dirección de retorno a la que vuelve la llamada de alguna de las funciones printf o gets?  
No, ya que cuando se llame a aquellas funciones, tendrán su determinado stack frame momentáneamente y luego volverán al stack de `saludo()`.

d) ¿Se soluciona el problema de seguridad si se elimina el segundo printf?  
Jsjasj no, el problema sigue siendo `gets()`.


### Ejercicio 2

```c
struct credential {
    char name[32];
    char pass[32];
};

bool login(void) {
    char realpass[32];
    struct credential user;

    // Pregunta el usuario
    printf("User: ");
    fgets(user.name, sizeof(user), stdin);

    // Obtiene la contraseña real desde la base de datos y lo guarda en realpass
    db_get_pass_for_user(user.name, realpass, sizeof(realpass));

    // Pregunta la contraseña
    printf("Pass: ");
    fgets(user.pass, sizeof(user), stdin);

    return strncmp(user.pass, realpass, sizeof(realpass) - 1) == 0;
    // True si user.pass == realpass
}
```

a) Diagrama del estado de pila:

```
0x0000             -
 _________________
|_________________|
|____user.name____| [0,...,7]
|____user.name____| [8,...,15]    todo esto es del struct, campo name
|____user.name____| [16,...,23]
|____user.name____| [24,...,31]
|____user.pass____| [0,...,7]
|____user.pass____| [8,...,15]    todo esto es del struct, campo pass
|____user.pass____| [16,...,23]
|____user.pass____| [24,...,31]
|____realpass_____| [0,...,7]
|____realpass_____| [8,...,15]    todo esto es real pass
|____realpass_____| [16,...,23]
|____realpass_____| [24,...,31]
|_______EBP_______|
|_EIP (ret addr)__|

0xFFFF             +
```

El problema es que el `fgets` está tomando como size el tamaño entero del struct (64B) en vez del tamaño de cada campo. Cuando el usuario ingrese nombre, puede ingresar uno mayor a largo 32B, lo cual, si bien no es lo ideal, no genera problemas ya que el tope sería el final del campo para la contraseña del mismo. El problema viene cuando trae la contraseña al buffer de `realpass`, y, luego, toma el input de usuario nuevamente para escribir en `user.pass`, permitiendo que escriba 64B en vez de 32B, llegando a pisar hasta el final del campo del buffer de `realpass`.  

b) En `user.name` puede escribir lo que sea (hasta largo 64B), y luego en `user.pass` puede escribir algo de largo 64B y que ambas mitades sean iguales, así pisa el campo `realpass` y el `strcmp` devuelve que son iguales. Un input para este último tiene que ser de 64 chars de largo, siendo los primeros y últimos 32B iguales. Ejemplo, el string `1234567812345678123456781234567812345678123456781234567812345678`, donde luego el `strcmp` comparará `12345678123456781234567812345678` con `12345678123456781234567812345678`.

### Ejercicio 4

El problema es que un `float`puede tener valor `NaN`.

### Ejercicio 6

```c
#define BUF_SIZE 1024
void wrapper_ls(const char * dir){
    char cmd[BUF_SIZE];
    snprintf(cmd, BUF_SIZE-1, "ls %s", dir);
    system(cmd);
}
```

a) Si `dir = ". ; cat /etc/passwd"`, entonces el programa ejecutará `ls . ; cat /etc/passwd`, es decir, hará el ls sobre el directorio actual y luego printeará en std output el contenido del archivo passwd.  

b) Al usar `ls "x"`, `x` será interpretado como un único valor del parámetro. Con esto, el nuevo programa busca que en `"ls \" %s\""`, solo se pueda poner un parámetro ya que pone unas comillas a la fuerza. Pero aún así, podemos poner como valor en `dir` algo que cierre las primeras comillas, ejecute algo malicioso, y luego abra otras comillas para las que se cierran en el string original. Si `dir = ".\"; cat /etc/passwd ; echo \"hello"`, entonces `system` ejecutará en shell el comando `ls "."; cat /etc/passwd ; echo "hello"`.  

c) Igual que antes, pero podemos reemplazar el `;` por `&&`.  

d) Hay que sanitizar mejor el input `dir` en caso de seguir usando `system`. pero lo mejor es evitar su uso. Una implementación más segura sería utilizando forks y `exec`:

```c
void secure_wrapper_ls(const char * dir) {
    pid_t pid = fork();
    if (pid == 0) {
        // Proceso hijo
        execlp("ls", "ls", dir, (char *)NULL);
        _exit(EXIT_FAILURE); // si el exec falla
    }
    else if (pid > 0) {
        // proceso padre
        wait(NULL); // espero a que termine hijo
    }
    else {
        perror("fork");
    }
}
```

## Breve apunte para usar

1. **Bit `setuid`**:
    - El bit `setuid` (Set User ID) se establece en el **propietario** de un archivo ejecutable.
    - Cuando un programa con `setuid` se ejecuta, **se ejecuta con los permisos del usuario propietario del archivo, sin importar quién lo ejecute**.
    - Si un archivo tiene `setuid` y es propiedad de `root`, cualquier usuario que ejecute el programa tendrá permisos de `root` **solo mientras se ejecuta ese programa específico**.
    - Esto permite que el programa acceda a recursos normalmente restringidos (como `/etc/shadow`), pero no otorga permisos `root` al usuario para realizar otras acciones fuera del contexto de ese programa.

2. **Permisos de root**:
    - Dar "permisos de root" implica que el usuario o proceso tiene acceso total al sistema, con la capacidad de leer, escribir y ejecutar archivos, modificar configuraciones del sistema, y realizar cualquier operación sin restricciones.
    - Un usuario con permisos `root` puede hacer mucho más que simplemente ejecutar un programa con `setuid`.

**Ejemplo práctico**:

- Si un programa, como `passwd`, tiene el bit `setuid` configurado y pertenece a `root`, cualquier usuario puede ejecutar `passwd` para cambiar su propia contraseña, incluso si este programa accede a `/etc/shadow`.
- Sin embargo, los usuarios **no ganan acceso directo a `/etc/shadow`** ni otros privilegios de root fuera de ese programa.

### Ejercicio 12

```c
void imprimir_habilitado(const char *nombre_usuario, const char* clave, const char * imprimir, int tam_imprimir) {
    char *cmd = malloc(tam_imprimir + 5 * sizeof(char));
    if (cmd == NULL)
        exit(1);
    
    if (usuario_habilitado("/etc/shadow", nombre_usuario, clave)) {
        snprintf(cmd, tam_imprimir + 4, "echo %s", imprimir);
        // int snprintf(char str[restrict .size], size_t size, const char *restrict format, ...);
        system(cmd);
    } else {
        printf("El usuario o clave indicados son incorrectos.");
        assert(-1);
    }
}
```

1. Podemos dar la siguiente configuración de permisos al binario del programa: `-rws --x --x`
	- El **binario del programa** debe ser propiedad de `root`.
	- Se debe activar el **bit `setuid`** usando `chmod u+s`.
	Con esto, logramos lo siguiente:
	- El `s` en la en los permisos de propietario (user), que permite que el programa se ejecute con los permisos del propietario del archivo (en este caso, debe ser `root`), lo que le otorga acceso al archivo `/etc/shadow`.
	- El resto de los permisos (`r-x` para group y others), indican que el grupo y otros usuarios tienen permisos de lectura y ejecución, pero no de escritura.  

    Con esta configuración, el programa podrá acceder al archivo `/etc/shadow` para verificar las credenciales de los usuarios, mientras que el resto del sistema no se ve afectado por permisos elevados.

2. Indique dos problemas de seguridad que podrían surgir (hint: tenga en cuenta el ítem anterior)
    1. El usuario puede llamar a la función con parámetro `imprimir = "hello"; sudo bash` y luego hacer lo que quiera en la computadora con un bash de privilegios de root.
    
    2. Cuando el `snprintf` utiliza echo para ejecutar luego con `system`, no está utilizando la ruta absoluta al binario donde se encuentra echo (`/bin/echo`). Una forma de explotar esto sería definir un nuevo path con un binario llamado igual al de la función utilizada (echo en este caso) que ejecute lo que nosotros queramos. Entonces a la hora de ejecutar el programa, el primer binario en encontrar echo será el del programa nuestro, en vez del del Unix.
  
3. Ya esta en el 2.

4. Ambos casos especificos mencionados en el 2 rompen los tres pilares (Disponibilidad, Integridad y Confidencialidad), ya que con una terminal de máximo privilegio podemos hacer lo que queramos xd.

5. Para el caso 2. del path, hay que escribir en el `snprintf` el path entero de la función echo: `/bin/echo`. Para el caso 1., deberíamos sanitizar el input del usuario.

### Ejercicio 13

```c
/**
 * Dado un usuario y una clave, indica si la misma es válida
 */
extern bool clave_es_valida(char* usuario, char* clave);

bool validar_clave_para_usuario(char *usuario) {
    // fmt = " %......."
    //  fmt = FORMAT
    char fmt[8];
    fmt[0] = ' %';

    printf("Ingrese un largo para la clave: ");
    // fmt = " %NNNN\0"
    scanf(" %4s", fmt + 1); // 123cien
    int l = strlen(fmt + 1);

    // max_size <- atoi(NNNN)
    unsigned char max_size = atoi(fmt + 1); // unsigned char tiene maximo valor 255!!!
    char clave[max_size + 1];
    // fmt = " %NNNNs\0"
    fmt[l + 1] = 's';
    fmt[l + 2] = '\0';

    scanf(fmt, clave);
    return clave_es_valida(usuario, clave);
}

int main(int argc, char **argv) {
    setuid(0);
    bool es_valida = validar_clave_para_usuario(argv[1]);
    if (es_valida) {
        system("/bin/bash");
    } else {
        exit(EXIT_FAILURE);
    }
}
```




1. Explique brevemente qué hace el código, qué función cumple cada variable, qué condiciones deben existir para que se ejecute completamente, y cuáles serían los vectores de ataque. Justifique e indique cualquier detalle relevante.
   - basicamente lo que hace el código es recibir por input la cant de caracteres de la clave.
        Guarda en la variable "l" la cant de digitos que ingreso el usuario.  
        Luego, guarda la cant de caracteres de la clave en la posicion de fmt+1 tq en fmt = ' %NNNN' y a atoi le manda atom('NNNN'),  
        tq parsea correctamente el nro (si es un nro) a la variable MAX_SIZE  
        luego, crea un array clave de tamaño MAX_SIZE + 1 (pues indexamos en 0)  
        finalmente, en la pos fmt[l+1] = "s" y fmt[l+1] = "\0"  
        tq fmt queda como = " %NNNNs\0" lo cual es el formato que utiliza scanf para indicar que debe leer caracteres de tipo string y le clava al final un '\0'  
        luego, lee a lo sumo NNNN caracteres de tipo string... de esta forma se fuerza a qué se ingresen a lo sumo NNNN caracteres....

   
   
2. Indique los nombres de las vulnerabilidades involucradas.
   - Integer overflow al tipar como `unsigned char` a `max_size`, dándole como máximo valor 255.
   - Buffer overflow, consecuencia de la vulnerabilidad previa.
   
3. Indique con qué argumentos y entradas de usuario deberá ser ejecutado el programa para vulnerarlo. En caso de requerir contar con un payload complejo, indicar la composición del mismo.
    - En el primer `scanf`, ingresamos como largo de clave 256
    - En consecuencia al integer overflow, el buffer de clave será de largo 1
    - Luego, debemos llenar esa clave con lo que sea, para luego escribir ciertos valores que lleguen hasta la dirección de retorno en el stack.
    - En total, hay que escribir 8B (clave largo 1 + padding) + 8B (int l + padding) + 8B (fmt) + 8B (rbp) + la dirección que queramos! O sea, 32B de basura y luego en otros 8B la dirección que queramos pisar para el retorno. Esto entrará en el `scanf` para la clave ya que dijimos que el largo era 256B.

```
0x0000             -
 _________________
|______CLAVE______| 8B (tam 1B)
|________l________| 8B (tam int)
|_______FMT_______| 8B (array 8B)
|_______EBP_______| 8B (tam int)
|_EIP (ret addr)__| 8B (tam int)

0xFFFF             +
```
   
4. En caso de requerir conocer algún dato o valor del programa para construir el payload o para realizar el ataque, indique de dónde proviene el mismo, y cómo lo obtendría.
   - Pongo a correr mi software malicioso al que quiero saltar y uso GDB para ver la dirección de su primer línea.

5. Indique y justifique las posibles formas de impacto del ataque en base a los tres principios básicos de la seguridad de la información.

6. Proponga una forma de mitigar la(s) vulnerabilidad(es) modificando el código.
    - No utilizar `unsigned char`, con un `unsigned uint16_t` funciona bien porque 2^16 > 9999 

7. Indique y justifique si existen formas de mitigar la(s) vulnerabilidad(es) a nivel del sistema operativo.
    - Utilizando el método de "Stack Canaries" visto en clase
    - Se puede usar Address Space Layout Randomization (ASLR) para forzar a que las direcciones sean random tal que no se pueda ingresar un valor de dirección hardcodeado de adónde saltar, pues no se sabe de antemano cuál es la DIR a la que queremos ir.
    


