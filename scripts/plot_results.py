import csv
import os
from collections import defaultdict

import matplotlib.pyplot as plt

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
INPUT_CSV = os.path.join(ROOT, "resultados_metricas.csv")
OUT_DIR = os.path.join(ROOT, "plots")
os.makedirs(OUT_DIR, exist_ok=True)


def load_rows(path):
    rows = []
    with open(path, newline="", encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter=";")
        for row in reader:
            rows.append(
                {
                    "dataset": row["dataset"],
                    "estructura": row["estructura_de_datos"],
                    "clave": row["clave"],
                    "n": int(row["cantidad_consultas"]),
                    "mean_us": float(row["promedio_tiempo_us"]),
                    "std_us": float(row["desviacion_estandar_us"]),
                    "reps": int(row["n_repeticiones"]),
                }
            )
    return rows


def plot_time_by_key(rows, key_name, output_name):
    grouped = defaultdict(list)
    for r in rows:
        if r["clave"] == key_name:
            grouped[r["estructura"]].append((r["n"], r["mean_us"], r["std_us"]))

    plt.figure(figsize=(10, 6))
    for estructura, points in sorted(grouped.items()):
        points.sort(key=lambda x: x[0])
        xs = [p[0] for p in points]
        ys = [p[1] for p in points]
        yerr = [p[2] for p in points]
        plt.plot(xs, ys, marker="o", linewidth=1.8, label=estructura)
        plt.fill_between(xs, [y - e for y, e in zip(ys, yerr)], [y + e for y, e in zip(ys, yerr)], alpha=0.12)

    plt.title(f"Escalabilidad de tiempo por estructura ({key_name})")
    plt.xlabel("Cantidad de tweets procesados")
    plt.ylabel("Tiempo promedio de construccion (us)")
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(OUT_DIR, output_name), dpi=180)
    plt.close()


def plot_key_comparison_at_final_n(rows, output_name):
    max_n = max(r["n"] for r in rows)
    by_structure = defaultdict(dict)

    for r in rows:
        if r["n"] == max_n:
            by_structure[r["estructura"]][r["clave"]] = r["mean_us"]

    estructuras = sorted(by_structure.keys())
    user_id_vals = [by_structure[e].get("user_id", 0.0) for e in estructuras]
    screen_vals = [by_structure[e].get("user_screen_name", 0.0) for e in estructuras]

    x = list(range(len(estructuras)))
    width = 0.38

    plt.figure(figsize=(11, 6))
    plt.bar([i - width / 2 for i in x], user_id_vals, width=width, label="user_id")
    plt.bar([i + width / 2 for i in x], screen_vals, width=width, label="user_screen_name")

    plt.title(f"Comparacion de claves por estructura (n = {max_n})")
    plt.xlabel("Estructura de datos")
    plt.ylabel("Tiempo promedio de construccion (us)")
    plt.xticks(x, estructuras, rotation=20, ha="right")
    plt.grid(axis="y", alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.savefig(os.path.join(OUT_DIR, output_name), dpi=180)
    plt.close()


def main():
    rows = load_rows(INPUT_CSV)
    plot_time_by_key(rows, "user_id", "tiempo_escalabilidad_user_id.png")
    plot_time_by_key(rows, "user_screen_name", "tiempo_escalabilidad_user_screen_name.png")
    plot_key_comparison_at_final_n(rows, "comparacion_claves_n_final.png")

    print("Graficos generados en:")
    print(OUT_DIR)


if __name__ == "__main__":
    main()
