/*╔═══════════════════════════════════════════════════════╗
  ║  SKELL'S GREENHOUSE V3.0 - NODO SENSORES (COMÚN)    ║
  ╚═══════════════════════════════════════════════════════╝*/


/*╔══════════════════════════════════╗
  ║ [ GLOBAL ] Librerías Importadas  ║
  ╚══════════════════════════════════╝*/
#include <ArduinoJson.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>


/*╔═════════════════════════════╗
  ║ [ GLOBAL ] Módulos Propios  ║
  ╚═════════════════════════════╝*/
#include "config.h"


/*╔═══════════════════════╗
  ║ [ GLOBAL ] Instancias ║
  ╚═══════════════════════╝*/
HardwareSerial core(2);
DHT            dht(DHT_PIN, DHT_MODEL);
OneWire        oneWire(DS18_PIN);
DallasTemperature ds18(&oneWire);


/*╔═══════════════════════════════════════════╗
  ║ [ GLOBAL ] Variables Globales/Temporales  ║
  ╚═══════════════════════════════════════════╝*/
float temperatura_ambiente = 0;
float temperatura_agua_a   = 22.0;
float temperatura_agua_b   = 22.0;
float humedad_ambiente     = 0;
float humedad_suelo        = 45.2;
int   intensidad_luz       = 0;
int   nivel_tanque         = 0;   // porcentaje (0–100 %)
int   altura_planta        = 0;   // distancia HC-SR04 en cm

static String bufferCore = "";

// Actuadores (valores recibidos desde el S3 / Alicia)
uint8_t uv          = 0;
bool    riego       = false;
uint8_t ventilacion = 0;

unsigned long tiempoAnterior = 0;
const long    intervalo      = INTERVALO * 1000;


void setup() {
  Serial.begin(BAUDIOS);

  core.begin(BAUDIOS, SERIAL_8N1, RX_PIN, TX_PIN);
  core.setTimeout(100);

  /* PINES DE SENSORES */
  pinMode(FC28_PIN, INPUT);
  pinMode(FTRT_PIN, INPUT);
  pinMode(NIVEL_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  /* PINES DE ACTUADORES */
  // LUZ UV
  ledcSetup(UV_LEDC_CANAL, UV_LEDC_FRECUENCIA, UV_LEDC_RESOLUCION);
  ledcAttachPin(UV_PIN, UV_LEDC_CANAL);
  ledcWrite(UV_LEDC_CANAL, 0);

  // BOMBA DE RIEGO
  pinMode(RIEGO_PIN, OUTPUT);
  digitalWrite(RIEGO_PIN, LOW);

  // CANDELABRO
  ledcSetup(CANDELABRO_CANAL, CANDELABRO_FRECUENCIA, CANDELABRO_RESOLUCION);
  ledcAttachPin(CANDELABRO_PIN, CANDELABRO_CANAL);
  ledcWrite(CANDELABRO_CANAL, 255);

  /* DHT22 */
  dht.begin();

  /* DS18 */
  ds18.begin();
  ds18.setResolution(11);
  ds18.setWaitForConversion(false);
  ds18.requestTemperatures();

  delay(400);
}


void loop() {
  unsigned long tiempoActual = millis();

  // ── Envío periódico de sensores al S3 ──────────────────────────────────
  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;

    float t0 = ds18.getTempCByIndex(0);
    float t1 = ds18.getTempCByIndex(1);
    if (t0 != DEVICE_DISCONNECTED_C && t0 > -50.0f) temperatura_agua_a = t0;
    if (t1 != DEVICE_DISCONNECTED_C && t1 > -50.0f) temperatura_agua_b = t1;
    ds18.requestTemperatures();

    // LECTURA DEL DHT22 (temperatura y humedad ambiente)
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!std::isnan(h)) humedad_ambiente     = h;
    if (!std::isnan(t)) temperatura_ambiente = t;

    // LECTURA DEL FC-28 (humedad del suelo)
    int valorCrudo = analogRead(FC28_PIN);
    int porcentaje = map(valorCrudo, FC28_LOW, FC28_HIGH, 0, 100);
    humedad_suelo  = (float)constrain(porcentaje, 0, 100);

    // LECTURA DE LA FOTORESISTENCIA (intensidad de luz exterior)
    valorCrudo    = analogRead(FTRT_PIN);
    porcentaje    = map(valorCrudo, FTRT_LOW, FTRT_HIGH, 0, 100);
    intensidad_luz = constrain(porcentaje, 0, 100);

    // LECTURA DEL SENSOR DE NIVEL (analógico resistivo)
    // más agua sumergida → mayor tensión → mayor valor ADC
    int nivelCrudo = analogRead(NIVEL_PIN);
    nivel_tanque   = constrain(map(nivelCrudo, NIVEL_LOW, NIVEL_HIGH, 0, 100), 0, 100);

    // LECTURA DEL ULTRASONIDO (altura de la planta en cm)
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long  duracion     = pulseIn(ECHO_PIN, HIGH, 30000);
    float distancia    = duracion * 0.0343f / 2.0f;
    altura_planta      = (int)distancia;

    /*╔════════════════════════════════════╗
      ║  Protocolo UART → Core (ESP32-S3)  ║
      ╠════════════════════════════════════╣
      ║  JSON Array de 8 elementos:        ║
      ║  [0] temp_ambiente  (DHT22,  °C)   ║
      ║  [1] temp_agua_a    (DS18[0], °C)  ║
      ║  [2] hum_ambiente   (DHT22,  %)    ║
      ║  [3] hum_suelo      (FC-28,  %)    ║
      ║  [4] intensidad_luz (FTRT,   %)    ║
      ║  [5] nivel_tanque   (analógico, %) ║
      ║  [6] temp_agua_b    (DS18[1], °C)  ║
      ║  [7] altura_planta  (HC-SR04, cm)  ║
      ╚════════════════════════════════════╝*/
    JsonDocument doc;
    JsonArray    payload = doc.to<JsonArray>();
    payload.add(temperatura_ambiente);   // [0]
    payload.add(temperatura_agua_a);     // [1]
    payload.add(humedad_ambiente);       // [2]
    payload.add(humedad_suelo);          // [3]
    payload.add(intensidad_luz);         // [4]
    payload.add(nivel_tanque);           // [5]
    payload.add(temperatura_agua_b);     // [6]
    payload.add(altura_planta);          // [7]

    serializeJson(payload, core);
    core.println();

    serializeJson(payload, Serial);
    Serial.println();
  }

  // ── Recepción de comandos del Core (salidas de Alicia) ─────────────────
  while (core.available()) {
    char c = core.read();
    if (c == '\n') {
      if (bufferCore.length() > 0) {
        JsonDocument actuadores;
        DeserializationError error = deserializeJson(actuadores, bufferCore);
        bufferCore = "";

        if (!error) {
          uv          = (uint8_t)constrain((int)actuadores[0], 0, 255);
          riego       = (bool)actuadores[1];
          ventilacion = (uint8_t)actuadores[2];

          Serial.printf("[ALICIA] UV=%d  Riego=%s\n", uv, riego ? "ON" : "OFF");

          ledcWrite(UV_LEDC_CANAL, uv);
          digitalWrite(RIEGO_PIN, riego ? HIGH : LOW);
        }
      }
    } else {
      bufferCore += c;
      if (bufferCore.length() > 128) bufferCore = "";
    }
  }
}
