#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include "skells_model.h"

tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;

constexpr int kTensorArenaSize = 4.5 * 1024; // 4KB suele ser suficiente para redes pequeñas
uint8_t tensor_arena[kTensorArenaSize];

void setup() {
  Serial.begin(115200);
  
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  model = tflite::GetModel(skells_model); 

  static tflite::AllOpsResolver resolver;

  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  interpreter->AllocateTensors();

  input = interpreter->input(0);
  output = interpreter->output(0);
}

void loop() {
  // === VALORES REALES DE LOS SENSORES ===
  float t_real = 30.5;    // Celsius
  float h_real = 70.0;    // % Humedad Aire
  float s_real = 4000.0;  // Humedad Suelo (Lectura analógica 0-4095)
  float l_real = 4000.0;   // Luz (Lectura analógica 0-4095)
  float n_real = 3000.0;  // Nivel Tanque (Lectura analógica 0-4095)

  // === PASO A: NORMALIZACIÓN TOTAL (Indispensable) ===
  // Usamos la fórmula: (valor - min) / (max - min)
  // AJUSTA LOS MIN/MAX SEGÚN TU DATASET DE PYTHON
  /* input->data.f[0] = (t_real - 0.0) / (50.0 - 0.0);    // Temp (0 a 50)
  input->data.f[1] = (h_real - 0.0) / (100.0 - 0.0);   // Hum Aire (0 a 100)
  input->data.f[2] = (s_real - 0.0) / (4095.0 - 0.0);  // Hum Suelo (0 a 4095)
  input->data.f[3] = (l_real - 0.0) / (4095.0 - 0.0);  // Luz (0 a 4095)
  input->data.f[4] = (n_real - 0.0) / (4095.0 - 0.0);  // Tanque (0 a 4095)*/
  input->data.f[0] = 0.6;
  input->data.f[1] = 0.5;
  input->data.f[2] = 0.1;
  input->data.f[3] = 0.0;
  input->data.f[4] = 0.9;

  // === PASO B: Inferencia ===
  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Error al pensar.");
    return;
  }

  // === PASO C: Extraer y Des-normalizar (Si es necesario) ===
  float bomba_raw = output->data.f[0]; 
  float uv_raw    = output->data.f[1];

  // Si la salida UV fue entrenada de 0 a 255 y escalada a 0-1:
  // float uv_final = uv_raw * 255.0; 

  Serial.print("Bomba (Probabilidad): "); Serial.println(bomba_raw);
  Serial.print("Luz UV (PWM Estimado): "); Serial.println(uv_raw);
  
  delay(3000);
}