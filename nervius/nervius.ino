/*╔═══════════════════════════════════════════════════╗
  ║  SKELL'S GREENHOUSE V3.0 - NODO SENSORES (COMÚN)  ║
  ╚═══════════════════════════════════════════════════╝*/


/*╔══════════════════════════════════╗
  ║ [ GLOBAL ] Librerías Importadas  ║
  ╚══════════════════════════════════╝*/
#include <ArduinoJson.h>


/*╔═════════════════════════════╗
  ║ [ GLOBAL ] Módulos Propios  ║
  ╚═════════════════════════════╝*/
#include "config.h"  // Configuración de Pines en General


/*╔═══════════════════════╗
  ║ [ GLOBAL ] Instancias ║
  ╚═══════════════════════╝*/
HardwareSerial core(2);


/*╔══════════════════════════╗
  ║ [ GLOBAL ] LED INTEGRADO ║
  ╚══════════════════════════╝*/

/*╔═══════════════════════════════════════════╗
  ║ [ GLOBAL ] Variables Globales/Temporales  ║
  ╚═══════════════════════════════════════════╝*/
float temperatura_ambiente = 24.5;
float temperatura_agua     = 22.0;
float humedad_ambiente     = 60.5;
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

  // [FIX 1] Reducir timeout de readStringUntil() a 100ms.
  // El valor por defecto es 1000ms: si el S3 tarda en responder,
  // el loop queda bloqueado un segundo completo en cada iteración.
  core.setTimeout(100);

  pinMode(LED_PIN, OUTPUT);
  delay(1000);
  Serial.println("[OK] Nodo sensores listo.");
}


void loop() {
  unsigned long tiempoActual = millis();

  // ── Envío periódico de sensores al S3 ──────────────────────────────────
  if (tiempoActual - tiempoAnterior >= intervalo) {
    digitalWrite(LED_PIN, HIGH);
    tiempoAnterior = tiempoActual;

    /*
      LECTURA DE SENSORES AQUÍ
    */
    temperatura_ambiente = random(16, 30);
    temperatura_agua     = random(16, 30);
    humedad_ambiente     = random(65, 98);
    humedad_suelo        = random(20, 90);
    intensidad_luz       = random(0, 4028);
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