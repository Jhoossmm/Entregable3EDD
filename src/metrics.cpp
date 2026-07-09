#include "../include/metrics.h"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

// calcula el promedio de un grupo (las n repeticiones con misma estructura, clave y tamaño [los primeros 10k, 20k...])
double computeMean(const vector<long long>& values) {
    if (values.empty()) {
        return 0.0;
    }

    long double sum = 0.0;
    for (long long value : values) {
        sum += value;
    }
    return static_cast<double>(sum / values.size());
}

// calcula la desviacion estandar muestral para el grupo
double computeStdDevSample(const vector<long long>& values, double mean) {
    if (values.size() < 2) {
        return 0.0;
    }

    long double sum_sq = 0.0;
    for (long long value : values) {
        long double diff = static_cast<long double>(value) - mean;
        sum_sq += diff * diff;
    }

    return static_cast<double>(sqrt(sum_sq / static_cast<long double>(values.size() - 1)));
}

// lee resultados crudos y escribe metricas agregadas
void writeSummaryCSV(const string& raw_csv_path, const string& summary_csv_path) {
    ifstream in(raw_csv_path);
    if (!in.is_open()) {
        cerr << "Error: No se pudo abrir '" << raw_csv_path << "' para calcular resumen." << endl;
        return;
    }

    string header;
    getline(in, header);

    map<tuple<string, string, long long>, vector<long long>> grouped_times;
    string line;

    while (getline(in, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        string exp_s, dataset, structure, key_name, queries_s, time_s;

        getline(ss, exp_s, ';');
        getline(ss, dataset, ';');
        getline(ss, structure, ';');
        getline(ss, key_name, ';');
        getline(ss, queries_s, ';');
        getline(ss, time_s, ';');

        if (dataset.empty() || structure.empty() || key_name.empty() || queries_s.empty() || time_s.empty()) {
            continue;
        }

        long long queries = stoll(queries_s);
        long long time_us = stoll(time_s);

        grouped_times[make_tuple(structure, key_name, queries)].push_back(time_us);
    }

    in.close();

    ofstream out(summary_csv_path);
    if (!out.is_open()) {
        cerr << "Error: No se pudo crear '" << summary_csv_path << "'." << endl;
        return;
    }

    out << "dataset;estructura_de_datos;clave;cantidad_consultas;promedio_tiempo_us;desviacion_estandar_us;n_repeticiones\n";

    out << fixed << setprecision(2);
    for (const auto& entry : grouped_times) {
        const auto& key = entry.first;
        const auto& times = entry.second;

        const string& structure = get<0>(key);
        const string& key_name = get<1>(key);
        long long queries = get<2>(key);

        double mean = computeMean(times);
        double stddev = computeStdDevSample(times, mean);

        out << "auspol2019;"
            << structure << ";"
            << key_name << ";"
            << queries << ";"
            << mean << ";"
            << stddev << ";"
            << times.size() << "\n";
    }

    out.close();
}
