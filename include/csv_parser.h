#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <string>
#include <vector>

//Estructura para almacenar solo los datos necesarios para el conteo
struct TweetRecord {
    long long user_id;
    std::string user_screen_name;
};

//Funcion para parsear el archivo CSV completo
std::vector<TweetRecord> parseCSV(const std::string& filepath);

#endif