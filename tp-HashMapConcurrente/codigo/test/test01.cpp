#include <iostream>
#include "../src/HashMapConcurrente.hpp"

using namespace std;

int main(){
    HashMapConcurrente hashmap_fruta = HashMapConcurrente();
    hashmap_fruta.incrementar("mandarina");
    
    for(auto clave : hashmap_fruta.claves()){
        cout << clave << endl;
    }

    return 0;
}