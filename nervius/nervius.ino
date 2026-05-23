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
#include "config.h"  // Configuración de Pines en General


/*╔═══════════════════════╗
  ║ [ GLOBAL ] Instancias ║
  ╚═══════════════════════╝*/
HardwareSerial core(2);
DHT dht(DHT_PIN, DHT_MODEL);


/*╔══════════════════════════╗
  ║ [ GLOBAL ] LED INTEGRADO ║
  ╚══════════════════════════╝*/

/*╔═══════════════════════════════════════════╗
  ║ [ GLOBAL ] Variables Globales/Temporales  ║
  ╚═══════════════════════════════════════════╝*/
float temperatura_ambiente = 0;
float temperatura_agua     = 22.0;
float humedad_ambiente     = 0;
float humedad_suelo        = 45.2;
int   intensidad_luz       = 1024;
int   ultrasonido          = 150;
// Actuadores
uint8_t uv          = 0;
bool    riego       = false;
uint8_t ventilacion = 0;

unsigned long tiempoAnterior = 0;
const long intervalo = INTERVALO * 1000;


void setup() {
  Serial.begin(BAUDIOS);

  core.begin(BAUDIOS, SERIAL_8N1, RX_PIN, TX_PIN);
  core.setTimeout(100);

  pinMode(LED_PIN, OUTPUT); // Led Azul Integrado
  pinMode(FC28_PIN, INPUT); // FC-28
  pinMode(FTRT_PIN, INPUT); // FOTORESISTENCIA

  dht.begin();

  delay(1000);
  Serial.println("[OK] Nodo sensores listo.");
}


void loop() {
  unsigned long tiempoActual = millis();

  // ── Envío periódico de sensores al S3 ──────────────────────────────────
  if (tiempoActual - tiempoAnterior >= intervalo) {
    digitalWrite(LED_PIN, HIGH);
    tiempoAnterior = tiempoActual;

    // LECTURA DEL DHT22
    float h = dht.readHumidity(); float t = dht.readTemperature();
    if (!std::isnan(h)) humedad_ambiente     = h;
    if (!std::isnan(t)) temperatura_ambiente = t;

    // LECTURA DEL FC-28
    int valorCrudo = analogRead(FC28_PIN);
    int porcentaje = map(valorCrudo, FC28_LOW, FC28_HIGH, 0, 100);
    humedad_suelo  = constrain(porcentaje, 0, 100);
    
    // LECTURA DE LA FOTORESISTENCIA
    valorCrudo = analogRead(FTRT_PIN);
    Serial.println(valorCrudo);
    porcentaje = map(valorCrudo, FTRT_LOW, FTRT_HIGH, 0, 100);
    intensidad_luz  = constrain(porcentaje, 0, 100);
    
    temperatura_agua     = random(16, 30);
    ultrasonido          = random(5, 20);

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

    // [FIX 2] Pequeña pausa tras enviar para dar ventana al S3 de responder
    // antes de que el loop vuelva a ejecutarse y potencialmente transmitir
    // de nuevo. Sin este delay, TX y RX pueden solaparse en el bus UART.
    delay(300);
  }

  // ── Recepción de comandos del S3 ───────────────────────────────────────
  if (core.available()) {
    digitalWrite(LED_PIN, LOW);
    String comandos = core.readStringUntil('\n');

    JsonDocument actuadores;
    DeserializationError error = deserializeJson(actuadores, comandos);

    if (!error) {
      uv          = actuadores[0]; // Índice 0: Estado de luz UV
      riego       = actuadores[1]; // Índice 1: Estado de Bomba de Riego
      ventilacion = actuadores[2]; // Índice 2: Estado de Ventilación

      Serial.print("[RX] Órdenes recibidas del S3: ");
      Serial.println(comandos);

      //
      //  EJECUCIÓN DE ACCIONES MEDIANTE ACTUADORES
      //

    } else {
      Serial.print("[ERROR] JSON del S3 corrupto o incompleto: ");
      Serial.println(error.c_str());
    }
  }
}