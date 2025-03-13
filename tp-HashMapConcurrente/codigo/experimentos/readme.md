# Experimentos

## Benchmarks.cpp
### Para compilarlo:
```
  g++ benchmarks.cpp ../src/HashMapConcurrente.cpp -o benchmarks
```
### Uso:
```
  ./benchmarks <cant_threads>
```

## gen_list.py
Genera en el directorio donde se corre el archivo `alphabet_sequence.txt`, cuyo contenido es una palabra distinta por linea, listando `A,B,C...,Z,AA,AB...` hasta llegar al numero pasado por parametro

### Uso:
```
  python gen_list.py <cantidad_de_claves_a_generar>
```

## benchmark_plot.py
Genera en el directorio donde se lo corre un grafico que compara la performance de promedio secuencial vs concurrente. Toma como input cant_threads y el delta en el que se va a incrementar cant_claves, que arranca en 10.000

**DISCLAIMER:** Es necesario que se encuentren en el directorio el ejecutable `benchmarks` y el script `gen_list.py`

### Uso:
```
  python benchmark_plot.py <delta_claves> <cant_threads>
```

