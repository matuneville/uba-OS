#ifndef HMC_HPP
#define HMC_HPP

using namespace std;

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
   public:
        static constexpr int cantLetras = 26;

        HashMapConcurrente();

        void incrementar(std::string clave);

        void prohibir_seguir_incrementando_letra(int letra);

        void obtener_claves_en_letra(vector<string>& claves_vec, int letra);

        void permitir_incrementar_letra(int letra);

        std::vector<std::string> claves();

        unsigned int valor(std::string clave);

        float promedio();

        void computar_promedio_thread(unsigned int i);
        float promedio_concurrente(unsigned int cant_threads);

   private:
            ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];
        static unsigned int hashIndex(std::string clave);

        // variables incrementar
        mutex mutex_incrementar;
        mutex mutex_incrementar_letra[26];
        atomic<int> espero_incrementar;
        atomic<int> incrementando;
        atomic<int> incrementando_letra[26] = {};
        mutex incrementar_letra[26];

        // variables claves bloqueando por bucket
        atomic<int> getting_claves[26];
        atomic<int> esperando_inc_claves[26];
        mutex permiso_inc_claves[26]; // tiene que arrancar locked
        mutex mutex_claves_bucket[26];
        mutex barrera_inc_claves[26];

        // variables valor
        atomic<int> getting_valor[26] = {};
        atomic<int> esperando_inc_valor[26] = {};
        mutex permiso_inc_valor[26]; // tienen que arrancar locked
        mutex mutex_valor[26];
        mutex barrera_inc_valor[26];

        // variables promedio
        mutex mutex_promedio;
        atomic<int> promediando;
        mutex permiso_inc_promedio;

        // variables promedio concurrente
        mutex mutex_promedio_thread;
        unsigned int letra_actual = 0;
        unsigned int cant_letras_promediadias = 0;
        atomic<int> letras_promediadas[26] = {};
        mutex mutex_incrementar_promedio_concurrente[26];
        float sum_values = 0;
        unsigned int count_letras = 0;

    void start_sincro_inc_promedio();

    void end_sincro_inc_promedio();

    void start_sincro_inc_claves(unsigned int letra);

    void start_sincro_inc_valor(unsigned int letra);

    void end_sincro_inc_claves(unsigned int letra);

    void end_sincro_inc_valor(unsigned int letra);

    void computar_incremento(string &clave, bool aparecio, ListaAtomica<hashMapPair> *lista) const;

    void start_sincro_inc_promedio_concurrente(unsigned int letra);

    void end_sincro_inc_promedio_concurrente(unsigned int letra);

    void reset_global_variables();
};

#endif  /* HMC_HPP */

/*
tabla:
[ 0 ] -> ListaAtomica<hashMapPair> -> Nodo(hashMapPair) -> Nodo(hashMapPair) -> ...
[ 1 ] -> ListaAtomica<hashMapPair> -> Nodo(hashMapPair) -> Nodo(hashMapPair) -> ...
[ 2 ] -> nullptr  (no hay lista para esta entrada)
[ 3 ] -> ListaAtomica<hashMapPair> -> Nodo(hashMapPair) -> Nodo(hashMapPair) -> ...
...
[ 2 ] -> ListaAtomica<hashMapPair> -> Nodo(hashMapPair) -> Nodo(hashMapPair) -> ...


*/


