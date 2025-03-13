#include <vector>
#include "lib/littletest.hpp"
#include <thread>

#include "../src/ListaAtomica.hpp"
#include "../src/HashMapConcurrente.hpp"
#include "../src/CargarArchivos.hpp"

// Tests Ejercicio 1

LT_BEGIN_SUITE(TestsEjercicio1)

ListaAtomica<int> l;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio1)

LT_BEGIN_TEST(TestsEjercicio1, ListaComienzaVacia)
    LT_CHECK_EQ(l.longitud(), 0);
LT_END_TEST(ListaComienzaVacia)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaElemento)
    l.insertar(42);
    LT_CHECK_EQ(l.longitud(), 1);
LT_END_TEST(InsertarAgregaElemento)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaElementoCorrecto)
    l.insertar(42);
    LT_CHECK_EQ(l[0], 42);
LT_END_TEST(InsertarAgregaElementoCorrecto)

LT_BEGIN_TEST(TestsEjercicio1, InsertarAgregaEnOrden)
    l.insertar(4);
    l.insertar(3);
    l.insertar(2);
    l.insertar(1);
    LT_CHECK_EQ(l.longitud(), 4);
    LT_CHECK_EQ(l[0], 1);
    LT_CHECK_EQ(l[1], 2);
    LT_CHECK_EQ(l[2], 3);
    LT_CHECK_EQ(l[3], 4);
LT_END_TEST(InsertarAgregaEnOrden)

// Tests concurrencia

LT_BEGIN_TEST(TestsEjercicio1, InsertarConcurrenteAgregaLosElementosCorrectamente)
    auto insertar_rango = [&](int inicio, int fin) {
        for (int i = inicio; i <= fin; ++i) {
            l.insertar(i);
        }
    };

    std::thread t1(insertar_rango, 1, 50);
    std::thread t2(insertar_rango, 51, 100);
    std::thread t3(insertar_rango, 101, 150);
    std::thread t4(insertar_rango, 151, 200);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    LT_CHECK_EQ(l.longitud(), 200);

    std::vector<bool> presentes(201, false);
    for (int i = 0; i < l.longitud(); ++i) {
        int valor = l[i];
        LT_CHECK(valor >= 1 && valor <= 200);
        presentes[valor] = true;
    }

    for (int i = 1; i <= 200; ++i) {
        LT_CHECK(presentes[i]);
    }

LT_END_TEST(InsertarConcurrenteAgregaLosElementosCorrectamente)

//Tests Ejercicio 2

LT_BEGIN_SUITE(TestsEjercicio2)

HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio2)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoEnHashMapVacio)
    LT_CHECK_EQ(hM.valor("Heladera"), 0);
LT_END_TEST(ValorEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoEnHashMapVacio)
    std::vector<std::string> actual = hM.claves();
    std::vector<std::string> expected = {};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoEnHashMapVacio)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasUnaInsercion)
    hM.incrementar("Heladera");
    LT_CHECK_EQ(hM.valor("Heladera"), 1);
LT_END_TEST(ValorEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasUnaInsercion)
    hM.incrementar("Heladera");
    std::vector<std::string> actual = hM.claves();
    //std::vector<std::string> expected = {"Heladera"};
    std::vector<std::string> expected = {"heladera"};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasUnaInsercion)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasDosInsercionesMismaPalabra)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    LT_CHECK_EQ(hM.valor("Heladera"), 2);
LT_END_TEST(ValorEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasDosInsercionesMismaPalabra)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    std::vector<std::string> actual = hM.claves();
    //std::vector<std::string> expected = {"Heladera"};
    std::vector<std::string> expected = {"heladera"};
    LT_CHECK_COLLECTIONS_EQ(actual.begin(), actual.end(), expected.begin());
LT_END_TEST(ClavesEsCorrectoTrasDosInsercionesMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasVariasInsercionesMismoBucket)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Lavarropa");
    LT_CHECK_EQ(hM.valor("heladera"), 2);
    LT_CHECK_EQ(hM.valor("lavarropa"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasVariasInsercionesMismoBucket)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Lavarropa");
    std::vector<std::string> actual = hM.claves();
    LT_CHECK_EQ(actual.size(), 2);
    LT_CHECK(std::find(actual.begin(), actual.end(), "heladera") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "lavarropa") != actual.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesMismoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasVariasInsercionesDistintoBucket)
    hM.incrementar("Heladera");
    hM.incrementar("Microondas");
    hM.incrementar("Heladera");
    LT_CHECK_EQ(hM.valor("heladera"), 2);
    LT_CHECK_EQ(hM.valor("microondas"), 1);
LT_END_TEST(ValorEsCorrectoTrasVariasInsercionesDistintoBucket)

LT_BEGIN_TEST(TestsEjercicio2, ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)
    hM.incrementar("Heladera");
    hM.incrementar("Microondas");
    hM.incrementar("Heladera");
    std::vector<std::string> actual = hM.claves();
    LT_CHECK_EQ(actual.size(), 2);
    LT_CHECK(std::find(actual.begin(), actual.end(), "heladera") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "microondas") != actual.end());
LT_END_TEST(ClavesEsCorrectoTrasVariasInsercionesDistintoBucket)

// Tests concurrencia
LT_BEGIN_TEST(TestsEjercicio2, ValorEsCorrectoTrasIncrementarConcurrentementeMismaPalabra)
    auto incrementar_palabra_n_veces = [&](std::string palabra, int n) {
        for (int i = 0; i < n; ++i) {
            hM.incrementar(palabra);
        }
    };

    std::thread t1(incrementar_palabra_n_veces, "heladera", 5);
    std::thread t2(incrementar_palabra_n_veces, "heladera", 5);
    std::thread t3(incrementar_palabra_n_veces, "heladera", 5);
    std::thread t4(incrementar_palabra_n_veces, "heladera", 5);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    LT_CHECK_EQ(hM.valor("heladera"), 20);

LT_END_TEST(ValorEsCorrectoTrasIncrementarConcurrentementeMismaPalabra)

LT_BEGIN_TEST(TestsEjercicio2, ValoresSonCorrectosTrasIncrementarConcurrentementeVariasPalabras)
    auto incrementar_palabra_n_veces = [&](std::string palabra, int n) {
        for (int i = 0; i < n; ++i) {
            hM.incrementar(palabra);
        }
    };

    std::thread t1(incrementar_palabra_n_veces, "heladera", 5);
    std::thread t2(incrementar_palabra_n_veces, "heladera", 5);
    std::thread t3(incrementar_palabra_n_veces, "lavarropas", 5);
    std::thread t4(incrementar_palabra_n_veces, "mousepad", 5);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    LT_CHECK_EQ(hM.valor("heladera"), 10);
    LT_CHECK_EQ(hM.valor("lavarropas"), 5);
    LT_CHECK_EQ(hM.valor("mousepad"), 5);

LT_END_TEST(ValoresSonCorrectosTrasIncrementarConcurrentementeVariasPalabras)

LT_BEGIN_TEST(TestsEjercicio2, ClavesSonCorrectasTrasIncrementarConcurrentementeVariasPalabras)
    auto incrementar_palabra_n_veces = [&](std::string palabra, int n) {
        for (int i = 0; i < n; ++i) {
            hM.incrementar(palabra);
        }
    };

    std::thread t1(incrementar_palabra_n_veces, "heladera", 5);
    std::thread t2(incrementar_palabra_n_veces, "heladera", 5);
    std::thread t3(incrementar_palabra_n_veces, "lavarropas", 5);
    std::thread t4(incrementar_palabra_n_veces, "mousepad", 5);

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    std::vector<std::string> actual = hM.claves();
    LT_CHECK_EQ(actual.size(), 3);
    LT_CHECK(std::find(actual.begin(), actual.end(), "heladera") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "lavarropas") != actual.end());
    LT_CHECK(std::find(actual.begin(), actual.end(), "mousepad") != actual.end());

LT_END_TEST(ClavesSonCorrectasTrasIncrementarConcurrentementeVariasPalabras)

// Tests Ejercicio 3

LT_BEGIN_SUITE(TestsEjercicio3)

HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio3)

LT_BEGIN_TEST(TestsEjercicio3, PromedioEsCorrecto)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Microondas");
    hM.incrementar("Microondas");

    float actual = hM.promedio();
    LT_CHECK_EQ(actual, 3);
LT_END_TEST(PromedioEsCorrecto)

// Tests concurrencia

LT_BEGIN_TEST(TestsEjercicio3, PromedioConcurrenteEsCorrecto)
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Heladera");
    hM.incrementar("Microondas");
    hM.incrementar("Microondas");

    float promedio_no_concurrente = hM.promedio();
    float promedio_concurrente = hM.promedio_concurrente(5);

    LT_CHECK_EQ(promedio_no_concurrente, promedio_concurrente);
LT_END_TEST(PromedioConcurrenteEsCorrecto)

LT_BEGIN_TEST(TestsEjercicio3, PromedioConcurrenteEsCorrectoStress)
        hM.incrementar("Heladera");
        hM.incrementar("Heladera");
        hM.incrementar("Heladera");
        hM.incrementar("Heladera");
        hM.incrementar("Microondas");
        hM.incrementar("Microondas");

        float promedio_no_concurrente = hM.promedio();
        for(int i = 1; i < 30; i++) {
            //cout << i << endl;
            float promedio_concurrente = hM.promedio_concurrente(i);
            LT_CHECK_EQ(promedio_no_concurrente, promedio_concurrente);
        }
LT_END_TEST(PromedioConcurrenteEsCorrectoStress)

LT_BEGIN_SUITE(TestsEjercicio4)

HashMapConcurrente hM;

void set_up()
{
}

void tear_down()
{
}
LT_END_SUITE(TestsEjercicio4)

LT_BEGIN_TEST(TestsEjercicio4, CargarArchivoFunciona)
   cargarArchivo(hM, "data/test-1");
   LT_CHECK_EQ(hM.valor("Heladera"), 1);
   LT_CHECK_EQ(hM.claves().size(), 5);
LT_END_TEST(CargarArchivoFunciona)

LT_BEGIN_TEST(TestsEjercicio4, CargarMultiplesArchivosFuncionaUnThread)
   cargarMultiplesArchivos(hM, 1, {"data/test-1", "data/test-2", "data/test-3"});
   LT_CHECK_EQ(hM.valor("Heladera"), 2);
   LT_CHECK_EQ(hM.valor("NotebookOMEN"), 3);
   LT_CHECK_EQ(hM.valor("Microondas"), 4);
   LT_CHECK_EQ(hM.claves().size(), 12);
LT_END_TEST(CargarMultiplesArchivosFuncionaUnThread)

LT_BEGIN_TEST(TestsEjercicio4, CargarMultiplesArchivosFuncionaDosThreads)
   cargarMultiplesArchivos(hM, 2, {"data/test-1", "data/test-2", "data/test-3"});
   LT_CHECK_EQ(hM.valor("Heladera"), 2);
   LT_CHECK_EQ(hM.valor("NotebookOMEN"), 3);
   LT_CHECK_EQ(hM.valor("Microondas"), 4);
   LT_CHECK_EQ(hM.claves().size(), 12);
LT_END_TEST(CargarMultiplesArchivosFuncionaDosThreads)

// Ejecutar tests
LT_BEGIN_AUTO_TEST_ENV()
    AUTORUN_TESTS()
LT_END_AUTO_TEST_ENV()
