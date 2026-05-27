"""
╔══════════════════════════════════════════════════════╗
║  ALICIA — Entrenamiento de la Red Neuronal           ║
║  Skell's GreenHouse V3.0                             ║
╚══════════════════════════════════════════════════════╝

Carga el CSV generado por generar_datos.py, normaliza las
entradas, construye la red neuronal Alicia y la entrena.
Al finalizar guarda el modelo en formato SavedModel y .h5.

Arquitectura:
    Entrada  → 6 neuronas  (sensores normalizados)
    Oculta 1 → 16 neuronas (ReLU)
    Oculta 2 →  8 neuronas (ReLU)
    Salida   →  2 neuronas (Sigmoid)
               [0] UV normalizado  → × 255 en el dispositivo
               [1] Riego           → umbral 0.5

Uso:
    python entrenar_alicia.py

Salida:
    alicia_saved/          Modelo en formato SavedModel
    alicia.keras           Modelo en formato Keras nativo
    entrenamiento.png      Gráfica de pérdida (opcional)
"""

import os
import sys
import numpy as np
import csv

# ── Validación temprana de dependencias ─────────────────────────────────────
try:
    import tensorflow as tf
    from tensorflow import keras
except ImportError:
    print("ERROR: TensorFlow no está instalado.")
    print("       Ejecuta: pip install -r requirements.txt")
    sys.exit(1)

try:
    from sklearn.model_selection import train_test_split
    from sklearn.metrics import classification_report
except ImportError:
    print("ERROR: scikit-learn no está instalado.")
    print("       Ejecuta: pip install -r requirements.txt")
    sys.exit(1)


# ═══════════════════════════════════════════════════════════════
#   RANGOS DE NORMALIZACIÓN (deben coincidir con alicia.h)
# ═══════════════════════════════════════════════════════════════

NORM = {
    "temp_ambiente": (-10.0, 50.0),
    "temp_agua":     (  5.0, 45.0),
    "humedad_ambiente": (0.0, 100.0),
    "humedad_suelo":    (0.0, 100.0),
    "intensidad_luz":   (0.0, 100.0),
    "perfil":           (1.0,   3.0),
}

COLUMNAS_ENTRADA = list(NORM.keys())
COLUMNAS_SALIDA  = ["uv", "riego"]


# ═══════════════════════════════════════════════════════════════
#   CARGA Y PREPROCESAMIENTO
# ═══════════════════════════════════════════════════════════════

def cargar_csv(ruta: str) -> tuple[np.ndarray, np.ndarray]:
    """Lee el CSV y devuelve (X normalizado, y normalizado)."""
    if not os.path.isfile(ruta):
        print(f"ERROR: No se encontró '{ruta}'")
        print("       Ejecuta primero: python generar_datos.py")
        sys.exit(1)

    X_raw, y_raw = [], []
    with open(ruta, "r", encoding="utf-8") as f:
        reader = csv.DictReader(f)
        for fila in reader:
            X_raw.append([float(fila[col]) for col in COLUMNAS_ENTRADA])
            y_raw.append([float(fila[col]) for col in COLUMNAS_SALIDA])

    X_raw = np.array(X_raw, dtype=np.float32)
    y_raw = np.array(y_raw, dtype=np.float32)

    # Normalizar entradas a [0, 1]
    X_norm = np.zeros_like(X_raw)
    for i, col in enumerate(COLUMNAS_ENTRADA):
        vmin, vmax = NORM[col]
        X_norm[:, i] = np.clip((X_raw[:, i] - vmin) / (vmax - vmin), 0.0, 1.0)

    # Normalizar salidas a [0, 1]
    # UV: 0-255 → 0-1 | Riego: ya es 0 o 1
    y_norm = np.zeros_like(y_raw)
    y_norm[:, 0] = y_raw[:, 0] / 255.0   # UV
    y_norm[:, 1] = y_raw[:, 1]            # Riego (binario)

    return X_norm, y_norm


# ═══════════════════════════════════════════════════════════════
#   CONSTRUCCIÓN DEL MODELO
# ═══════════════════════════════════════════════════════════════

def construir_modelo() -> keras.Model:
    """
    Alicia — Perceptrón Multicapa (MLP).

    Capas:
      Entrada:   6 neuronas  — sensores normalizados
      Oculta 1: 16 neuronas  — ReLU (captura relaciones no lineales)
      Oculta 2:  8 neuronas  — ReLU (comprime representación)
      Salida:    2 neuronas  — Sigmoid (ambas salidas en [0,1])

    Parámetros totales:
      W1 + b1: 6×16 + 16  = 112
      W2 + b2: 16×8  + 8  = 136
      W3 + b3: 8×2   + 2  =  18
      ─────────────────────────
      Total                = 266 parámetros
    """
    modelo = keras.Sequential(
        [
            keras.layers.Input(shape=(6,), name="entrada"),
            keras.layers.Dense(16, activation="relu",    name="oculta_1"),
            keras.layers.Dense(8,  activation="relu",    name="oculta_2"),
            keras.layers.Dense(2,  activation="sigmoid", name="salida"),
        ],
        name="Alicia",
    )
    return modelo


# ═══════════════════════════════════════════════════════════════
#   ENTRENAMIENTO
# ═══════════════════════════════════════════════════════════════

def main():
    print("=" * 58)
    print("  ALICIA — Entrenamiento")
    print("  Skell's GreenHouse V3.0")
    print("=" * 58)

    # 1. Cargar datos
    print("\n[1/5] Cargando datos...")
    X, y = cargar_csv("datos_entrenamiento.csv")
    print(f"      Muestras cargadas : {len(X):,}")
    print(f"      Forma X           : {X.shape}")
    print(f"      Forma y           : {y.shape}")

    # 2. División entrenamiento / validación
    print("\n[2/5] Dividiendo datos (80% entrenamiento / 20% validación)...")
    X_train, X_val, y_train, y_val = train_test_split(
        X, y, test_size=0.20, random_state=42, shuffle=True
    )
    print(f"      Entrenamiento : {len(X_train):,} muestras")
    print(f"      Validación    : {len(X_val):,} muestras")

    # 3. Construir modelo
    print("\n[3/5] Construyendo modelo Alicia...")
    modelo = construir_modelo()
    modelo.summary()

    modelo.compile(
        optimizer=keras.optimizers.Adam(learning_rate=0.001),
        loss="mse",           # MSE funciona bien para ambas salidas normalizadas [0,1]
        metrics=["mae"],
    )

    # 4. Entrenar
    print("\n[4/5] Entrenando...")
    callbacks = [
        keras.callbacks.EarlyStopping(
            monitor="val_loss",
            patience=15,
            restore_best_weights=True,
            verbose=1,
        ),
        keras.callbacks.ReduceLROnPlateau(
            monitor="val_loss",
            factor=0.5,
            patience=8,
            min_lr=1e-6,
            verbose=1,
        ),
    ]

    historial = modelo.fit(
        X_train, y_train,
        validation_data=(X_val, y_val),
        epochs=300,
        batch_size=64,
        callbacks=callbacks,
        verbose=1,
    )

    # 5. Evaluación
    print("\n[5/5] Evaluando en conjunto de validación...")
    val_loss, val_mae = modelo.evaluate(X_val, y_val, verbose=0)
    print(f"      val_loss : {val_loss:.6f}")
    print(f"      val_mae  : {val_mae:.6f}")

    # Métricas específicas para el riego (clasificación binaria)
    y_pred  = modelo.predict(X_val, verbose=0)
    riego_pred_bin = (y_pred[:, 1] > 0.5).astype(int)
    riego_real_bin = (y_val[:, 1] > 0.5).astype(int)
    print("\n  Reporte de clasificación — Salida Riego:")
    print(classification_report(riego_real_bin, riego_pred_bin,
                                target_names=["No regar", "Regar"]))

    # Error en UV (en escala 0-255)
    uv_error = np.abs(y_pred[:, 0] - y_val[:, 0]) * 255.0
    print(f"  Error promedio UV (0-255) : {uv_error.mean():.2f}")
    print(f"  Error máximo UV (0-255)   : {uv_error.max():.2f}")

    # 6. Guardar modelo
    print("\nGuardando modelo...")
    os.makedirs("alicia_saved", exist_ok=True)
    modelo.save("alicia_saved")
    modelo.save("alicia.keras")
    print("  alicia_saved/   → formato SavedModel (para convertir a TFLite)")
    print("  alicia.keras    → formato Keras nativo (para re-entrenar)")

    # 7. Gráfica (opcional)
    try:
        import matplotlib.pyplot as plt
        plt.figure(figsize=(10, 4))
        plt.subplot(1, 2, 1)
        plt.plot(historial.history["loss"],     label="Entrenamiento")
        plt.plot(historial.history["val_loss"], label="Validación")
        plt.title("Pérdida (MSE)")
        plt.xlabel("Época")
        plt.ylabel("Loss")
        plt.legend()
        plt.grid(True, alpha=0.3)

        plt.subplot(1, 2, 2)
        plt.plot(historial.history["mae"],     label="Entrenamiento")
        plt.plot(historial.history["val_mae"], label="Validación")
        plt.title("Error Absoluto Medio (MAE)")
        plt.xlabel("Época")
        plt.ylabel("MAE")
        plt.legend()
        plt.grid(True, alpha=0.3)

        plt.tight_layout()
        plt.savefig("entrenamiento.png", dpi=150)
        print("  entrenamiento.png → gráfica del entrenamiento")
        plt.close()
    except Exception:
        pass

    print("\nSiguiente paso:")
    print("  python convertir_alicia.py")


if __name__ == "__main__":
    main()
