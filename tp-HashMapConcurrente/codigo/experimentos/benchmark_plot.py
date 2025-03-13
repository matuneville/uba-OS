import subprocess
import re
import matplotlib.pyplot as plt
import sys

def run_gen_list(cant_claves):
    try:
        result = subprocess.run(
            ['python', 'gen_list.py', str(cant_claves)],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        if result.returncode == 0:
            print(f"Resultado de gen_list.py con {cant_claves} claves generado correctamente.")
            return result.stdout
        else:
            print(f"Error al ejecutar gen_list.py: {result.stderr}")
            return None
    except Exception as e:
        print(f"Excepción al ejecutar gen_list.py: {e}")
        return None

def run_benchmarks(cant_threads):
    try:
        result = subprocess.run(
            ['./benchmarks', str(cant_threads)],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
        )
        if result.returncode == 0:
            print(f"Resultado de benchmarks:\n{result.stdout}")
            return result.stdout
        else:
            print(f"Error al ejecutar benchmarks: {result.stderr}")
            return None
    except Exception as e:
        print(f"Excepción al ejecutar benchmarks: {e}")
        return None

def parse_benchmark_output(output):
    try:
        promedio = re.search(r"Promedio: ([\d.]+)ms", output)
        promedio_concurrente = re.search(r"Promedio concurrente: ([\d.]+)ms", output)

        if promedio and promedio_concurrente:
            promedio = float(promedio.group(1))
            promedio_concurrente = float(promedio_concurrente.group(1))
            return promedio, promedio_concurrente
        else:
            print("No se encontraron los tiempos en el output de benchmarks.")
            return None, None
    except Exception as e:
        print(f"Error al parsear el output de benchmarks: {e}")
        return None, None

def plot_results(cant_claves_list, secuencial_times, concurrente_times):
    plt.figure(figsize=(10, 6))
    plt.plot(cant_claves_list, secuencial_times, label="Tiempo Secuencial", marker='o')
    plt.plot(cant_claves_list, concurrente_times, label="Tiempo Concurrente", marker='o')

    plt.xlabel("Cantidad de Claves")
    plt.ylabel("Tiempo (ms)")
    plt.title("Comparación de Tiempos: Promedio secuencial vs concurrente con 26 threads")
    plt.legend()
    plt.grid(True)
    plt.savefig("comparacion_tiempos.png")
    plt.show()

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Uso: python script.py <delta_claves> <cant_threads>")
        sys.exit(1)

    delta_claves = int(sys.argv[1])
    cant_threads = int(sys.argv[2])

    cant_claves = 10000
    cant_claves_list = []
    secuencial_times = []
    concurrente_times = []

    while cant_claves < 300000:
        print(f"\nEjecutando con {cant_claves} claves...\n")

        run_gen_list(cant_claves)

        benchmarks_result = run_benchmarks(cant_threads)
        if benchmarks_result:
            promedio, promedio_concurrente = parse_benchmark_output(benchmarks_result)
            if promedio is None or promedio_concurrente is None:
                print("Hubo un problema al obtener los tiempos, terminando la ejecución.")
                break

            print(f"Promedio secuencial: {promedio}ms")
            print(f"Promedio concurrente: {promedio_concurrente}ms")

            cant_claves_list.append(cant_claves)
            secuencial_times.append(promedio)
            concurrente_times.append(promedio_concurrente)

            cant_claves += delta_claves

    plot_results(cant_claves_list, secuencial_times, concurrente_times)
