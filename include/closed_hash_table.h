#ifndef CLOSED_HASH_TABLE_H
#define CLOSED_HASH_TABLE_H

#include <vector>
#include <cstddef>

enum State { EMPTY, OCCUPIED, DELETED };

template <typename K>
class ClosedHashTable {
private:
    //Estructura para almacenar la clave, el contador de tweets y el estado de la celda
    struct Node {
        K key;
        int tweet_count;
        State state;
        Node() : state(EMPTY), tweet_count(0) {}
    };
    
    //Tabla hash cerrada representada como un vector de nodos
    std::vector<Node> table;
    size_t capacity;
    size_t (*hash1)(const K&, size_t);
    size_t (*hash2)(const K&, size_t);
    int probing_strategy; 

public:
    //Constructor que inicializa la tabla con la capacidad y las funciones hash proporcionadas
    ClosedHashTable(size_t size, size_t (*h1)(const K&, size_t), size_t (*h2)(const K&, size_t), int strategy)
        : capacity(size), hash1(h1), hash2(h2), probing_strategy(strategy) {
        table.resize(capacity);
    }

    //Insertar un nuevo elemento en la tabla hash cerrada. Si la clave ya existe, incrementa el contador de tweets.
    void insert(const K& key) {
        size_t base_index = hash1(key, capacity);
        size_t index = base_index;
        size_t i = 0;

        while (table[index].state == OCCUPIED) {
            if (table[index].key == key) {
                table[index].tweet_count++;
                return;
            }
            
            //Probing según la estrategia seleccionada
            i++;
            if (probing_strategy == 0) {
                index = (base_index + i) % capacity;                  
            } else if (probing_strategy == 1) {
                index = (base_index + i * i) % capacity;              
            } else if (probing_strategy == 2) {
                index = (base_index + i * hash2(key, capacity)) % capacity; 
            }
            
            if (i >= capacity) return; 
        }

        //Si encontramos un espacio vacío o eliminado, insertamos la nueva clave, inicializamos el contador de tweets y marcamos la celda como ocupada
        table[index].key = key;
        table[index].tweet_count = 1;
        table[index].state = OCCUPIED;
    }
};

#endif