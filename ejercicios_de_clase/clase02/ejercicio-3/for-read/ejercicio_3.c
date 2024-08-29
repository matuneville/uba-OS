#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define RANGO_MIN 2L
#define RANGO_MAX 1000000001L

// Constants 0 and 1 for READ and WRITE
enum { READ, WRITE };

int procesos;

int esPar(long numero) {
  return (numero & 1) == 0;
}

long contarPares(long desde, long hasta) {
  long cantidad = 0;
  for (long i = desde; i < hasta; ++i) {
    if (esPar(i)) {
      cantidad++;
    }
  }
  return cantidad;
}

void ejecutarHijo(int i, int pipes[][2], int pipe_resultado[2]) {
  // Leer del i-ésimo pipe el rango [desde, hasta) para realizar el cómputo
  long desde;
  long hasta;
  read(pipes[i][READ], &desde, sizeof(desde));
  read(pipes[i][READ], &hasta, sizeof(hasta));
  printf("Hijo %d: desde=%ld hasta=%ld\n", i, desde, hasta);

  // Contar pares y escribir el resultado en el pipe resultado
  long resultado = contarPares(desde, hasta);
  write(pipe_resultado[WRITE], &resultado, sizeof(resultado));
  exit(EXIT_SUCCESS);
}

int main(int argc, char const* argv[]) {
  // Parsear la cantidad de procesos
  if (argc != 2) {
    printf("Debe ejecutar con la cantidad de procesos N como parámetro.\n");
    printf("Ejemplo: %s N\n", argv[0]);
    return 1;
  }
  procesos = atoi(argv[1]);

  // Crear pipe resultado para comunicación hijo -> padre
  int pipe_resultado[2];
  pipe(pipe_resultado);

  // Crear pipes para comunicación padre -> hijo
  int pipes[procesos][2];
  for (int i = 0; i < procesos; i++) {
    pipe(pipes[i]);
  }

  // Crear hijos
  for (int i = 0; i < procesos; i++) {
    if (fork() == 0) ejecutarHijo(i, pipes, pipe_resultado);
  }

  // Calcular rangos para cada hijo
  // El intervalo es: [RANGO_MIN, RANGO_MAX) (es decir, cerrado-abierto)
  long cantidad = ((RANGO_MAX - RANGO_MIN) + (procesos - 1)) / procesos;
  long desde = RANGO_MIN;
  for (int i = 0; i < procesos; i++) {
    long hasta = desde + cantidad;
    if (hasta > RANGO_MAX) hasta = RANGO_MAX;

    write(pipes[i][WRITE], &desde, sizeof(desde));
    write(pipes[i][WRITE], &hasta, sizeof(hasta));

    desde = hasta;
  }

  // Leer los resultados de cada hijo del (único) pipe resultado.
  // Esta solución es válida para este problema particular donde no importa
  // identificar el resultado individual de cada hijo.
  // En otro contexto, si necesitamos saber el resultado específico de cada hijo,
  // será necesaria otra solución, por ejemplo utilizar un pipe resultado por
  // cada hijo. Otra opción es que los hijos envien un struct conteniendo el
  // resultado del cómputo junto a un identificar de cada hijo.
  long resultado = 0;
  for (int i = 0; i < procesos; i++) {
    long resultado_hijo;
    read(pipe_resultado[READ], &resultado_hijo, sizeof(resultado_hijo));
    resultado += resultado_hijo;
  }
  printf("Resultado total: %ld\n", resultado);

  return 0;
}
