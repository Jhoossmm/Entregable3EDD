#ifndef METRICS_H
#define METRICS_H

#include <string>

// lee resultados crudos y escribe metricas agregadas
void writeSummaryCSV(const std::string& raw_csv_path, const std::string& summary_csv_path);

#endif
