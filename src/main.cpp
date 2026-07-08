#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include "../include/csv_parser.h"
#include "../include/hash_functions.h"
#include "../include/open_hash_table.h"
#include "../include/closed_hash_table.h"

using namespace std;
using namespace std::chrono;

// Función auxiliar (Lambda como parámetro) para evitar repetir el ciclo de medición
template<typename Func>
void runExperiment(int exp, const string& dataset, const string& struct_name, 
                   size_t total_records, ofstream& out, Func&& insert_func) {
    
    auto start = high_resolution_clock::now();
    
    for (size_t i = 0; i < total_records; i++) {
        insert_func(i); // Llama a la inserción de la estructura correspondiente
        
        // Medir cada 10.000 tweets o en el último registro
        if ((i + 1) % 10000 == 0 || i == total_records - 1) {
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(end - start).count();
            // Imprime en formato: número_experimento; dataset; estructura_de_datos; cantidad_consultas; tiempo_ejecucion
            out << exp << ";" << dataset << ";" << struct_name << ";" << (i + 1) << ";" << duration << "\n";
        }
    }
}

int main() {
    cout << "Cargando dataset en memoria..." << endl;
    vector<TweetRecord> records = parseCSV("data/auspol2019.csv");
    size_t N = records.size();
    
    if (N == 0) {
        cerr << "Error: No se cargaron registros. Verifica la ruta del CSV." << endl;
        return 1;
    }
    cout << "Se cargaron " << N << " tweets exitosamente." << endl;

    // Configuración inicial
    int repeticiones = 20;
    size_t capacidad = 200003; // Número primo para reducir colisiones
    string dataset_name = "auspol2019";
    
    ofstream outCSV("resultados.csv");
    // Cabecera solicitada en la rúbrica
    outCSV << "numero_experimento;dataset;estructura_de_datos;cantidad_consultas;tiempo_ejecucion_us\n";

    cout << "Iniciando bateria de experimentos (20 repeticiones)... Esto tomara un tiempo." << endl;

    for (int exp = 1; exp <= repeticiones; exp++) {
        cout << "Ejecutando experimento " << exp << "/" << repeticiones << "..." << endl;

        // ====================================================================
        // 1. STL UNORDERED_MAP
        // ====================================================================
        unordered_map<long long, int> stl_id;
        runExperiment(exp, dataset_name, "STL_UnorderedMap_UserID", N, outCSV, [&](size_t i) {
            stl_id[records[i].user_id]++;
        });

        unordered_map<string, int> stl_name;
        runExperiment(exp, dataset_name, "STL_UnorderedMap_ScreenName", N, outCSV, [&](size_t i) {
            stl_name[records[i].user_screen_name]++;
        });

        // ====================================================================
        // 2. HASHING ABIERTO (Encadenamiento)
        // ====================================================================
        OpenHashTable<long long> open_id(capacidad, hash_user_id);
        runExperiment(exp, dataset_name, "OpenHash_UserID", N, outCSV, [&](size_t i) {
            open_id.insert(records[i].user_id);
        });

        OpenHashTable<string> open_name(capacidad, hash_screen_name);
        runExperiment(exp, dataset_name, "OpenHash_ScreenName", N, outCSV, [&](size_t i) {
            open_name.insert(records[i].user_screen_name);
        });

        // ====================================================================
        // 3. HASHING CERRADO (Linear Probing = 0)
        // ====================================================================
        ClosedHashTable<long long> closed_lin_id(capacidad, hash_user_id, hash2_user_id, 0);
        runExperiment(exp, dataset_name, "ClosedHash_Linear_UserID", N, outCSV, [&](size_t i) {
            closed_lin_id.insert(records[i].user_id);
        });

        ClosedHashTable<string> closed_lin_name(capacidad, hash_screen_name, hash2_screen_name, 0);
        runExperiment(exp, dataset_name, "ClosedHash_Linear_ScreenName", N, outCSV, [&](size_t i) {
            closed_lin_name.insert(records[i].user_screen_name);
        });

        // ====================================================================
        // 4. HASHING CERRADO (Quadratic Probing = 1)
        // ====================================================================
        ClosedHashTable<long long> closed_quad_id(capacidad, hash_user_id, hash2_user_id, 1);
        runExperiment(exp, dataset_name, "ClosedHash_Quad_UserID", N, outCSV, [&](size_t i) {
            closed_quad_id.insert(records[i].user_id);
        });

        ClosedHashTable<string> closed_quad_name(capacidad, hash_screen_name, hash2_screen_name, 1);
        runExperiment(exp, dataset_name, "ClosedHash_Quad_ScreenName", N, outCSV, [&](size_t i) {
            closed_quad_name.insert(records[i].user_screen_name);
        });

        // ====================================================================
        // 5. HASHING CERRADO (Double Hashing = 2)
        // ====================================================================
        ClosedHashTable<long long> closed_dh_id(capacidad, hash_user_id, hash2_user_id, 2);
        runExperiment(exp, dataset_name, "ClosedHash_Double_UserID", N, outCSV, [&](size_t i) {
            closed_dh_id.insert(records[i].user_id);
        });

        ClosedHashTable<string> closed_dh_name(capacidad, hash_screen_name, hash2_screen_name, 2);
        runExperiment(exp, dataset_name, "ClosedHash_Double_ScreenName", N, outCSV, [&](size_t i) {
            closed_dh_name.insert(records[i].user_screen_name);
        });
    }

    outCSV.close();
    cout << "Experimentos finalizados. Resultados exportados a 'resultados.csv'" << endl;
    return 0;
}