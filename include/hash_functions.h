#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <string>
#include <cstddef>

//Funcion hash para user_id (long long)
inline size_t hash_user_id(const long long& key, size_t m) {
    return key % m;
}

//Funcion hash secundaria para user_id (long long) para Double Hashing
inline size_t hash2_user_id(const long long& key, size_t m) {
    return 1 + (key % (m - 1));
}

//Funcion hash para user_screen_name (std::string)
inline size_t hash_screen_name(const std::string& key, size_t m) {
    size_t hash_val = 0;
    size_t p = 31;
    size_t p_pow = 1;
    for (char c : key) {
        hash_val = (hash_val + c * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return hash_val;
}

//Funcion hash secundaria para user_screen_name (std::string) para Double Hashing
inline size_t hash2_screen_name(const std::string& key, size_t m) {
    size_t hash_val = 0;
    for (char c : key) {
        hash_val = (hash_val * 37 + c) % (m - 1);
    }
    return 1 + hash_val;
}

#endif