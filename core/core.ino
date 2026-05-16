/*╔═════════════════════════════════╗
  ║    SKELL'S GREENHOUSE V3.0      ║
  ╠═════════════════════════════════╣
  ║          Developed By:          ║
  ║ - Robert Rodríguez "Skellent"   ║
  ║    - Christopher Ramirez        ║
  ║     - Fabiana Hernandez         ║
  ╚═════════════════════════════════╝*/


/*╔══════════════════════════════════╗
  ║ [ GLOBAL ] Librerías Importadas  ║
  ╚══════════════════════════════════╝*/
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "esp_camera.h"


/*╔═════════════════════════════╗
  ║ [ GLOBAL ] Módulos Propios  ║
  ╚═════════════════════════════╝*/
#include "config.h"  // Configuracion de Pines en General
#include "sensors.h" // Memoria Reservada en la RAM para la lectura de Sensores y de IA
#include "cam.h"     // Script de inicialización de Cámara Integrada


/*╔═══════════════════════════════════╗
  ║ [ GLOBAL ] Instancias Requeridas  ║
  ╚═══════════════════════════════════╝*/
Sensores          sensores;       // Reescribir Valor de Sensores
SemaphoreHandle_t mutexDatos;     // Para evitar conflictos entre núcleos
WebServer         servidor(80);   // Servidor Web
HardwareSerial    nervios(1); // Comunicación Serial con ESP32 Comúm


/*╔═══════════════════════════════════════════╗
  ║ [ GLOBAL ] Variables Globales/Temporales  ║
  ╚═══════════════════════════════════════════╝*/
bool    camStatus   = false;
uint8_t intentos    = 0;
uint8_t perfil      = 0;
uint8_t uv          = 0;
uint8_t riego       = 0;
uint8_t ventilacion = 0;


// Constructor del Servidor Web para Evitar Problemas de Compilación y Llamado de Función
void servidorWeb(void *pvParameters);


/*╔═══════════════════════════════════════╗
  ║ [ GLOBAL ] Configuraciones Iniciales  ║
  ╚═══════════════════════════════════════╝*/
void setup() {
  neopixelWrite(PIN_RGB, 255, 255, 0); // AMARILLO

  /* Comunicación Serial */
  Serial.begin(BAUDIOS);
  nervios.begin(BAUDIOS, SERIAL_8N1, PIN_RX, PIN_TX);

  mutexDatos = xSemaphoreCreateMutex();
  camStatus = initCamara();
  WiFi.begin(WIFI_RED, WIFI_PSW);

  while (WiFi.status() != WL_CONNECTED && intentos < 20) { delay(500); intentos++; }

  if (WiFi.status() == WL_CONNECTED) { Serial.println(WiFi.localIP()); } else { Serial.println("\nNo se pudo conectar a la red. Trabajando en modo local fuera de línea."); }

  xTaskCreatePinnedToCore(
    servidorWeb,   // Función de la tarea
    "ServidorWeb", // Nombre identificador
    8192,          // Tamaño de pila (RAM asignada)
    NULL,          // Parámetros de entrada
    1,             // Prioridad de la tarea
    NULL,          // Handle
    0              // <--- NÚCLEO 0 (Dedicado a WiFi)
  );
}


/*╔═══════════════════════════════╗
  ║ [ NÚCLEO 1 ] Búcle Principal  ║
  ╚═══════════════════════════════╝*/
void loop() {
  neopixelWrite(PIN_RGB, 255, 255, 255); // BLANCO

  

  if (nervios.available()) {
    neopixelWrite(PIN_RGB, 0, 0, 255); // AZUL TOTAL
    String datosEntrantes = nervios.readStringUntil('\n'); 
    StaticJsonDocument<256> datos;
    DeserializationError error = deserializeJson(datos, datosEntrantes);

    if (!error) {
      if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(100)) == pdTRUE) {

        sensores.sen_temperatura_ambiente = datos[0];
        sensores.sen_temperatura_agua     = datos[1];
        sensores.sen_humedad_ambiente     = datos[2];
        sensores.sen_humedad_suelo        = datos[3];
        sensores.sen_intensidad_luz       = datos[4];
        sensores.sen_ultrasonido          = datos[5];
        
        neopixelWrite(PIN_RGB, 0, 255, 0); // VERDE
        sensores.ia_perfilplanta = detectarPerfil();

        /*
        Justo en este espacio va la parte de interpretación de la IA

        // INPUT
        sensores.sen_temperatura_ambiente;
        sensores.sen_temperatura_agua;
        sensores.sen_humedad_ambiente;
        sensores.sen_humedad_suelo;
        sensores.sen_intensidad_luz;
        sensores.sen_ultrasonido;
        
        // OUTPUT
        sensores.ia_ventilacion    = 0;
        sensores.ia_intensidad_uv  = 0;
        sensores.ia_riego          = 0;
        sensores.esp32_temperatura = 0;
        sensores.esp32_camStatus   = camStatus;

        ejecutarActuadores();

        */
        xSemaphoreGive(mutexDatos);
      }
    } else { Serial.println("Error al parsear el JSON entrante por UART."); }
  }

  vTaskDelay(pdMS_TO_TICKS(100)); // Delay amigable con FreeRTOS
}


/*╔════════════════════════════════════════════╗
  ║ [ NÚCLEO 1 ] Enviar Comandos de Ejecución  ║
  ╚════════════════════════════════════════════╝*/
void ejecutarActuadores(int uv, int riego, int ventilacion) {
  JsonDocument docOut;
  docOut["uv"]          = uv;
  docOut["riego"]       = riego;
  docOut["ventilacion"] = ventilacion;
  serializeJson(docOut, Serial1); 
  Serial1.println(); 
}


/*╔═════════════════════════════════════════╗
  ║ [ NÚCLEO 1 ] Detector de Maceta/Perfil  ║
  ╚═════════════════════════════════════════╝*/
uint8_t detectarPerfil() {
  return 1;
}


/*╔════════════════════════════╗
  ║ [ NÚCLEO 0 ] Servidor Web  ║
  ╚════════════════════════════╝*/
void servidorWeb(void *pvParameters) {
  /*╔══════════════════╗
    ║  Datos Globales  ║
    ╚══════════════════╝*/
  servidor.on("/api/datos", HTTP_GET, []() {
    JsonDocument informacion; // Creacion del JSON vacio
    if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(50)) == pdTRUE) {
      // Sensores
      informacion["sensores"]["temperatura_ambiente"] = sensores.sen_temperatura_ambiente;
      informacion["sensores"]["temperatura_agua"]     = sensores.sen_temperatura_agua;
      informacion["sensores"]["humedad_ambiente"]     = sensores.sen_humedad_ambiente;
      informacion["sensores"]["humedad_suelo"]        = sensores.sen_humedad_suelo;
      informacion["sensores"]["intensidad_luz"]       = sensores.sen_intensidad_luz;
      informacion["sensores"]["ultrasonido"]          = sensores.sen_ultrasonido;

      // Informacion del ESP32
      informacion["esp32"]["temperatura"]      = sensores.esp32_temperatura;
      informacion["esp32"]["camStatus"]        = sensores.esp32_camStatus;

      // Informacion de la IA
      informacion["ia"]["intensidad_uv"] = sensores.ia_intensidad_uv;
      informacion["ia"]["riego"]         = sensores.ia_riego;
      informacion["ia"]["ventilacion"]   = sensores.ia_ventilacion;
      informacion["ia"]["perfil"]        = sensores.ia_perfilplanta;

      xSemaphoreGive(mutexDatos);

      String respuestaJSON;
      serializeJson(informacion, respuestaJSON);
      servidor.send(200, "application/json", respuestaJSON);
    } else { servidor.send(503, "application/json", "{\"error\":\"Inferencia en progreso\"}"); }
  });


  /*╔══════════════╗
    ║  Fotografía  ║
    ╚══════════════╝*/
  servidor.on("/api/foto", HTTP_GET, []() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) { servidor.send(500, "text/plain", "Fallo al capturar imagen"); return; }
    servidor.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb); 
  });


  // Inicia el Servidor SOLAMENTE cuando está conectado
  if (WiFi.status() == WL_CONNECTED) { servidor.begin(); }


  // Iteraciones infinitas con intervalos de descanso para no saturar al núcleo
  for (;;) { if (WiFi.status() == WL_CONNECTED) { servidor.handleClient(); } vTaskDelay(pdMS_TO_TICKS(5)); }
}