#ifndef CHM_CPP
#define CHM_CPP

using namespace std;

#include <thread>
#include <iostream>
#include <atomic>
#include <algorithm>
#include "HashMapConcurrente.hpp"


// In HashMapConcurrente.cpp

// -------------------------- Métodos --------------------------

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }

    for (int i = 0; i < 26; i++) {
        permiso_inc_claves[i].lock();
    }

    for(int i=0; i < 26; i++){
        permiso_inc_valor[i].lock();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

/*
 * ############################################### incrementar ###############################################
 */

void HashMapConcurrente::incrementar(std::string clave) {
    clave[0] = tolower(clave[0]);

    unsigned int letra = HashMapConcurrente::hashIndex(clave);
    
    bool aparecio = false;
    ListaAtomica<hashMapPair> *lista = tabla[letra];

    start_sincro_inc_promedio();

    //  mutex para dar acceso exclusivo a cada thread por cada letra.
    mutex_incrementar_letra[letra].lock();

    start_sincro_inc_claves(letra);
    start_sincro_inc_valor(letra);
    start_sincro_inc_promedio_concurrente(letra);

    computar_incremento(clave, aparecio, lista);

    end_sincro_inc_promedio_concurrente(letra);
    end_sincro_inc_valor(letra);
    end_sincro_inc_claves(letra);

    mutex_incrementar_letra[letra].unlock();

    end_sincro_inc_promedio();
}

void HashMapConcurrente::end_sincro_inc_promedio_concurrente(unsigned int letra) {
    mutex_incrementar_promedio_concurrente[letra].lock();
    incrementando_letra[letra].fetch_sub(1);
    incrementar_letra[letra].unlock(); // concurrencia con computar_promedio_thread
    mutex_incrementar_promedio_concurrente[letra].unlock();
}

void HashMapConcurrente::start_sincro_inc_promedio_concurrente(unsigned int letra) {
    mutex_incrementar_promedio_concurrente[letra].lock(); // dos lineas que funcionen de manera atomica
    incrementando_letra[letra].fetch_add(1);
    incrementar_letra[letra].lock(); // concurrencia con computar_promedio_thread
    mutex_incrementar_promedio_concurrente[letra].unlock();
}

void HashMapConcurrente::computar_incremento(string &clave, bool aparecio,
                                             ListaAtomica<hashMapPair> *lista) const {// computamos el ""incremento""
    for(auto &nodo_hashpair : *lista) {
        if (nodo_hashpair.first == clave){
            // caso 1: ya estaba en la lista
            aparecio = true;
            nodo_hashpair.second++;
            break;
        }
    }
    // caso 2: no estaba en la lista
    if(not aparecio){
        hashMapPair par_nuevo = make_pair(clave, 1);
        lista->insertar(par_nuevo);
    }
}

void HashMapConcurrente::end_sincro_inc_valor(unsigned int letra) { barrera_inc_valor[letra].unlock(); }

void HashMapConcurrente::end_sincro_inc_claves(unsigned int letra) { barrera_inc_claves[letra].unlock(); }

void HashMapConcurrente::start_sincro_inc_valor(unsigned int letra) {
    if (getting_valor[letra].load()){
        esperando_inc_valor[letra].fetch_add(1);
        // puedo empezar a incrementar solo si no hay nadie en this.valor() con MI letra
        permiso_inc_valor[letra].lock();
    }
    barrera_inc_valor[letra].lock();
}

void HashMapConcurrente::start_sincro_inc_claves(unsigned int letra) {// solucion concurrencia con this.CLAVES()
    if (getting_claves[letra].load()) {
        esperando_inc_claves[letra].fetch_add(1);
        // puedo empezar a incrementar solo si no hay nadie en this.claves()
        permiso_inc_claves[letra].lock();
    }
    barrera_inc_claves[letra].lock();
}

void HashMapConcurrente::start_sincro_inc_promedio() {
    /*  solucion concurrencia con this.PROMEDIO()
     * podemos incrementar sii no hay nadie promediando
     * podemos promediar sii no hay nadie incrementando
     */
    mutex_incrementar.lock();
    if (espero_incrementar.load() == 0){
        permiso_inc_promedio.lock();
    }
    espero_incrementar.fetch_add(1);
    mutex_incrementar.unlock();
}

void HashMapConcurrente::end_sincro_inc_promedio() {
    mutex_incrementar.lock();
    espero_incrementar.fetch_sub(1);
    if (espero_incrementar.load() == 0){
        permiso_inc_promedio.unlock();
    }
    mutex_incrementar.unlock();
}

/*
 * ############################################### claves ###############################################
 * */

void HashMapConcurrente::prohibir_seguir_incrementando_letra(int letra) {
    getting_claves[letra].fetch_add(1);
    barrera_inc_claves[letra].lock();
}

void HashMapConcurrente::obtener_claves_en_letra(vector<string>& claves_vec, int letra) {
//    incrementar_letra[letra].lock();
    ListaAtomica<hashMapPair> *lista = this->tabla[letra];
    for(auto &nodo_hashpair : *lista){
        claves_vec.push_back(nodo_hashpair.first);
    }
//    incrementar_letra[letra].unlock();
}

void HashMapConcurrente::permitir_incrementar_letra(int letra) {
    mutex_claves_bucket[letra].lock();
    if(esperando_inc_claves[letra].load()){
        permiso_inc_claves[letra].unlock();
    }
    getting_claves[letra].fetch_sub(1); // aviso que ya no estoy obteniendo claves
    mutex_claves_bucket[letra].unlock();

    barrera_inc_claves[letra].unlock();
}

std::vector<std::string> HashMapConcurrente::claves() {
    vector<string> claves_vec = {};

    for(int letra = 0; letra < 26; letra++){
        prohibir_seguir_incrementando_letra(letra);
        obtener_claves_en_letra(claves_vec, letra);
        permitir_incrementar_letra(letra);
    }

    return claves_vec;
}

/*
 * ############################################### valor ###############################################
 * */

/**
 * Esta solucion permite obtener el valor de palabras con una misma letra inicial a la vez.
 */
unsigned int HashMapConcurrente::valor(std::string clave) {
    //  obtenemos hash
    clave[0] = tolower(clave[0]);
    unsigned int letra = HashMapConcurrente::hashIndex(clave);

    //  Incremento para indicar que estoy obtiendo el valor
    getting_valor[letra].fetch_add(1);

    // puedo retornar el valor SOLO si no estan incrementando mi letra, asi que espero a que termine
    barrera_inc_valor[letra].lock();

    // si estoy aca, puedo obtener valor
    ListaAtomica<hashMapPair> *lista = this->tabla[letra];
    unsigned int resultado = 0; // devuelve 0 si no está presente en la tabla
    for(auto &nodo_hashpair : *lista) {
        if (nodo_hashpair.first == clave){
            resultado = nodo_hashpair.second;
            break;
        }
    }

    // dejo salir al que estaba esperando en mi letra (como maximo hay 1 por el mutex)
    mutex_valor[letra].lock();
    if (esperando_inc_valor[letra].load()){
        permiso_inc_valor[letra].unlock();
        esperando_inc_valor[letra].fetch_sub(1);
    }
    getting_valor[letra].fetch_sub(1); // aviso que ya no estoy obteniendo valor
    mutex_valor[letra].unlock();

    barrera_inc_valor[letra].unlock();

    return resultado;
}

/*
 * ############################################### promedio ###############################################
 * */

float HashMapConcurrente::promedio() {
    // lo resolvemos como el problema del puente de 1 carril
    mutex_promedio.lock();
    if (promediando.load() == 0){
        // soy el primero en promediar, espero a que no haya ninguno incrementando
        permiso_inc_promedio.lock();
    }
    promediando.fetch_add(1);
    mutex_promedio.unlock();

    // hacemos lo importante
    float sum = 0.0;
    unsigned int count = 0;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
    }

    mutex_promedio.lock();
    promediando.fetch_sub(1);
    if (promediando.load() == 0){
        permiso_inc_promedio.unlock();
    }
    mutex_promedio.unlock();

    if (count > 0) {
        return sum / count;
    }
    return 0;
}

// ------------------ Promedio concurrente ------------------

void HashMapConcurrente::computar_promedio_thread(unsigned int i) {
    int mi_letra;
    /*
     * idea:
     * un array de largo 26 en el que marcamos cuáles letras ya promediamos
     * la letra avanza de 1 a 26, si una letra esta siendo incrementada, se avanza igual,
     * y cuando termine en 26, vuelve a 1 para seguir con las restantes
     * */

    // [1, 1, 1, 1, ...]

    while(cant_letras_promediadias < 26){
        // mutex por cada hilo de promedio
        mutex_promedio_thread.lock(); // cierro mutex promedio por hilo
        mi_letra = letra_actual++ % 26;
        if(letras_promediadas[mi_letra]){
            // si ya fue promediada, paso a la sgte letra
            mutex_promedio_thread.unlock();
            continue;
        }
        mutex_incrementar_promedio_concurrente[mi_letra].lock();
        if(incrementando_letra[mi_letra].load()){
            // si la estan incrementando, paso a la sgte letra
            mutex_promedio_thread.unlock();
            mutex_incrementar_promedio_concurrente[mi_letra].unlock();
            continue;
        } else {
            // no la estan incrementando, puedo acceder a la lista
            incrementar_letra[mi_letra].lock();
            mutex_incrementar_promedio_concurrente[mi_letra].unlock();

            letras_promediadas[mi_letra].fetch_add(1);
            cant_letras_promediadias++;
            mutex_promedio_thread.unlock(); // abro mutex promedio por hilo

            //cout << "Soy thread: " << i << ", promediando letra: " << mi_letra <<endl;
            float sum = 0.0;
            unsigned int count = 0;

            for (const auto& p : *tabla[mi_letra]) {
                sum += (float) p.second;
                count++;
            }
            incrementar_letra[mi_letra].unlock();

            //if ( count != 0 ) {
            //promedio_por_letra[mi_letra] = sum / (float)count;
            sum_values += sum;
            count_letras += count;
            //}

            //cout << "Hilo " << i << " termino con letra " << mi_letra << endl;

        }
    }
}

float HashMapConcurrente::promedio_concurrente(unsigned int cant_threads){
    unsigned int max_cant_threads = 26;
    vector<float> promedio_por_letra(26,0);

    vector<thread> threads;
    for (int i = 0; i < min(max_cant_threads, cant_threads); ++i) {
        threads.emplace_back([this, i]() {
            this->computar_promedio_thread(i);
        });
    }

    for (auto &t:threads) {
        t.join();
    }

   // cout << "debug!" << endl;

    float res = sum_values / (float)count_letras;

    reset_global_variables();

    return res;
}

void HashMapConcurrente::reset_global_variables() {
    for(int i = 0; i < 26; i++){
        letras_promediadas[i].store(0);
    }
    sum_values = 0;
    count_letras = 0;
    cant_letras_promediadias = 0;
}

#endif