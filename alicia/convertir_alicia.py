"""
╔══════════════════════════════════════════════════════╗
║  ALICIA — Conversión TFLite + Generación de Header   ║
║  Skell's GreenHouse V3.0                             ║
╚══════════════════════════════════════════════════════╝

Convierte el modelo entrenado (alicia.keras) a formato
TensorFlow Lite (.tflite) y luego genera el archivo C
header (.h) que se incluye directamente en el sketch de
Arduino del ESP32-S3.

Uso:
    python convertir_alicia.py

Salida:
    alicia.tflite          Modelo TFLite (binario)
    alicia_model.h         Header C para Arduino
    (Copia alicia_model.h → ../core/alicia_model.h)
"""

import os
import sys
import struct

try:
    import tensorflow as tf
except ImportError:
    print("ERROR: TensorFlow no está instalado.")
    print("       Ejecuta: pip install -r requirements.txt")
    sys.exit(1)


# ═══════════════════════════════════════════════════════════════
#   CONVERSIÓN A TFLITE
# ═══════════════════════════════════════════════════════════════

def convertir_a_tflite(ruta_modelo: str, ruta_salida: str) -> bytes:
    """Convierte modelo Keras (.keras) → TFLite con optimización por defecto."""
    if not os.path.isfile(ruta_modelo):
        print(f"ERROR: No se encontró el modelo en '{ruta_modelo}'")
        print("       Ejecuta primero: python entrenar_alicia.py")
        sys.exit(1)

    print(f"Cargando modelo desde: {ruta_modelo}")
    keras_model = tf.keras.models.load_model(ruta_modelo)
    converter = tf.lite.TFLiteConverter.from_keras_model(keras_model)

    # Optimización DEFAULT:
    # - Comprime pesos de float32 a int8 (cuantización dinámica).
    # - Las activaciones siguen siendo float32 en tiempo de ejecución.
    # - Reduce tamaño del modelo ~75% sin pérdida significativa de precisión.
    converter.optimizations = [tf.lite.Optimize.DEFAULT]

    print("Convirtiendo a TFLite...")
    tflite_model = converter.convert()

    with open(ruta_salida, "wb") as f:
        f.write(tflite_model)

    kb = len(tflite_model) / 1024.0
    print(f"Modelo TFLite guardado: {ruta_salida}  ({kb:.2f} KB)")
    return tflite_model


# ═══════════════════════════════════════════════════════════════
#   GENERACIÓN DEL HEADER C
# ═══════════════════════════════════════════════════════════════

def generar_header(tflite_bytes: bytes, ruta_header: str) -> None:
    """Genera el archivo .h con el array C del modelo TFLite."""
    nombre_array = "alicia_model_data"
    bytes_por_linea = 12

    lineas_hex = []
    for i in range(0, len(tflite_bytes), bytes_por_linea):
        chunk = tflite_bytes[i:i + bytes_por_linea]
        linea = ", ".join(f"0x{b:02x}" for b in chunk)
        lineas_hex.append(f"  {linea}")

    hex_block = ",\n".join(lineas_hex)

    contenido = f"""\
/*
 * alicia_model.h — Modelo TFLite de Alicia
 * Skell's GreenHouse V3.0
 *
 * GENERADO AUTOMÁTICAMENTE por convertir_alicia.py
 * NO editar a mano.
 *
 * Tamaño del modelo: {len(tflite_bytes)} bytes ({len(tflite_bytes)/1024:.2f} KB)
 *
 * Arquitectura:
 *   Entrada  : 6 neuronas  (sensores normalizados)
 *   Oculta 1 : 16 neuronas (ReLU)
 *   Oculta 2 :  8 neuronas (ReLU)
 *   Salida   :  2 neuronas (Sigmoid)
 *              [0] Intensidad UV  (0.0 → 1.0, escalar × 255)
 *              [1] Riego          (umbral 0.5 → bool)
 */

#pragma once
#include <stdint.h>

alignas(8) const unsigned char {nombre_array}[] = {{
{hex_block}
}};

const unsigned int {nombre_array}_len = {len(tflite_bytes)}U;
"""

    with open(ruta_header, "w", encoding="utf-8") as f:
        f.write(contenido)

    print(f"Header C generado   : {ruta_header}")


# ═══════════════════════════════════════════════════════════════
#   VERIFICACIÓN RÁPIDA CON EL INTÉRPRETE TFLITE
# ═══════════════════════════════════════════════════════════════

def verificar_modelo(tflite_bytes: bytes) -> None:
    """Ejecuta una inferencia de prueba para confirmar que el modelo es válido."""
    import numpy as np

    print("\nVerificando modelo con inferencia de prueba...")
    interpreter = tf.lite.Interpreter(model_content=tflite_bytes)
    interpreter.allocate_tensors()

    input_details  = interpreter.get_input_details()
    output_details = interpreter.get_output_details()

    print(f"  Forma de entrada : {input_details[0]['shape']}")
    print(f"  Forma de salida  : {output_details[0]['shape']}")
    print(f"  Tipo de entrada  : {input_details[0]['dtype']}")

    # Ejemplo: Día caluroso, suelo seco, perfil regular
    # temp_amb=32°C, temp_agua=24°C, hum_amb=45%, hum_suelo=30%, luz=85%, perfil=2
    entrada = np.array([[
        (32.0 - (-10.0)) / (50.0 - (-10.0)),   # temp_amb normalizada
        (24.0 -   5.0)   / (45.0 -   5.0),     # temp_agua normalizada
        (45.0 -   0.0)   / (100.0 - 0.0),       # hum_amb normalizada
        (30.0 -   0.0)   / (100.0 - 0.0),       # hum_suelo normalizada
        (85.0 -   0.0)   / (100.0 - 0.0),       # luz normalizada
        (2.0  -   1.0)   / (3.0   - 1.0),       # perfil normalizado (regular=0.5)
    ]], dtype=np.float32)

    interpreter.set_tensor(input_details[0]['index'], entrada)
    interpreter.invoke()
    salida = interpreter.get_tensor(output_details[0]['index'])

    uv_raw   = salida[0][0]
    riego    = salida[0][1]
    uv_final = int(uv_raw * 255)
    riego_b  = riego > 0.5

    print(f"\n  Prueba: día caluroso (32°C), suelo seco (30%), luz alta (85%), perfil Regular")
    print(f"  Salida UV    : {uv_raw:.4f}  →  {uv_final} / 255")
    print(f"  Salida Riego : {riego:.4f}   →  {'ENCENDIDO' if riego_b else 'APAGADO'}")
    print("\n  Resultado esperado: UV alto (mucha luz), Riego ENCENDIDO (suelo seco)")


# ═══════════════════════════════════════════════════════════════
#   MAIN
# ═══════════════════════════════════════════════════════════════

def main():
    print("=" * 58)
    print("  ALICIA — Conversión a TFLite")
    print("  Skell's GreenHouse V3.0")
    print("=" * 58)
    print()

    tflite_bytes = convertir_a_tflite("alicia.keras", "alicia.tflite")
    generar_header(tflite_bytes, "alicia_model.h")
    verificar_modelo(tflite_bytes)

    destino_core = os.path.join("..", "core", "alicia_model.h")
    print(f"\n{'='*58}")
    print("SIGUIENTE PASO:")
    print(f"  Copia el archivo generado al directorio core/:")
    print()
    print(f"  Windows:  copy alicia_model.h ..\\core\\alicia_model.h")
    print(f"  Linux/Mac: cp alicia_model.h ../core/alicia_model.h")
    print()
    print("  Luego abre Arduino IDE y compila core/core.ino")
    print(f"{'='*58}")

    # Intento de copia automática
    try:
        import shutil
        shutil.copy("alicia_model.h", destino_core)
        print(f"\n  [OK] Copia automática exitosa → {destino_core}")
    except Exception as e:
        print(f"\n  (Copia automática no disponible: {e})")
        print("  Copia manualmente como se indica arriba.")


if __name__ == "__main__":
    main()
