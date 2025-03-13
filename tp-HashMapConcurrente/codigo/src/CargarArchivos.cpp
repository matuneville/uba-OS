#ifndef CHM_CPP
#define CHM_CPP

using namespace std;

#include <vector>
#include <iostream>
#include <fstream>
#include <thread>

#include "CargarArchivos.hpp"

//  VARS GLOBALES
atomic<int> indexArchivoAProcesar(0);//   en c/momento, da el nro del ultimo archivo procesado


int cargarArchivo(HashMapConcurrente &hashMap, std::string filePath) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;


    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }

    //  leemos palabra por palabra
    while (file >> palabraActual) {
        //  ponemos primera letra en lowerCase
        palabraActual[0] = tolower(palabraActual[0]);
        //  la agregamos al hashmap
        hashMap.incrementar(palabraActual);

        cant++;
    }

    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error en EOF del archivo con filePath =  " << filePath << std::endl;
        file.close();
        return -1;
    }
    file.close();

    return cant;
}



void threadCargarArchivo(HashMapConcurrente &hashMap, const vector<string> &filePaths, const int &cantFiles) {
    int i = 0;

    //  notar que ${VAR}++ incrementa post-uso de la variable, luego esto funciona OK
    //  si ya se procesaron todos los archivos, salimos
    while ( (i = indexArchivoAProcesar++) < cantFiles ) {
        //  procesamos el archivo
        cargarArchivo(hashMap, filePaths[i]);
    }
}

/*
    NOTAS:
        - cada thread se encarga de un archivo a la vez
        - se debe maximizar concurrencia:
            cuando un thread termina de procesar un archivo, pasa al siguiente para procesar. Esto se repite hasta
            que ya no hay más archivos y luego los hilos terminan.
 */
void cargarMultiplesArchivos(HashMapConcurrente &hashMap, unsigned int cantThreads, vector<string> filePaths) {
    vector<thread> vecThreads;
    int cantFiles = filePaths.size();

    for (int i = 0; i < (int)cantThreads; ++i) {
        vecThreads.emplace_back( [&hashMap, &filePaths, &cantFiles]{ threadCargarArchivo(hashMap, filePaths, cantFiles); } );
    }

    //  esperamos a cada thread con join();
    for (auto &t : vecThreads) {
        t.join();
    }

    //  reiniciamos index.
    indexArchivoAProcesar.store(0);

}    

#endif
