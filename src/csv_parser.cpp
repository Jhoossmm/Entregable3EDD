#include "../include/csv_parser.h"
#include <iostream>
#include <fstream>
#include <sstream>

//Funcion auxiliar para separar una línea respetando comillas
std::vector<std::string> splitCSVLine(const std::string& line) {
    std::vector<std::string> result; //Vector para almacenar los campos separados
    std::string current_field; //Variable para almacenar el campo actual
    bool in_quotes = false; //Variable para rastrear si estamos dentro de comillas, porque dentro de los twitts puede haber comas que no deben separar campos
    
    for (char c : line) {
        if (c == '\"') { //Si encontramos una comilla, alternamos el estado de in_quotes
            in_quotes = !in_quotes;
        } else if (c == ',' && !in_quotes) { //Si encontramos una coma y no estamos dentro de comillas, es el final de un campo
            result.push_back(current_field);
            current_field.clear();
        } else { //Si es cualquier otro carácter, lo agregamos al campo actual
            current_field += c;
        }
    }
    result.push_back(current_field); // Agregar el último campo
    return result;
}

//Funcion para parsear el archivo CSV completo y devolver un vector de TweetRecord
std::vector<TweetRecord> parseCSV(const std::string& filepath) {
    std::vector<TweetRecord> records;
    std::ifstream file(filepath);
    
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filepath << std::endl;
        return records;
    }

    std::string line;
    //Leer y descartar la cabecera
    std::getline(file, line); 

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::vector<std::string> fields = splitCSVLine(line);
        
        //Solo para asegurarnos de que la línea tiene al menos 8 columnas, porque el índice 7 es user_screen_name, y asi evitar errores
        if (fields.size() > 7) {
            try {
                TweetRecord record;
                //user_id está en la columna 5
                record.user_id = std::stoll(fields[5]);
                //user_screen_name está en la columna 7
                record.user_screen_name = fields[7];
                
                records.push_back(record);
            } catch (const std::exception& e) {
                //Ignorar filas malformadas
            }
        }
    }

    file.close();
    return records;
}