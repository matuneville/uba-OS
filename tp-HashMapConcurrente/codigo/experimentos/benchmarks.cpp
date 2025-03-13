#include "../src/HashMapConcurrente.hpp"
#include "../src/CargarArchivos.hpp"
#include "iostream"
#include "vector"
#include <chrono>
#include <fstream>
using namespace std;


void promedio_vs_promedio_concurrente(unsigned int cant_threads) {
    HashMapConcurrente hmap;

    auto inicio_carga = chrono::steady_clock::now();
    ifstream archivo("alphabet_sequence.txt");
    string linea;
    int i = 0;
    if (archivo.is_open()) {
        while (getline(archivo, linea)) {
            //lineas.push_back(linea);
            hmap.incrementar(linea);
            i++;
            if (i % 100000 == 0) {
                cout << "✅ cargados" << i  << "archivos" << endl;
            }

        }
        archivo.close();
    } else {
        cerr << "No se pudo abrir el archivo." <<
             std::endl;
    }
    auto fin_carga = chrono::steady_clock::now();

    chrono::duration<double, milli> duracion_carga = fin_carga - inicio_carga;

    cout << "Duracion de la carga: " << duracion_carga.count() << endl;

    auto inicio2 = chrono::steady_clock::now();
    float prom2 = hmap.promedio_concurrente(cant_threads);
    auto fin2 = chrono::steady_clock::now();

    chrono::duration<double, milli> duracion2 = fin2 - inicio2;

    auto inicio1 = chrono::steady_clock::now();
    float prom1 = hmap.promedio();
    auto fin1 = chrono::steady_clock::now();

    chrono::duration<double, milli> duracion1 = fin1 - inicio1;

    cout << "Promedio: " << duracion1.count() << "ms\nPromedio concurrente: " << duracion2.count() << "ms con " << cant_threads << " threads" << endl;
}

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Uso: %s <cantidad_threads>\n", argv[0]);
        return 1;
    }

    int cant_threads = atoi(argv[1]);

    if (cant_threads <= 0) {
        printf("La cantidad de threads debe ser un número entero positivo.\n");
        return 1;
    }

    if (cant_threads > 26) {
        printf("La cantidad de threads no debe ser mayor a 26.\n");
        return 1;
    }

    promedio_vs_promedio_concurrente(cant_threads);

    return 0;
}
