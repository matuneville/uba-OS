#ifndef LISTA_ATOMICA_HPP
#define LISTA_ATOMICA_HPP

using namespace std;

#include <atomic>
#include <cstddef>
#include <mutex>


template<typename T>
class ListaAtomica {
 private:
    struct Nodo {
        Nodo(const T &val) : _valor(val), _siguiente(nullptr) {}

        T _valor;
        Nodo *_siguiente;
    };

    std::atomic<Nodo *> _cabeza;
    mutex mutex_insertar_lista;

 public:
    ListaAtomica() : _cabeza(nullptr) {}

    ~ListaAtomica() {
        Nodo *n, *t;
        n = _cabeza.load();
        while (n) {
            t = n;
            n = n->_siguiente;
            delete t;
        }
    }

    void insertar(const T &valor) {
        Nodo* nueva_cabeza = new Nodo(valor);

        mutex_insertar_lista.lock();
        Nodo* cabeza_actual = _cabeza.load();
        if (cabeza_actual == NULL ) {
            //    CASO1: está vacia!
            this->_cabeza.store(nueva_cabeza);

        } else {
            //    CASO2: ya hay nodos
            nueva_cabeza->_siguiente = cabeza_actual;

            this->_cabeza.store(nueva_cabeza);
        }
        mutex_insertar_lista.unlock();
    }

    T& operator[](size_t i) const {
        Nodo *n = _cabeza.load();
        for (size_t j = 0; j < i; j++) {
            n = n->_siguiente;
        }
        return n->_valor;
    }

    unsigned int longitud() const {
        Nodo *n = _cabeza.load();
        unsigned int cant = 0;
        while (n != nullptr) {
            cant++;
            n = n->_siguiente;
        }
        return cant;
    }

    struct iterator {
    private:
        ListaAtomica *_lista;

        typename ListaAtomica::Nodo *_nodo_sig;

        iterator(ListaAtomica<T> *lista, typename ListaAtomica<T>::Nodo *sig)
            : _lista(lista), _nodo_sig(sig) {}

    public:
        iterator &operator=(const typename ListaAtomica::iterator &otro) {
            _lista = otro._lista;
            _nodo_sig = otro._nodo_sig;
            return *this;
        }

        T& operator*() {
            return _nodo_sig->_valor;
        }

        iterator& operator++() {
            _nodo_sig = _nodo_sig->_siguiente;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const typename ListaAtomica::iterator &otro) const {
            return _lista->_cabeza.load() == otro._lista->_cabeza.load()
                && _nodo_sig == otro._nodo_sig;
        }

        bool operator!=(const typename ListaAtomica::iterator &otro) const {
            return !(*this == otro);
        }

        friend iterator ListaAtomica<T>::begin();
        friend iterator ListaAtomica<T>::end();
    };

    iterator begin() {
        return iterator(this, _cabeza);
    }

    iterator end() {
        return iterator(this, nullptr);
    }
};

#endif /* LISTA_ATOMICA_HPP */
