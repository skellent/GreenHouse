/* ╔ ║ ╚ ═ ╗ ╝ */

/*╔══╗
  ║  ║
  ╚══╝*/

/*╔═════════════════════════════════╗
  ║  SKELL'S GREENHOUSE V2.0        ║
  ╠═════════════════════════════════╣
  ║          Developed By:          ║
  ║ - Robert Rodríguez "Skellent"   ║
  ║    - Christopher Ramirez        ║
  ║     - Fabiana Hernandez         ║
  ╚═════════════════════════════════╝*/


/*╔═══════════════════════════════════════════╗
  ║ IMPORTACIÓN DE LIBRERÍAS NATIVAS/EXTERNAS ║
  ╚═══════════════════════════════════════════╝*/
#include <WiFi.h>
#include <WebServer.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "DHT.h"
#include <Preferences.h>
#include "esp_camera.h"


/*╔═══════════════════════════╗
  ║ IMPORTACIÓN DE TENSORFLOW ║
  ╚═══════════════════════════╝*/
#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"


/*╔════════════════════════════════╗
  ║ IMPORTACIÓN DE MÓDULOS PROPIOS ║
  ╚════════════════════════════════╝*/
#include "CONFIG.h"
#include "skells_model.h"


/*╔═══════════════════════════════════════════╗
  ║ CREACIÓN DE INSTANCIAS/PUNTEROS GENERALES ║
  ╚═══════════════════════════════════════════╝*/
DHT dht(pines.dht.PIN, pines.dht.MODEL);
OneWire oneWire(pines.esp32.TEMP);
DallasTemperature sensores(&oneWire);
WebServer server(red.PUERTO);
Preferences prefs;


/*##################################################################
  ### MEMORIA Y PUNTEROS PARA LA RED NEURONAL                    ###
  ##################################################################*/
tflite::ErrorReporter*    error_reporter = nullptr;
const tflite::Model*      model          = nullptr;
tflite::MicroInterpreter* interpreter    = nullptr;
TfLiteTensor* input  = nullptr;
TfLiteTensor* output = nullptr;

constexpr int kTensorArenaSize = 16 * 1024;
alignas(16) uint8_t tensor_arena[kTensorArenaSize];


/*##########################
  ### VARIABLES GLOBALES ###
  ##########################*/
String ipAddress = red.ESCUCHA;

float sen_humedadAire      = 0.0;
float sen_humedadTierra    = 0.0;
float sen_litrosAgua       = 0.0;
float sen_alturaPlanta     = 0.0;
float sen_temperatura      = 0.0;
float sen_temperaturaEsp32 = 0.0;
float sen_temperaturaAgua  = 0.0;
float sen_luz              = 0.0;

float uv    = 0.0;
float bomba = 0.0;

float raw_temperatura   = 0.0;
float raw_humedadAire   = 0.0;
float raw_humedadTierra = 0.0;
float raw_luz           = 0.0;
float raw_litrosAgua    = 0.0;

bool camaraOk = false;


/*══════════════════════════════════════════
  ║              CAMARA                   ║
  ╚══════════════════════════════════════*/
bool iniciarCamara() {
  camera_config_t cfg;

  cfg.pin_pwdn      = pines.cam.PWDN_GPIO_NUM;
  cfg.pin_reset     = pines.cam.RESET_GPIO_NUM;
  cfg.pin_xclk      = pines.cam.XCLK_GPIO_NUM;
  cfg.pin_sccb_sda  = pines.cam.SIOD_GPIO_NUM;
  cfg.pin_sccb_scl  = pines.cam.SIOC_GPIO_NUM;
  cfg.pin_d7        = pines.cam.Y9_GPIO_NUM;
  cfg.pin_d6        = pines.cam.Y8_GPIO_NUM;
  cfg.pin_d5        = pines.cam.Y7_GPIO_NUM;
  cfg.pin_d4        = pines.cam.Y6_GPIO_NUM;
  cfg.pin_d3        = pines.cam.Y5_GPIO_NUM;
  cfg.pin_d2        = pines.cam.Y4_GPIO_NUM;
  cfg.pin_d1        = pines.cam.Y3_GPIO_NUM;
  cfg.pin_d0        = pines.cam.Y2_GPIO_NUM;
  cfg.pin_vsync     = pines.cam.VSYNC_GPIO_NUM;
  cfg.pin_href      = pines.cam.HREF_GPIO_NUM;
  cfg.pin_pclk      = pines.cam.PCLK_GPIO_NUM;

  cfg.xclk_freq_hz  = 20000000;
  cfg.ledc_timer    = LEDC_TIMER_1;    // TIMER_0 reservado para actuadores
  cfg.ledc_channel  = LEDC_CHANNEL_3; // Canales 0-2 usados por bomba/UV

  cfg.pixel_format  = PIXFORMAT_JPEG;
  cfg.frame_size    = FRAMESIZE_VGA;  // 640x480; cambia a QVGA si hay poca RAM
  cfg.jpeg_quality  = 12;
  cfg.fb_count      = 1;
  cfg.fb_location   = CAMERA_FB_IN_DRAM;
  cfg.grab_mode     = CAMERA_GRAB_LATEST;

  esp_err_t err = esp_camera_init(&cfg);
  if (err != ESP_OK) {
    Serial.printf("[CAM] Error al inicializar: 0x%x\n", err);
    return false;
  }

  Serial.println("[CAM] OV2640 lista");
  sensor_t* s = esp_camera_sensor_get();
  if (s) Serial.printf("[CAM] Sensor PID: 0x%02X (OV2640=0x26)\n", s->id.PID);
  return true;
}


/*══════════════════════════════════════════
  ║           SETUP                       ║
  ╚══════════════════════════════════════*/
void setup() {
  Serial.begin(115200);
  delay(1000);

  /* SENSORES */
  dht.begin();
  sensores.begin();
  pinMode(pines.ultrasonido.TRIG, OUTPUT);
  pinMode(pines.ultrasonido.ECHO, INPUT);

  // UV-A
  ledcSetup(pines.ultravioleta.A.CAN, pines.ultravioleta.FRQ, pines.ultravioleta.RES);
  ledcAttachPin(pines.ultravioleta.A.PIN, pines.ultravioleta.A.CAN);
  ledcWrite(pines.ultravioleta.A.CAN, pines.ultravioleta.OFF);
  // UV-B
  ledcSetup(pines.ultravioleta.B.CAN, pines.ultravioleta.FRQ, pines.ultravioleta.RES);
  ledcAttachPin(pines.ultravioleta.B.PIN, pines.ultravioleta.B.CAN);
  ledcWrite(pines.ultravioleta.B.CAN, pines.ultravioleta.OFF);
  // Bomba
  ledcSetup(pines.bomba.CAN, pines.bomba.FRQ, pines.bomba.RES);
  ledcAttachPin(pines.bomba.PIN, pines.bomba.CAN);
  ledcWrite(pines.bomba.CAN, pines.bomba.OFF);

  /* CAMARA — inicializar antes del WiFi para evitar conflictos de heap */
  camaraOk = iniciarCamara();

  /* RED NEURONAL */
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  model = tflite::GetModel(aigis);

  static tflite::AllOpsResolver resolver;
  static tflite::MicroInterpreter static_interpreter(
    model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  interpreter->AllocateTensors();
  input  = interpreter->input(0);
  output = interpreter->output(0);

  /* SERVIDOR WEB en núcleo 0 */
  xTaskCreatePinnedToCore(ServidorWeb, "WebSrv", 10000, NULL, 1, NULL, 0);
}


/*══════════════════════════════════════════
  ║           LOOP PRINCIPAL              ║
  ╚══════════════════════════════════════*/
void loop() {
  input  = interpreter->input(0);
  output = interpreter->output(0);

  actualizarSensores();
  formatearSensores();

  input->data.f[0] = raw_temperatura;
  input->data.f[1] = raw_humedadAire;
  input->data.f[2] = raw_humedadTierra;
  input->data.f[3] = raw_luz;
  input->data.f[4] = raw_litrosAgua;

  if (interpreter->Invoke() == kTfLiteOk) {
    neopixelWrite(48, random(0, 255), random(0, 255), random(0, 255));
    bomba = output->data.f[0];
    uv    = output->data.f[1];
  } else {
    Serial.println("❌ ERROR CRÍTICO: La IA falló al procesar los datos.");
  }

  log(true);
  delay(1000);
}


/*══════════════════════════════════════════
  ║              SENSORES                 ║
  ╚══════════════════════════════════════*/
float leerUltrasonido() {
  digitalWrite(pines.ultrasonido.TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(pines.ultrasonido.TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(pines.ultrasonido.TRIG, LOW);
  long duracion = pulseIn(pines.ultrasonido.ECHO, HIGH, 30000);
  float distancia = duracion / 58.2;
  if (distancia <= 0 || distancia > 400) return 0;
  return distancia;
}

void actualizarSensores() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!std::isnan(h)) sen_humedadAire = h;
  if (!std::isnan(t)) sen_temperatura = t;

  sensores.requestTemperatures();
  sen_temperaturaEsp32 = sensores.getTempCByIndex(0);

  sen_humedadTierra = constrain(
    map(analogRead(pines.agua.TIERRA), calibracion.tierra.SECO, calibracion.tierra.MOJA, 0, 100),
    0, 100);

  sen_litrosAgua = constrain(
    map(analogRead(pines.agua.TANQUE), calibracion.tanque.SECO, calibracion.tanque.MOJA, 0, 100),
    0, 100);

  sen_alturaPlanta = leerUltrasonido();
  sen_luz = (float)(analogRead(pines.fotoresistencia.PIN));
}

void formatearSensores() {
  raw_temperatura   = sen_temperatura   / 50.0;
  raw_humedadAire   = sen_humedadAire   / 100.0;
  raw_humedadTierra = sen_humedadTierra / 100.0;
  raw_luz           = sen_luz           / 4095.0;
  raw_litrosAgua    = sen_litrosAgua    / 100.0;
}

String validarDato(float valor) {
  if (isnan(valor)) return "0.0";
  return String(valor, 1);
}


/*══════════════════════════════════════════
  ║               LOG SERIAL              ║
  ╚══════════════════════════════════════*/
void log(bool activo) {
  if (!activo) return;
  Serial.println("#[SKELL-LOG]> Sensores");
  Serial.printf( " - HUM. TIERRA:    %.1f%%\n", sen_humedadTierra);
  Serial.printf( " - HUM. AIRE:      %.1f%%\n", sen_humedadAire);
  Serial.printf( " - TEMP. AMBIENTE: %.1fC\n",  sen_temperatura);
  Serial.printf( " - TEMP. TANQUE:   %.1fC\n",  sen_temperaturaAgua);
  Serial.printf( " - NIVEL TANQUE:   %.1f%%\n", sen_litrosAgua);
  Serial.printf( " - ALTURA PLANTA:  %.1f cm\n",sen_alturaPlanta);
  Serial.printf( " - LUZ:            %.1f\n",   sen_luz);
  Serial.println("-------------------------");
  Serial.println("#[SKELL-LOG]> Conexiones");
  Serial.printf( " - Datos:  http://%s/data\n", ipAddress.c_str());
  Serial.printf( " - Camara: http://%s/cam\n",  ipAddress.c_str());
  Serial.printf( " - Cam OK: %s\n", camaraOk ? "SI" : "NO");
  Serial.println("-------------------------");
  Serial.println("#[SKELL-LOG]> IA");
  Serial.printf( " - UV:    %.4f\n", uv);
  Serial.printf( " - Bomba: %.4f\n", bomba);
  Serial.println("-------------------------");
  Serial.println("#[SKELL-LOG]> ESP32");
  Serial.printf( " - TEMP:  %.1fC\n", sen_temperaturaEsp32);
  Serial.printf( " - SRAM:  %.0f / %.0f KB\n",
    ESP.getFreeHeap() / 1024.0, ESP.getHeapSize() / 1024.0);
  if (psramFound()) {
    Serial.printf(" - PSRAM: %.0f / %.0f KB\n",
      ESP.getFreePsram() / 1024.0, ESP.getPsramSize() / 1024.0);
  } else {
    Serial.println(" - PSRAM: NO DETECTADA");
  }
  Serial.println("");
}


/*══════════════════════════════════════════
  ║   SERVIDOR WEB ÚNICO — Núcleo 0       ║
  ║                                       ║
  ║  GET /data  → JSON con sensores       ║
  ║  GET /cam   → foto JPEG de la planta  ║
  ╚══════════════════════════════════════*/
void ServidorWeb(void* p) {
  WiFi.begin(red.NOMBRE, red.CLAVE);
  while (WiFi.status() != WL_CONNECTED) vTaskDelay(pdMS_TO_TICKS(500));
  ipAddress = WiFi.localIP().toString();

  /* ── GET /data ── */
  server.on("/data", []() {
    String json;
    json.reserve(220);
    json  = "{";
    json += "\"humaire\":"   + validarDato(sen_humedadAire)      + ",";
    json += "\"tempambi\":"  + validarDato(sen_temperatura)      + ",";
    json += "\"tempesp\":"   + validarDato(sen_temperaturaEsp32) + ",";
    json += "\"tempagua\":"  + validarDato(sen_temperaturaAgua)  + ",";
    json += "\"humtierra\":" + validarDato(sen_humedadTierra)    + ",";
    json += "\"litagua\":"   + validarDato(sen_litrosAgua)       + ",";
    json += "\"luz\":"       + validarDato(sen_luz)              + ",";
    json += "\"alturpl\":"   + validarDato(sen_alturaPlanta)     + ",";
    json += "\"uv\":"        + validarDato(uv)                   + ",";
    json += "\"bomba\":"     + validarDato(bomba)                + ",";
    json += "\"camOk\":"     + String(camaraOk ? "true" : "false");
    json += "}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
  });

  /* ── GET /cam ── foto JPEG estática */
  server.on("/cam", []() {
    if (!camaraOk) {
      server.send(503, "application/json", "{\"error\":\"camara no disponible\"}");
      return;
    }
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      server.send(500, "application/json", "{\"error\":\"fallo al capturar foto\"}");
      return;
    }
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Content-Disposition", "inline; filename=planta.jpg");
    server.send_P(200, "image/jpeg", (const char*)fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });

  server.begin();

  for (;;) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
