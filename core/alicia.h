/*╔═══════════════════════════════════════════════════════════╗
  ║  ALICIA — Módulo de Inferencia TFLite                     ║
  ║  Skell's GreenHouse V3.0                                  ║
  ╠═══════════════════════════════════════════════════════════╣
  ║  Encapsula toda la inicialización y ejecución de la       ║
  ║  red neuronal Alicia sobre TensorFlow Lite Micro.         ║
  ║                                                           ║
  ║  Librería requerida (Arduino Library Manager):            ║
  ║    "TensorFlow Lite Micro for Espressif Chips"            ║
  ║     o "Arduino_TensorFlowLite"                            ║
  ╚═══════════════════════════════════════════════════════════╝*/

#pragma once
#include <Arduino.h>
#include "sensors.h"
#include "alicia_model.h"

// Librerías TFLite Micro
#include <TensorFlowLite_ESP32.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_error_reporter.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <tensorflow/lite/schema/schema_generated.h>


/*╔═══════════════════════════════════════╗
  ║  Rangos de Normalización de Entradas  ║
  ╚═══════════════════════════════════════╝
  Deben coincidir exactamente con los usados en entrenar_alicia.py.
  Si ajustas los rangos en Python, actualiza también aquí.        */

#define ALICIA_TEMP_AMB_MIN   -10.0f
#define ALICIA_TEMP_AMB_MAX    50.0f
#define ALICIA_TEMP_AGUA_MIN    5.0f
#define ALICIA_TEMP_AGUA_MAX   45.0f
#define ALICIA_HUM_AMB_MIN      0.0f
#define ALICIA_HUM_AMB_MAX    100.0f
#define ALICIA_HUM_SUE_MIN      0.0f
#define ALICIA_HUM_SUE_MAX    100.0f
#define ALICIA_LUZ_MIN          0.0f
#define ALICIA_LUZ_MAX        100.0f
#define ALICIA_PERFIL_MIN       1.0f
#define ALICIA_PERFIL_MAX       3.0f

/* Tamaño del arena de tensores en KB.
   Asignado en PSRAM para no consumir la RAM interna que
   comparten la cámara y el servidor HTTP.                    */
#define ALICIA_ARENA_KB        16


namespace Alicia {

  // ── Estado interno ──────────────────────────────────────────
  static const tflite::Model*              _model       = nullptr;
  static tflite::MicroInterpreter*         _interpreter = nullptr;
  static uint8_t*                          _arena       = nullptr;
  static TfLiteTensor*                     _input       = nullptr;
  static TfLiteTensor*                     _output      = nullptr;
  static bool                              _listo       = false;


  // ── Normalizar un valor al rango [0, 1] ─────────────────────
  inline float _norm(float valor, float vmin, float vmax) {
    float r = (valor - vmin) / (vmax - vmin);
    if (r < 0.0f) r = 0.0f;
    if (r > 1.0f) r = 1.0f;
    return r;
  }


  /*╔═══════════════════════════════════════════════════════════╗
    ║  init() — Inicializar el intérprete TFLite               ║
    ║  Llamar una vez en setup(), después de inicializar PSRAM  ║
    ╚═══════════════════════════════════════════════════════════╝*/
  bool init() {
    // Intentar asignar el arena de tensores en PSRAM primero
    _arena = (uint8_t*)heap_caps_malloc(
      ALICIA_ARENA_KB * 1024,
      MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT
    );
    if (!_arena) {
      // Fallback a RAM interna si no hay PSRAM disponible
      Serial.println("[ALICIA] PSRAM no disponible, usando RAM interna...");
      _arena = (uint8_t*)malloc(ALICIA_ARENA_KB * 1024);
    }
    if (!_arena) {
      Serial.println("[ALICIA] ERROR: Sin memoria para tensor arena.");
      return false;
    }

    // Cargar modelo desde el array del header
    _model = tflite::GetModel(alicia_model_data);
    if (_model->version() != TFLITE_SCHEMA_VERSION) {
      Serial.printf("[ALICIA] ERROR: Versión del esquema incorrecta (%u != %u)\n",
                    _model->version(), TFLITE_SCHEMA_VERSION);
      return false;
    }

    // Registrar únicamente las operaciones que usa Alicia:
    //   FullyConnected → capas Dense
    //   Relu           → activación de capas ocultas
    //   Logistic       → activación Sigmoid de la capa de salida
    static tflite::MicroMutableOpResolver<3> resolver;
    resolver.AddFullyConnected();
    resolver.AddRelu();
    resolver.AddLogistic();

    // Crear el intérprete de forma estática (sin heap dinámico)
    static tflite::MicroErrorReporter micro_error_reporter;
    static tflite::MicroInterpreter static_interpreter(
      _model, resolver, _arena, ALICIA_ARENA_KB * 1024, &micro_error_reporter
    );
    _interpreter = &static_interpreter;

    if (_interpreter->AllocateTensors() != kTfLiteOk) {
      Serial.println("[ALICIA] ERROR: AllocateTensors() falló.");
      return false;
    }

    _input  = _interpreter->input(0);
    _output = _interpreter->output(0);

    // Verificar que la red tiene la forma esperada
    if (_input->dims->data[1] != 6 || _output->dims->data[1] != 2) {
      Serial.println("[ALICIA] ERROR: Dimensiones del modelo no coinciden (esperado 6→2).");
      return false;
    }

    _listo = true;
    Serial.printf("[ALICIA] Lista. Arena: %d KB en %s\n",
                  ALICIA_ARENA_KB,
                  heap_caps_get_free_size(MALLOC_CAP_SPIRAM) > 0 ? "PSRAM" : "RAM interna");
    return true;
  }


  /*╔═══════════════════════════════════════════════════════════╗
    ║  inferir() — Ejecutar la red neuronal                    ║
    ║                                                           ║
    ║  Entrada: struct Sensores con todos los datos leídos.     ║
    ║  Salida:  uv_out   → intensidad UV [0–255]               ║
    ║           riego_out → true = regar, false = no regar     ║
    ║                                                           ║
    ║  Retorna false si el intérprete no está inicializado.     ║
    ╚═══════════════════════════════════════════════════════════╝*/
  bool inferir(const Sensores& s, int& uv_out, bool& riego_out) {
    if (!_listo || !_interpreter) return false;

    // Cargar y normalizar las 6 entradas
    _input->data.f[0] = _norm(s.sen_temperatura_ambiente, ALICIA_TEMP_AMB_MIN,  ALICIA_TEMP_AMB_MAX);
    _input->data.f[1] = _norm(s.sen_temperatura_agua,     ALICIA_TEMP_AGUA_MIN, ALICIA_TEMP_AGUA_MAX);
    _input->data.f[2] = _norm(s.sen_humedad_ambiente,     ALICIA_HUM_AMB_MIN,   ALICIA_HUM_AMB_MAX);
    _input->data.f[3] = _norm(s.sen_humedad_suelo,        ALICIA_HUM_SUE_MIN,   ALICIA_HUM_SUE_MAX);
    _input->data.f[4] = _norm(s.sen_intensidad_luz,       ALICIA_LUZ_MIN,       ALICIA_LUZ_MAX);
    _input->data.f[5] = _norm((float)s.ia_perfilplanta,   ALICIA_PERFIL_MIN,    ALICIA_PERFIL_MAX);

    // Ejecutar la inferencia
    if (_interpreter->Invoke() != kTfLiteOk) {
      Serial.println("[ALICIA] ERROR: Invoke() falló.");
      return false;
    }

    // Leer y escalar las salidas
    uv_out    = (int)(_output->data.f[0] * 255.0f);
    riego_out = _output->data.f[1] > 0.5f;

    return true;
  }

} // namespace Alicia
