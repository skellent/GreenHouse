/*╔═══════════════════════════════════════════════════╗
  ║  SKELL'S GREENHOUSE V3.0 - NODO SENSORES (COMÚN)  ║
  ╚═══════════════════════════════════════════════════╝*/


/*╔══════════════════════════════════╗
  ║ [ GLOBAL ] Librerías Importadas  ║
  ╚══════════════════════════════════╝*/
#include <ArduinoJson.h>
#include <DHT.h>


/*╔═════════════════════════════╗
  ║ [ GLOBAL ] Módulos Propios  ║
  ╚═════════════════════════════╝*/
#include "config.h"


/*╔═══════════════════════╗
  ║ [ GLOBAL ] Instancias ║
  ╚═══════════════════════╝*/
HardwareSerial core(2);
DHT dht(DHT_PIN, DHT_MODEL);


/*╔═══════════════════════════════════════════╗
  ║ [ GLOBAL ] Variables Globales/Temporales  ║
  ╚═══════════════════════════════════════════╝*/
float temperatura_ambiente = 0;
float temperatura_agua     = 22.0;
float humedad_ambiente     = 0;
float humedad_suelo        = 45.2;
int   intensidad_luz       = 1024;
int   ultrasonido          = 150;

// Actuadores (valores recibidos desde el S3 / Alicia)
uint8_t uv          = 0;
bool    riego       = false;
uint8_t ventilacion = 0;

unsigned long tiempoAnterior = 0;
const long intervalo = INTERVALO * 1000;


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
  // UV: configurar canal LEDC para PWM de 8 bits a 5 kHz
  ledcSetup(UV_LEDC_CANAL, UV_LEDC_FRECUENCIA, UV_LEDC_RESOLUCION);
  ledcAttachPin(UV_PIN, UV_LEDC_CANAL);
  ledcWrite(UV_LEDC_CANAL, 0);          // LEDs apagados al inicio

  // Riego: pin digital hacia el relé
  pinMode(RIEGO_PIN, OUTPUT);
  digitalWrite(RIEGO_PIN, LOW);         // Bomba apagada al inicio

  dht.begin();

  delay(1000);
  Serial.println("[OK] Nodo sensores + actuadores listo.");
}


void loop() {
  unsigned long tiempoActual = millis();

  // ── Envío periódico de sensores al S3 ──────────────────────────────────
  if (tiempoActual - tiempoAnterior >= intervalo) {
    digitalWrite(LED_PIN, HIGH);
    tiempoAnterior = tiempoActual;

    // LECTURA DEL DHT22
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
    long duracion  = pulseIn(ECHO_PIN, HIGH, 30000);
    float distancia = duracion * 0.0343f / 2.0f;
    ultrasonido = (int)distancia;

    // Temperatura del agua: sustituir por sensor DS18B20 cuando esté disponible
    temperatura_agua = (float)random(16, 30);

    // Enviar JSON de sensores al S3
    JsonDocument doc;
    JsonArray sensores = doc.to<JsonArray>();
    sensores.add(temperatura_ambiente);
    sensores.add(temperatura_agua);
    sensores.add(humedad_ambiente);
    sensores.add(humedad_suelo);
    sensores.add(intensidad_luz);
    sensores.add(ultrasonido);

    serializeJson(sensores, core);
    core.println();

    serializeJson(sensores, Serial);
    Serial.println();

    delay(600);
  }

  // ── Recepción de comandos del S3 (salidas de Alicia) ──────────────────
  if (core.available()) {
    digitalWrite(LED_PIN, LOW);
    String comandos = core.readStringUntil('\n');

    JsonDocument actuadores;
    DeserializationError error = deserializeJson(actuadores, comandos);

    if (!error) {
      uv          = (uint8_t)constrain((int)actuadores[0], 0, 255);
      riego       = (bool)actuadores[1];
      ventilacion = (uint8_t)actuadores[2];

      Serial.printf("[ALICIA] UV=%d  Riego=%s\n", uv, riego ? "ON" : "OFF");

      /*╔════════════════════════════════════════╗
        ║  EJECUCIÓN DE ACTUADORES               ║
        ╠════════════════════════════════════════╣
        ║  UV:    PWM sobre LED UV vía LEDC      ║
        ║  Riego: Relé de bomba de agua          ║
        ╚════════════════════════════════════════╝*/

      // Luz UV — intensidad continua vía PWM (0-255)
      ledcWrite(UV_LEDC_CANAL, uv);

      // Bomba de riego — on/off mediante relé
      digitalWrite(RIEGO_PIN, riego ? HIGH : LOW);

    } else {
      Serial.print("[ERROR] JSON del S3 corrupto: ");
      Serial.println(error.c_str());
    }
  }
}
