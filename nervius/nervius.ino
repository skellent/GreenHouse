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
#include "config.h"  // Configuracion de Pines en General


/*╔═══════════════════════╗
  ║ [ GLOBAL ] Instancias ║
  ╚═══════════════════════╝*/
HardwareSerial core(2);


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
const long intervalo = 2 * 1000;

void setup() {
  Serial.begin(BAUDIOS);
  core.begin(BAUDIOS, SERIAL_8N1, RX_PIN, TX_PIN);
  delay(1000);
}

void loop() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;

    /* 
      LECTURA DE SENSORES AQUÍ
     */

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
  }

  if (core.available()) {
    String comandos = core.readStringUntil('\n'); 
    
    JsonDocument actuadores;
    DeserializationError error = deserializeJson(actuadores, comandos);

    if (!error) {
      uv          = actuadores[0]; // Índice 0: Estado de luz UV
      riego       = actuadores[1]; // Índice 1: Estado de Bomba de Riego
      ventilacion = actuadores[2]; // Índice 2: Estado de Ventilación

      Serial.print("[RX] Órdenes en Array recibidas del S3: ");
      Serial.println(comandos);

      //
      //  EJECUCIÓN DE ACCIONES MEDIANTE ACTUADORES
      // 

    } else {
      Serial.print("[ERROR] JSON de S3 corrupto o incompleto: ");
      Serial.println(error.c_str());
    }
  }
}