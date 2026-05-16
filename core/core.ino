#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "esp_camera.h"


#include "config.h"
#include "sensors.h"
#include "cam.h"


Sensores          sensores;
SemaphoreHandle_t mutexDatos;
WebServer         server(80);


void serverAPI(void *pvParameters);


void setup() {
  Serial.begin(115200);


  mutexDatos = xSemaphoreCreateMutex();
  

  if (!initCamara()) {
    Serial.println("El sistema continuará sin funciones de video.");
  }


  Serial.printf("Conectando a la red: %s \n", WIFI_RED);
  WiFi.begin(WIFI_RED, WIFI_PSW);


  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }


  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n¡Conectado con éxito a la red local!");
    Serial.print("Dirección IP asignada por el router: ");
    Serial.println(WiFi.localIP()); // <--- Guarda esta IP para consultar tu API
  } else {
    Serial.println("\nNo se pudo conectar a la red. Trabajando en modo local fuera de línea.");
  }

  // 2. Lanzar la API en el NÚCLEO 0
  xTaskCreatePinnedToCore(
    serverAPI,         // Función de la tarea
    "ServidorWeb",    // Nombre identificador
    8192,             // Tamaño de pila (RAM asignada)
    NULL,             // Parámetros de entrada
    1,                // Prioridad de la tarea
    NULL,             // Handle
    0                 // <--- NÚCLEO 0 (PRO_CPU)
  );
}

void loop() {

  if (xSemaphoreTake(mutexDatos, portMAX_DELAY) == pdTRUE) {
    sensores.sen_temperatura_ambiente = 0;
    sensores.sen_temperatura_agua     = 0;
    sensores.sen_humedad_ambiente     = 0;
    sensores.sen_humedad_suelo        = 0;
    sensores.sen_intensidad_luz       = 0;
    sensores.sen_ultrasonido          = 0;
    sensores.ia_perfilplanta          = 0;
    sensores.ia_ventilacion           = 0;
    sensores.ia_intensidad_uv         = 0;
    sensores.ia_riego                 = 0;
    sensores.esp32_temperatura        = 0;
    sensores.esp32_camStatus          = true;
    xSemaphoreGive(mutexDatos);
  }
  
  vTaskDelay(pdMS_TO_TICKS(100)); // Delay amigable con FreeRTOS
}


void serverAPI(void *pvParameters) {
  
  server.on("/api/datos", HTTP_GET, []() {
    JsonDocument informacion;

    if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(50)) == pdTRUE) {
      informacion["sensores"]["temperatura_ambiente"] = sensores.sen_temperatura_ambiente;
      informacion["sensores"]["temperatura_agua"]     = sensores.sen_temperatura_agua;
      informacion["sensores"]["humedad_ambiente"]     = sensores.sen_humedad_ambiente;
      informacion["sensores"]["humedad_suelo"]        = sensores.sen_humedad_suelo;
      informacion["sensores"]["intensidad_luz"]       = sensores.sen_intensidad_luz;
      informacion["sensores"]["ultrasonido"]          = sensores.sen_ultrasonido;
      informacion["esp32"]["temperatura"]       = sensores.esp32_temperatura;
      informacion["esp32"]["camStatus"]         = sensores.esp32_camStatus;
      informacion["ia"]["intensidad_uv"] = sensores.ia_intensidad_uv;
      informacion["ia"]["riego"]         = sensores.ia_riego;
      informacion["ia"]["ventilacion"]   = sensores.ia_ventilacion;
      informacion["ia"]["perfil"]        = sensores.ia_perfilplanta;

      xSemaphoreGive(mutexDatos);

      String respuestaJSON;
      serializeJson(informacion, respuestaJSON);
      server.send(200, "application/json", respuestaJSON);
    } else {
      server.send(503, "application/json", "{\"error\":\"Inferencia en progreso\"}");
    }
  });

  server.on("/api/foto", HTTP_GET, []() {
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) { server.send(500, "text/plain", "Fallo al capturar imagen"); return; }
    server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb); 
  });

  if (WiFi.status() == WL_CONNECTED) {
    server.begin();
    Serial.println("Servidor API HTTP escuchando peticiones locales.");
  }

  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      server.handleClient();
    }
    vTaskDelay(pdMS_TO_TICKS(5)); // Mantiene el Watchdog del núcleo 0 en calma
  }
}