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
    fgets(user.name, sizeof(user.name), stdin);

    // Obtiene la contraseña real desde la base de datos y lo guarda en realpass
    db_get_pass_for_user(user.name, realpass, sizeof(realpass));

    // Pregunta la contraseña
    printf("Pass: ");
    fgets(user.pass, sizeof(user.pass), stdin);

    return strncmp(user.pass, realpass, sizeof(realpass) - 1) == 0;
    // True si user.pass == realpass
}
```