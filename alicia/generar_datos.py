"""
╔══════════════════════════════════════════════════════╗
║  ALICIA — Generador de Datos de Entrenamiento        ║
║  Skell's GreenHouse V3.0                             ║
╚══════════════════════════════════════════════════════╝

Este script genera el archivo CSV con los datos de entrenamiento
para la red neuronal Alicia. Te pedirá los rangos de valores que
experimentará tu invernadero (mínimos y máximos de temperatura y
humedad) y generará muestras realistas con lógica determinística.

Uso:
    python generar_datos.py

Salida:
    datos_entrenamiento.csv
"""

import numpy as np
import csv


# ═══════════════════════════════════════════════════════════════
#   LÓGICA DE LA IA (Reglas determinísticas que la red aprenderá)
# ═══════════════════════════════════════════════════════════════

def calcular_uv(luz: float, perfil: int, temp_amb: float) -> int:
    """
    Calcula la intensidad UV que deben emitir los LEDs.

    Lógica:
    - "Mientras más luz exterior haya, más intenso el UV" — para forzar
      a la planta a crecer derecho (fototropismo desde arriba).
    - El perfil ajusta la intensidad base:
        Desértico (1): +15% (estas plantas toleran y necesitan más UV)
        Regular   (2): base (sin ajuste)
        Alpino    (3): -25% (las plantas alpinas prefieren menos intensidad)
    - Si hay calor extremo (>35°C), se reduce un 15% para evitar estrés.
    - En la oscuridad casi total (<15%), se garantiza un mínimo de UV
      para mantener el ciclo de crecimiento nocturno.
    """
    multiplicadores = {1: 1.15, 2: 1.0, 3: 0.75}
    uv = luz * 2.55 * multiplicadores[perfil]   # escalar 0-100 → 0-255

    if temp_amb > 35.0:
        uv *= 0.85                               # alivio térmico

    if luz < 15.0:
        minimos = {1: 40, 2: 30, 3: 20}
        uv = max(uv, float(minimos[perfil]))    # mínimo nocturno por perfil

    return int(np.clip(uv, 0, 255))


def calcular_riego(temp_amb: float, temp_agua: float,
                   hum_amb: float, hum_suelo: float,
                   perfil: int) -> bool:
    """
    Decide si se debe activar el riego.

    Lógica:
    - Si el agua está muy fría (<8°C) o muy caliente (>40°C) → no regar
      (riesgo de choque térmico en las raíces).
    - Cada perfil tiene umbrales propios de humedad del suelo:
        Desértico (1): suelo_seco < 25 %, suelo_húmedo > 50 %
        Regular   (2): suelo_seco < 40 %, suelo_húmedo > 65 %
        Alpino    (3): suelo_seco < 55 %, suelo_húmedo > 80 %
    - Si el suelo está por debajo del umbral_seco → regar siempre.
    - Si el suelo está por encima del umbral_húmedo → nunca regar.
    - Zona intermedia: regar si el ambiente está muy seco (baja humedad
      del aire) o si hace mucho calor (alta evapotranspiración).
    """
    # Temperatura del agua extrema: proteger raíces
    if temp_agua < 8.0 or temp_agua > 40.0:
        return False

    config = {
        1: {'seco': 25.0, 'humedo': 50.0, 'amb_seco': 25.0, 'temp_calor': 35.0},  # Desértico
        2: {'seco': 40.0, 'humedo': 65.0, 'amb_seco': 40.0, 'temp_calor': 30.0},  # Regular
        3: {'seco': 55.0, 'humedo': 80.0, 'amb_seco': 55.0, 'temp_calor': 25.0},  # Alpino
    }[perfil]

    if hum_suelo < config['seco']:
        return True                              # suelo muy seco: regar siempre
    if hum_suelo > config['humedo']:
        return False                             # suelo ya húmedo: no regar

    # Zona intermedia — condiciones ambientales deciden
    if hum_amb < config['amb_seco']:
        return True                              # aire muy seco: evaporación alta
    if temp_amb > config['temp_calor']:
        return True                              # calor intenso: más evapotranspiración

    return False


# ═══════════════════════════════════════════════════════════════
#   ENTRADA DE RANGOS POR CONSOLA
# ═══════════════════════════════════════════════════════════════

def pedir_rango(nombre: str, default_min: float, default_max: float,
                unidad: str = "") -> tuple[float, float]:
    """Solicita al usuario el rango mínimo/máximo de una variable."""
    print(f"\n  {nombre}:")
    try:
        raw_min = input(f"    Mínimo  [{default_min}{unidad}]: ").strip()
        raw_max = input(f"    Máximo  [{default_max}{unidad}]: ").strip()
        vmin = float(raw_min) if raw_min else default_min
        vmax = float(raw_max) if raw_max else default_max
    except ValueError:
        print("    ⚠  Entrada inválida — usando valores por defecto.")
        return float(default_min), float(default_max)

    if vmin >= vmax:
        print("    ⚠  El mínimo debe ser menor al máximo — usando valores por defecto.")
        return float(default_min), float(default_max)

    return vmin, vmax


# ═══════════════════════════════════════════════════════════════
#   MAIN
# ═══════════════════════════════════════════════════════════════

def main():
    print("=" * 58)
    print("  ALICIA — Generador de Datos de Entrenamiento")
    print("  Skell's GreenHouse V3.0")
    print("=" * 58)
    print("\nIngresa los rangos de valores que experimentará")
    print("tu invernadero. ENTER = valor por defecto.\n")

    t_amb_min,  t_amb_max  = pedir_rango("Temperatura Ambiente",   10.0, 45.0,  "°C")
    t_agua_min, t_agua_max = pedir_rango("Temperatura del Agua",    5.0, 40.0,  "°C")
    h_amb_min,  h_amb_max  = pedir_rango("Humedad Ambiente",       20.0, 95.0,  "%")
    h_sue_min,  h_sue_max  = pedir_rango("Humedad del Suelo",       5.0, 95.0,  "%")

    print()
    try:
        raw = input("  Muestras a generar [8000]: ").strip()
        n_samples = int(raw) if raw else 8000
        if n_samples < 100:
            print("  ⚠  Mínimo 100 muestras. Usando 8000.")
            n_samples = 8000
    except ValueError:
        n_samples = 8000

    output_file = "datos_entrenamiento.csv"

    print(f"\nGenerando {n_samples:,} muestras...\n")

    np.random.seed(42)

    conteo_riego    = 0
    conteo_no_riego = 0

    with open(output_file, "w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow([
            "temp_ambiente", "temp_agua", "humedad_ambiente",
            "humedad_suelo", "intensidad_luz", "perfil",
            "uv", "riego"
        ])

        for _ in range(n_samples):
            # Valores base aleatorios dentro del rango del usuario
            t_amb  = np.random.uniform(t_amb_min,  t_amb_max)
            t_agua = np.random.uniform(t_agua_min, t_agua_max)
            h_amb  = np.random.uniform(h_amb_min,  h_amb_max)
            h_sue  = np.random.uniform(h_sue_min,  h_sue_max)
            luz    = np.random.uniform(0.0, 100.0)
            perfil = np.random.randint(1, 4)            # 1, 2 o 3

            # ── Etiquetas determinísticas ──────────────────────
            uv    = calcular_uv(luz, perfil, t_amb)
            riego = calcular_riego(t_amb, t_agua, h_amb, h_sue, perfil)

            if riego:
                conteo_riego += 1
            else:
                conteo_no_riego += 1

            # ── Ruido gaussiano suave en las entradas ──────────
            # Simula la imprecisión real de los sensores y mejora
            # la capacidad de generalización de la red.
            t_amb  += np.random.normal(0.0, 0.4)
            t_agua += np.random.normal(0.0, 0.3)
            h_amb  += np.random.normal(0.0, 0.8)
            h_sue  += np.random.normal(0.0, 0.5)
            luz    += np.random.normal(0.0, 0.5)

            # Clampear a rangos físicamente posibles
            t_amb  = float(np.clip(t_amb,  t_amb_min  - 5.0, t_amb_max  + 5.0))
            t_agua = float(np.clip(t_agua, t_agua_min - 3.0, t_agua_max + 3.0))
            h_amb  = float(np.clip(h_amb,  0.0, 100.0))
            h_sue  = float(np.clip(h_sue,  0.0, 100.0))
            luz    = float(np.clip(luz,    0.0, 100.0))

            writer.writerow([
                round(t_amb,  2),
                round(t_agua, 2),
                round(h_amb,  2),
                round(h_sue,  2),
                round(luz,    1),
                int(perfil),
                int(uv),
                int(riego),
            ])

    pct_riego = 100.0 * conteo_riego / n_samples
    print(f"  Archivo generado : {output_file}")
    print(f"  Total muestras   : {n_samples:,}")
    print(f"  Riego = SÍ       : {conteo_riego:,}  ({pct_riego:.1f}%)")
    print(f"  Riego = NO       : {conteo_no_riego:,}  ({100.0 - pct_riego:.1f}%)")
    print()
    print("Siguiente paso:")
    print("  python entrenar_alicia.py")


if __name__ == "__main__":
    main()
