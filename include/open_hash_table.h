#ifndef OPEN_HASH_TABLE_H
#define OPEN_HASH_TABLE_H

#include <vector>
#include <list>
#include <cstddef>

template <typename K>
class OpenHashTable {
private:
    //Estructura para almacenar la clave y el contador de tweets
    struct Node {
        K key;
        int tweet_count;
    };
    
    //Tabla hash abierta representada como un vector de listas enlazadas
    std::vector<std::list<Node>> table;
    size_t capacity;
    size_t (*hash_func)(const K&, size_t);

public:
    //Constructor que inicializa la tabla con la capacidad y la función hash proporcionadas
    OpenHashTable(size_t size, size_t (*h)(const K&, size_t)) : capacity(size), hash_func(h) {
        table.resize(capacity);
    }

    //Insertar un nuevo elemento en la tabla hash abierta. Si la clave ya existe, incrementa el contador de tweets.
    void insert(const K& key) {
        size_t index = hash_func(key, capacity);
        
        for (auto& node : table[index]) {
            if (node.key == key) {
                node.tweet_count++;
                return;
            }
        }
        
        table[index].push_back({key, 1});
    }
};

#endif