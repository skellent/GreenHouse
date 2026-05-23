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
#include <Preferences.h>
#include "esp_camera.h"
#include <LiquidCrystal_I2C.h>


/*╔═════════════════════════════╗
  ║ [ GLOBAL ] Módulos Propios  ║
  ╚═════════════════════════════╝*/
#include "config.h"
#include "sensors.h"
#include "cam.h"
#include "icons.h"


/*╔═══════════════════════════════════╗
  ║ [ GLOBAL ] Instancias Requeridas  ║
  ╚═══════════════════════════════════╝*/
Sensores          sensores;
SemaphoreHandle_t mutexDatos;
WebServer         servidor(80);
HardwareSerial    nervios(1);
LiquidCrystal_I2C lcd(LCD_PROTOCOLO, 16, 2);
Preferences       prefs;


/*╔═══════════════════════════════════════════╗
  ║ [ GLOBAL ] Variables Globales/Temporales  ║
  ╚═══════════════════════════════════════════╝*/
bool    camStatus   = false;
uint8_t intentos    = 0;
uint8_t uv          = 0;
bool    riego       = false;
uint8_t ventilacion = 0;
unsigned long lastLCDUpdate = 0;

// Credenciales WiFi activas (cargadas desde NVS o fallback a config.h)
String wifiRed = WIFI_RED;
String wifiPsw = WIFI_PSW;


// Declaraciones anticipadas
void servidorWeb(void *pvParameters);
bool conectarWiFi(const String& red, const String& psw, uint8_t maxIntentos = 20);


/*╔═══════════════════════════════════════╗
  ║ [ GLOBAL ] Configuraciones Iniciales  ║
  ╚═══════════════════════════════════════╝*/
void setup() {
  neopixelWrite(PIN_RGB, 255, 255, 0); // AMARILLO

  //pinMode(PIN_BUZZER, OUTPUT);

  Serial.begin(BAUDIOS);
  nervios.begin(BAUDIOS, SERIAL_8N1, PIN_RX, PIN_TX);
  // Sin setTimeout: la lectura en loop() es no bloqueante (byte a byte)

  delay(1000);

  // ── Cargar preferencias persistentes desde NVS ─────────────────────────
  prefs.begin("greenhouse", false);

  if (prefs.isKey("wifi_red")) {
    wifiRed = prefs.getString("wifi_red", WIFI_RED);
    wifiPsw = prefs.getString("wifi_psw", WIFI_PSW);
    Serial.println("[NVS] Credenciales WiFi cargadas desde memoria.");
  }

  // Perfil 1 por defecto si nunca se ha guardado uno
  sensores.ia_perfilplanta = prefs.getUChar("perfil", 1);
  Serial.printf("[NVS] Perfil activo: %d\n", sensores.ia_perfilplanta);

  prefs.end();

  // ── Resto del hardware ─────────────────────────────────────────────────
  mutexDatos = xSemaphoreCreateMutex();
  camStatus  = initCamara();

  conectarWiFi(wifiRed, wifiPsw);

  neopixelWrite(PIN_RGB, 0, 0, 255); // AZUL

  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("    SKELLENT    ");
  lcd.setCursor(0, 1);
  lcd.print("Skell's GrHs  V3");

  lcd.createChar(0, iconoTemperatura);
  lcd.createChar(1, iconoHumedad);
  lcd.createChar(2, iconoWifi);
  lcd.createChar(3, iconoAltura);

  delay(5000);
  neopixelWrite(PIN_RGB, 255, 255, 0); // AMARILLO

  xTaskCreatePinnedToCore(servidorWeb, "ServidorWeb", 8192, NULL, 1, NULL, 0);
}


/*╔══════════════════════════════════╗
  ║ [ GLOBAL ] Conectar a WiFi       ║
  ╚══════════════════════════════════╝*/
bool conectarWiFi(const String& red, const String& psw, uint8_t maxIntentos) {
  WiFi.disconnect(true);
  delay(200);
  WiFi.begin(red.c_str(), psw.c_str());
  uint8_t i = 0;
  while (WiFi.status() != WL_CONNECTED && i < maxIntentos) { delay(500); i++; }
  return WiFi.status() == WL_CONNECTED;
}


/*╔═══════════════════════════════╗
  ║ [ NÚCLEO 1 ] Búcle Principal  ║
  ╚═══════════════════════════════╝*/
void loop() {
  neopixelWrite(PIN_RGB, 255, 255, 255); // BLANCO
  actualizarLCD();
  //tone(PIN_BUZZER, random(100, 1000), 500);

  // ── Lectura UART no bloqueante (byte a byte) ───────────────────────────
  static String bufferEntrante = "";

  while (nervios.available()) {
    // noTone(PIN_BUZZER);
    // neopixelWrite(PIN_RGB, 0, 0, 255); // AZUL

    char c = nervios.read();
    if (c == '\n') {
      if (bufferEntrante.length() > 0) {
        JsonDocument datos;
        DeserializationError error = deserializeJson(datos, bufferEntrante);
        bufferEntrante = "";

        if (!error) {
          if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(100)) == pdTRUE) {
            sensores.sen_temperatura_ambiente = datos[0];
            sensores.sen_temperatura_agua     = datos[1];
            sensores.sen_humedad_ambiente     = datos[2];
            sensores.sen_humedad_suelo        = datos[3];
            sensores.sen_intensidad_luz       = datos[4];
            sensores.sen_ultrasonido          = datos[5];
            // ia_perfilplanta ya está cargado desde NVS en setup()
            detectarPerfil();
            xSemaphoreGive(mutexDatos);
          }

          /*
            EJECUCIÓN DE LA IA
            INPUT:  sensores.sen_*  y  sensores.ia_perfilplanta
            OUTPUT: sensores.ia_ventilacion, ia_intensidad_uv, ia_riego
          */

          ejecutarActuadores(150, true, 1);
        } else {
          Serial.print("[ERROR] JSON del Común corrupto: ");
          Serial.println(error.c_str());
        }
      }
    } else {
      bufferEntrante += c;
      if (bufferEntrante.length() > 128) bufferEntrante = "";
    }
  }

  vTaskDelay(pdMS_TO_TICKS(200));
}


/*╔════════════════════════════════════════════╗
  ║ [ NÚCLEO 1 ] Enviar Comandos de Ejecución  ║
  ╚════════════════════════════════════════════╝*/
void ejecutarActuadores(int uv, bool riego, uint8_t ventilacion) {
  JsonDocument doc;
  JsonArray actuadores = doc.to<JsonArray>();
  actuadores.add(uv);
  actuadores.add(riego);
  actuadores.add(ventilacion);
  serializeJson(doc, nervios);
  nervios.println();
}


/*╔═════════════════════════════════════════╗
  ║ [ NÚCLEO 1 ] Detector de Maceta/Perfil  ║
  ╚═════════════════════════════════════════╝*/
void detectarPerfil() {
  // sensores.ia_perfilplanta ya está cargado desde NVS.
  // Aquí añade lógica visual o de ajuste según el perfil activo.
  Serial.printf("[Perfil] Perfil activo: %d\n", sensores.ia_perfilplanta);
}


/*╔════════════════════════════╗
  ║ [ NÚCLEO 1 ] PANTALLA LCD  ║
  ╚════════════════════════════╝*/
void actualizarLCD() {
  if (millis() - lastLCDUpdate < 1000) return;

  if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(50)) == pdTRUE) {
    lcd.clear();

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
    ║  GET /api/datos  ║
    ╚══════════════════╝*/
  servidor.on("/api/datos", HTTP_GET, []() {
    JsonDocument informacion;
    if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(50)) == pdTRUE) {
      informacion["sensores"]["temperatura_ambiente"] = sensores.sen_temperatura_ambiente;
      informacion["sensores"]["temperatura_agua"]     = sensores.sen_temperatura_agua;
      informacion["sensores"]["humedad_ambiente"]     = sensores.sen_humedad_ambiente;
      informacion["sensores"]["humedad_suelo"]        = sensores.sen_humedad_suelo;
      informacion["sensores"]["intensidad_luz"]       = sensores.sen_intensidad_luz;
      informacion["sensores"]["ultrasonido"]          = sensores.sen_ultrasonido;
      informacion["esp32"]["temperatura"]             = sensores.esp32_temperatura;
      informacion["esp32"]["camStatus"]               = sensores.esp32_camStatus;
      informacion["ia"]["intensidad_uv"]              = sensores.ia_intensidad_uv;
      informacion["ia"]["riego"]                      = sensores.ia_riego;
      informacion["ia"]["ventilacion"]                = sensores.ia_ventilacion;
      informacion["ia"]["perfil"]                     = sensores.ia_perfilplanta;
      xSemaphoreGive(mutexDatos);

      String respuestaJSON;
      serializeJson(informacion, respuestaJSON);
      servidor.send(200, "application/json", respuestaJSON);
    } else {
      servidor.send(503, "application/json", "{\"error\":\"Inferencia en progreso\"}");
    }
  });


  /*╔═══════════════════╗
    ║  GET  /api/foto   ║
    ╚═══════════════════╝*/
  servidor.on("/api/foto", HTTP_GET, []() {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) { servidor.send(500, "text/plain", "Fallo al capturar imagen"); return; }
    servidor.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
  });


  /*╔══════════════════════╗
    ║  POST /api/perfil    ║
    ╚══════════════════════╝*/
  // Body: número plano 1–3 (ej: "2"). Se clampea si se excede el rango.
  servidor.on("/api/perfil", HTTP_POST, []() {
    if (!servidor.hasArg("plain") || servidor.arg("plain").length() == 0) {
      servidor.send(400, "application/json", "{\"error\":\"Body vacío\"}");
      return;
    }

    int nuevoPerfil = servidor.arg("plain").toInt();
    if (nuevoPerfil < 1) nuevoPerfil = 1;
    if (nuevoPerfil > 3) nuevoPerfil = 3;

    if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(100)) == pdTRUE) {
      sensores.ia_perfilplanta = (uint8_t)nuevoPerfil;
      xSemaphoreGive(mutexDatos);
    }

    prefs.begin("greenhouse", false);
    prefs.putUChar("perfil", (uint8_t)nuevoPerfil);
    prefs.end();

    JsonDocument resp;
    resp["ok"]     = true;
    resp["perfil"] = nuevoPerfil;
    String out;
    serializeJson(resp, out);
    servidor.send(200, "application/json", out);
  });


  /*╔══════════════════════╗
    ║  POST /api/wifi      ║
    ╚══════════════════════╝*/
  // Body JSON: { "red": "NombreRed", "psw": "Contraseña" }
  // Intenta conectarse. Si lo logra, guarda en NVS. Si falla, mantiene la
  // red anterior. Responde ANTES de reconectar para no perder la conexión HTTP.
  servidor.on("/api/wifi", HTTP_POST, []() {
    if (!servidor.hasArg("plain") || servidor.arg("plain").length() == 0) {
      servidor.send(400, "application/json", "{\"error\":\"Body vacío\"}");
      return;
    }

    JsonDocument body;
    DeserializationError err = deserializeJson(body, servidor.arg("plain"));
    if (err || !body["red"].is<const char*>() || !body["psw"].is<const char*>()) {
      servidor.send(400, "application/json", "{\"error\":\"JSON inválido o campos faltantes\"}");
      return;
    }

    String nuevaRed = body["red"].as<String>();
    String nuevaPsw = body["psw"].as<String>();

    Serial.printf("[API/WiFi] Intentando conectar a: %s\n", nuevaRed.c_str());

    servidor.send(200, "application/json",
      "{\"ok\":true,\"mensaje\":\"Reconectando... espera ~15s y consulta la nueva IP en tu router\"}");

    vTaskDelay(pdMS_TO_TICKS(500)); // Dar tiempo a que la respuesta se envíe

    bool exito = conectarWiFi(nuevaRed, nuevaPsw, 20);

    if (exito) {
      prefs.begin("greenhouse", false);
      prefs.putString("wifi_red", nuevaRed);
      prefs.putString("wifi_psw", nuevaPsw);
      prefs.end();
      wifiRed = nuevaRed;
      wifiPsw = nuevaPsw;
    } else {
      conectarWiFi(wifiRed, wifiPsw, 20);
    }
  });

  // Esperar conexión activa antes de levantar el servidor
  while (WiFi.status() != WL_CONNECTED) { vTaskDelay(pdMS_TO_TICKS(500)); }
  servidor.begin();

  for (;;) {
    if (WiFi.status() == WL_CONNECTED) { servidor.handleClient(); }
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}