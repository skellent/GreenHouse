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
#include <LiquidCrystal_I2C.h>


/*╔═════════════════════════════╗
  ║ [ GLOBAL ] Módulos Propios  ║
  ╚═════════════════════════════╝*/
#include "config.h"  // Configuracion de Pines en General
#include "sensors.h" // Memoria Reservada en la RAM para la lectura de Sensores y de IA
#include "cam.h"     // Script de inicialización de Cámara Integrada
#include "icons.h"   // Iconos para la pantalla LCD

/*╔═══════════════════════════════════╗
  ║ [ GLOBAL ] Instancias Requeridas  ║
  ╚═══════════════════════════════════╝*/
Sensores          sensores;       // Reescribir Valor de Sensores
SemaphoreHandle_t mutexDatos;     // Para evitar conflictos entre núcleos
WebServer         servidor(80);   // Servidor Web
HardwareSerial    nervios(1);     // Comunicación Serial con ESP32 Común
LiquidCrystal_I2C lcd(LCD_PROTOCOLO, 16, 2); // Dirección típica 0x27, pantalla 16x2


/*╔═══════════════════════════════════════════╗
  ║ [ GLOBAL ] Variables Globales/Temporales  ║
  ╚═══════════════════════════════════════════╝*/
bool    camStatus   = false;
uint8_t intentos    = 0;
uint8_t perfil      = 0;
uint8_t uv          = 0;
bool    riego       = false;
uint8_t ventilacion = 0;
unsigned long lastLCDUpdate = 0;


// Declaración anticipada para evitar problemas de compilación
void servidorWeb(void *pvParameters);


/*╔═══════════════════════════════════════╗
  ║ [ GLOBAL ] Configuraciones Iniciales  ║
  ╚═══════════════════════════════════════╝*/
void setup() {
  neopixelWrite(PIN_RGB, 255, 255, 0); // AMARILLO

  /* Comunicación Serial */
  Serial.begin(BAUDIOS);
  nervios.begin(BAUDIOS, SERIAL_8N1, PIN_RX, PIN_TX);

  // [FIX 1] Reducir timeout de readStringUntil() para no bloquear el loop
  // esperando el '\n' durante 1000ms (valor por defecto)
  nervios.setTimeout(100);

  delay(1000);

  mutexDatos = xSemaphoreCreateMutex();
  camStatus = initCamara();

  WiFi.begin(WIFI_RED, WIFI_PSW);
  while (WiFi.status() != WL_CONNECTED && intentos < 20) { delay(500); intentos++; }

  neopixelWrite(PIN_RGB, 0, 0, 255); // AZUL

  // Configurar I2C
  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("    SKELLENT    ");
  lcd.setCursor(0, 1);
  lcd.print("Skell's GrHs  V3");

  // Crear Iconos en la Memoria de la LCD
  lcd.createChar(0, iconoTemperatura);
  lcd.createChar(1, iconoHumedad);
  lcd.createChar(2, iconoWifi);
  lcd.createChar(3, iconoAltura);

  delay(5000);

  neopixelWrite(PIN_RGB, 255, 255, 0); // AMARILLO

  xTaskCreatePinnedToCore(
    servidorWeb,   // Función de la tarea
    "ServidorWeb", // Nombre identificador
    8192,          // Tamaño de pila (RAM asignada)
    NULL,          // Parámetros de entrada
    1,             // Prioridad de la tarea
    NULL,          // Handle
    0              // NÚCLEO 0 (Dedicado a WiFi)
  );
}


/*╔═══════════════════════════════╗
  ║ [ NÚCLEO 1 ] Búcle Principal  ║
  ╚═══════════════════════════════╝*/
void loop() {
  neopixelWrite(PIN_RGB, 255, 255, 255); // BLANCO
  actualizarLCD();

  // ── Variables de estado para lectura no bloqueante ─────────────────────
  static String bufferEntrante = "";

  // ── Recepción de datos del Común (no bloqueante) ────────────────────────
  while (nervios.available()) {
    char c = nervios.read();
    if (c == '\n') {
      // Línea completa recibida → procesar
      if (bufferEntrante.length() > 0) {
        JsonDocument datos;
        DeserializationError error = deserializeJson(datos, bufferEntrante);
        bufferEntrante = ""; // Limpiar siempre, haya error o no

        if (!error) {
          if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(100)) == pdTRUE) {
            sensores.sen_temperatura_ambiente = datos[0];
            sensores.sen_temperatura_agua     = datos[1];
            sensores.sen_humedad_ambiente     = datos[2];
            sensores.sen_humedad_suelo        = datos[3];
            sensores.sen_intensidad_luz       = datos[4];
            sensores.sen_ultrasonido          = datos[5];
            sensores.ia_perfilplanta          = detectarPerfil();
            xSemaphoreGive(mutexDatos);
          }

          /*
            EJECUCIÓN DE LA IA
          */

          ejecutarActuadores(150, true, 1);
        } else {
          Serial.print("[ERROR] JSON del Común corrupto: ");
          Serial.println(error.c_str());
        }
      }
    } else {
      bufferEntrante += c;
      // Protección contra basura infinita sin '\n'
      if (bufferEntrante.length() > 128) bufferEntrante = "";
    }
  }

  vTaskDelay(pdMS_TO_TICKS(200)); // Delay amigable con FreeRTOS
}


/*╔════════════════════════════════════════════╗
  ║ [ NÚCLEO 1 ] Enviar Comandos de Ejecución  ║
  ╚════════════════════════════════════════════╝*/
void ejecutarActuadores(int uv, bool riego, uint8_t ventilacion) {
  JsonDocument doc;
  JsonArray actuadores = doc.to<JsonArray>();
  actuadores.add(uv);          // Índice 0 en el nodo receptor
  actuadores.add(riego);       // Índice 1 en el nodo receptor
  actuadores.add(ventilacion); // Índice 2 en el nodo receptor

  serializeJson(doc, nervios);
  nervios.println();
}


/*╔═════════════════════════════════════════╗
  ║ [ NÚCLEO 1 ] Detector de Maceta/Perfil  ║
  ╚═════════════════════════════════════════╝*/
uint8_t detectarPerfil() {
  return 1;
}


void actualizarLCD() {
  if (millis() - lastLCDUpdate < 1000) return; // Solo actualizar cada 1s

  if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(50)) == pdTRUE) {
    lcd.clear();

    // Fila 1: Temperatura, Humedad y Altura
    lcd.setCursor(0, 0);
    lcd.write(0);
    lcd.print((int)sensores.sen_temperatura_ambiente);
    lcd.print("C ");
    lcd.write(1);
    lcd.print((int)sensores.sen_humedad_ambiente);
    lcd.print("% ");
    lcd.write(3);
    lcd.print((int)sensores.sen_ultrasonido);
    lcd.print("cm");

    // Fila 2: Estado WiFi e IP
    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.print(" ");
    lcd.print(WiFi.localIP());

    xSemaphoreGive(mutexDatos);
    lastLCDUpdate = millis();
  }
}


/*╔════════════════════════════╗
  ║ [ NÚCLEO 0 ] Servidor Web  ║
  ╚════════════════════════════╝*/
void servidorWeb(void *pvParameters) {

  /*╔══════════════════╗
    ║  Datos Globales  ║
    ╚══════════════════╝*/
  servidor.on("/api/datos", HTTP_GET, []() {
    JsonDocument informacion;
    if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(50)) == pdTRUE) {
      // Sensores
      informacion["sensores"]["temperatura_ambiente"] = sensores.sen_temperatura_ambiente;
      informacion["sensores"]["temperatura_agua"]     = sensores.sen_temperatura_agua;
      informacion["sensores"]["humedad_ambiente"]     = sensores.sen_humedad_ambiente;
      informacion["sensores"]["humedad_suelo"]        = sensores.sen_humedad_suelo;
      informacion["sensores"]["intensidad_luz"]       = sensores.sen_intensidad_luz;
      informacion["sensores"]["ultrasonido"]          = sensores.sen_ultrasonido;

      // Información del ESP32
      informacion["esp32"]["temperatura"] = sensores.esp32_temperatura;
      informacion["esp32"]["camStatus"]   = sensores.esp32_camStatus;

      // Información de la IA
      informacion["ia"]["intensidad_uv"] = sensores.ia_intensidad_uv;
      informacion["ia"]["riego"]         = sensores.ia_riego;
      informacion["ia"]["ventilacion"]   = sensores.ia_ventilacion;
      informacion["ia"]["perfil"]        = sensores.ia_perfilplanta;

      xSemaphoreGive(mutexDatos);

      String respuestaJSON;
      serializeJson(informacion, respuestaJSON);
      servidor.send(200, "application/json", respuestaJSON);
    } else {
      servidor.send(503, "application/json", "{\"error\":\"Inferencia en progreso\"}");
    }
  });


  /*╔══════════════╗
    ║  Fotografía  ║
    ╚══════════════╝*/
  servidor.on("/api/foto", HTTP_GET, []() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) { servidor.send(500, "text/plain", "Fallo al capturar imagen"); return; }
    servidor.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });


  // [FIX 3] Espera activa dentro de la tarea hasta confirmar conexión WiFi.
  // Antes: si los 20 intentos del setup() se agotaban sin conexión, el servidor
  // nunca arrancaba y no había ningún reintento posterior.
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(pdMS_TO_TICKS(500));
  }
  servidor.begin();
  Serial.print("[WiFi] Servidor activo en: http://");
  Serial.println(WiFi.localIP());

  // Iteraciones infinitas con intervalos de descanso para no saturar al núcleo
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) { servidor.handleClient(); }
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}