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
float temperatura_agua_a   = 22.0;   // DS18[0] — sensor A (tanque principal)
float temperatura_agua_b   = 22.0;   // DS18[1] — sensor B (punto secundario)
float humedad_ambiente     = 0;
float humedad_suelo        = 45.2;
int   intensidad_luz       = 0;
int   ultrasonido          = 150;

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
  pinMode(LED_PIN,  OUTPUT);
  pinMode(FC28_PIN, INPUT);
  pinMode(FTRT_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  /* PINES DE ACTUADORES */
  ledcSetup(UV_LEDC_CANAL, UV_LEDC_FRECUENCIA, UV_LEDC_RESOLUCION);
  ledcAttachPin(UV_PIN, UV_LEDC_CANAL);
  ledcWrite(UV_LEDC_CANAL, 0);

  pinMode(RIEGO_PIN, OUTPUT);
  digitalWrite(RIEGO_PIN, LOW);

  /* DHT22 */
  dht.begin();

  /*╔══════════════════════════════════════════════════╗
    ║  DS18B20 — Inicialización                        ║
    ╠══════════════════════════════════════════════════╣
    ║  Resolución 11-bit → conversión ~375 ms, ±0.125°C║
    ║  setWaitForConversion(false) = modo no-bloqueante ║
    ║  El ciclo de 1 s sirve como tiempo de conversión  ║
    ╚══════════════════════════════════════════════════╝*/
  ds18.begin();
  ds18.setResolution(11);
  ds18.setWaitForConversion(false);

  // Primera solicitud en setup → delay para cebar la lectura inicial
  ds18.requestTemperatures();
  delay(400);

  int encontrados = ds18.getDeviceCount();
  Serial.printf("[DS18] Sensores encontrados: %d en pin %d\n", encontrados, DS18_PIN);
  if (encontrados < 2) {
    Serial.println("[DS18] ADVERTENCIA: se esperaban 2 sensores.");
  }

  delay(200);
  Serial.println("[OK] Nodo sensores + actuadores listo. Intervalo: " + String(INTERVALO) + " s.");
}


void loop() {
  unsigned long tiempoActual = millis();

  // ── Envío periódico de sensores al S3 ──────────────────────────────────
  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;
    digitalWrite(LED_PIN, HIGH);

    /*╔══════════════════════════════════════════════════════╗
      ║  DS18B20 — Lectura no-bloqueante                     ║
      ╠══════════════════════════════════════════════════════╣
      ║  1. Se leen los valores de la conversión ANTERIOR    ║
      ║     (tiene ≥ 1 s desde que se solicitó → lista).    ║
      ║  2. Se solicita la conversión para el PRÓXIMO ciclo. ║
      ╚══════════════════════════════════════════════════════╝*/
    float t0 = ds18.getTempCByIndex(0);
    float t1 = ds18.getTempCByIndex(1);
    if (t0 != DEVICE_DISCONNECTED_C && t0 > -50.0f) temperatura_agua_a = t0;
    if (t1 != DEVICE_DISCONNECTED_C && t1 > -50.0f) temperatura_agua_b = t1;
    ds18.requestTemperatures();   // solicitar conversión para el siguiente ciclo

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

    // LECTURA DEL ULTRASONIDO (nivel de agua / altura de planta)
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long  duracion  = pulseIn(ECHO_PIN, HIGH, 30000);
    float distancia = duracion * 0.0343f / 2.0f;
    ultrasonido     = (int)distancia;

    /*╔══════════════════════════════════════════════════════╗
      ║  Protocolo UART → Core (ESP32-S3)                    ║
      ╠══════════════════════════════════════════════════════╣
      ║  JSON Array de 7 elementos:                          ║
      ║  [0] temp_ambiente  (DHT22,  °C)                    ║
      ║  [1] temp_agua_a    (DS18[0], °C) — sensor A        ║
      ║  [2] hum_ambiente   (DHT22,  %)                     ║
      ║  [3] hum_suelo      (FC-28,  %)                     ║
      ║  [4] intensidad_luz (FTRT,   %)                     ║
      ║  [5] ultrasonido    (HC-SR04, cm)                   ║
      ║  [6] temp_agua_b    (DS18[1], °C) — sensor B        ║
      ╚══════════════════════════════════════════════════════╝*/
    JsonDocument doc;
    JsonArray    payload = doc.to<JsonArray>();
    payload.add(temperatura_ambiente);   // [0]
    payload.add(temperatura_agua_a);     // [1] DS18[0]
    payload.add(humedad_ambiente);       // [2]
    payload.add(humedad_suelo);          // [3]
    payload.add(intensidad_luz);         // [4]
    payload.add(ultrasonido);            // [5]
    payload.add(temperatura_agua_b);     // [6] DS18[1]

    serializeJson(payload, core);
    core.println();

    serializeJson(payload, Serial);
    Serial.println();

    digitalWrite(LED_PIN, LOW);
  }

  // ── Recepción de comandos del Core (salidas de Alicia) ─────────────────
  if (core.available()) {
    String comandos = core.readStringUntil('\n');

    JsonDocument actuadores;
    DeserializationError error = deserializeJson(actuadores, comandos);

    if (!error) {
      uv          = (uint8_t)constrain((int)actuadores[0], 0, 255);
      riego       = (bool)actuadores[1];
      ventilacion = (uint8_t)actuadores[2];

      Serial.printf("[ALICIA] UV=%d  Riego=%s\n", uv, riego ? "ON" : "OFF");

      // Luz UV — intensidad continua vía PWM (0-255)
      ledcWrite(UV_LEDC_CANAL, uv);

      // Bomba de riego — on/off mediante relé
      digitalWrite(RIEGO_PIN, riego ? HIGH : LOW);

    } else {
      Serial.print("[ERROR] JSON del Core corrupto: ");
      Serial.println(error.c_str());
    }
  }
}
