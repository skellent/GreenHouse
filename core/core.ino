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
#include "alicia.h"   // Red neuronal Alicia (TFLite Micro)


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
bool    camStatus        = false;
uint8_t intentos         = 0;
unsigned long lastLCDUpdate = 0;
String wifiRed = WIFI_RED;
String wifiPsw = WIFI_PSW;
static String bufferEntrante = "";


/*╔═════════════════════════════════════════╗
  ║ [ GLOBAL ]  Constructores de Funciones  ║
  ╚═════════════════════════════════════════╝*/
void servidorWeb(void *pvParameters);
bool conectarWiFi(const String& red, const String& psw, uint8_t maxIntentos = 20);


/*╔═══════════════════════════════════════╗
  ║ [ GLOBAL ] Configuraciones Iniciales  ║
  ╚═══════════════════════════════════════╝*/
void setup() {
  neopixelWrite(PIN_RGB, 255, 255, 0); // AMARILLO

  /* INICIALIZACIÓN DE LA PANTALLA */
  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("    SKELLENT    ");
  lcd.setCursor(0, 1);
  lcd.print("Skell's GrHs  V3");

  neopixelWrite(PIN_RGB, 255, 0, 0); // ROJO

  /* INICIALIZACIÓN DE PINES */
  pinMode(LCD_BOTON, INPUT_PULLUP);

  /* INICIALIZACIÓN DE MONITORES SERIALES (UART) */
  Serial.begin(BAUDIOS);
  nervios.begin(BAUDIOS, SERIAL_8N1, PIN_RX, PIN_TX);

  /* CARGA A LA RAM PREFERENCIAS/CONFIGURACIONES */
  prefs.begin("greenhouse", false);
  if (prefs.isKey("wifi_red")) {
    wifiRed = prefs.getString("wifi_red", WIFI_RED);
    wifiPsw = prefs.getString("wifi_psw", WIFI_PSW);
  }
  sensores.ia_perfilplanta = prefs.getUChar("perfil", 1);
  prefs.end();

  /* HARDWARE: mutex, cámara, WiFi */
  mutexDatos = xSemaphoreCreateMutex();
  camStatus  = initCamara();
  sensores.esp32_camStatus = camStatus;
  conectarWiFi(wifiRed, wifiPsw);

  /* CREACIÓN DE ÍCONOS PARA LA PANTALLA */
  lcd.createChar(0, iconoTemperatura);
  lcd.createChar(1, iconoSuelo);
  lcd.createChar(2, iconoWifi);
  lcd.createChar(3, iconoTanque);
  lcd.createChar(4, iconoRed);
  lcd.createChar(5, iconoUV);
  lcd.createChar(6, iconoRiego);

  /* INICIALIZACIÓN DE ALICIA */
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando Alicia");
  if (!Alicia::init()) {
    lcd.setCursor(0, 1);
    lcd.print("  ERROR en IA!  ");
    neopixelWrite(PIN_RGB, 255, 0, 0);
  } else {
    lcd.setCursor(0, 1);
    lcd.print("  Alicia lista  ");
    neopixelWrite(PIN_RGB, 0, 255, 0); // VERDE
    delay(1200);
  }

  /* DELEGACIÓN DE API AL SEGUNDO NÚCLEO */
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
  while (WiFi.status() != WL_CONNECTED && i < maxIntentos) {
    delay(500);
    i++;
  }
  return WiFi.status() == WL_CONNECTED;
}


/*╔═══════════════════════════════╗
  ║ [ NÚCLEO 1 ] Búcle Principal  ║
  ╚═══════════════════════════════╝*/
void loop() {
  neopixelWrite(PIN_RGB, 0, 0, 255); // AZUL
  actualizarLCD();
  bufferEntrante = "";

  /* LEE JSON ENTRANTE POR UART (enviado por el ESP32 / nervius) */
  while (nervios.available()) {
    char c = nervios.read();
    if (c == '\n') {
      if (bufferEntrante.length() > 0) {
        JsonDocument datos;
        DeserializationError error = deserializeJson(datos, bufferEntrante);
        bufferEntrante = "";

        if (!error) {
          /* Actualizar sensores con mutex para proteger la RAM compartida */
          if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(100)) == pdTRUE) {
            sensores.sen_temperatura_ambiente = datos[0];
            sensores.sen_temperatura_agua     = datos[1];   // DS18[0]
            sensores.sen_humedad_ambiente     = datos[2];
            sensores.sen_humedad_suelo        = datos[3];
            sensores.sen_intensidad_luz       = datos[4];
            sensores.sen_nivel_tanque         = datos[5];
            sensores.sen_temperatura_agua_b   = datos[6];   // DS18[1]
            sensores.sen_altura_planta        = datos[7];
            xSemaphoreGive(mutexDatos);
          }

          /*╔══════════════════════╗
            ║  EJECUCIÓN DE ALICIA ║
            ╚══════════════════════╝*/
          int  uv_ia    = 0;
          bool riego_ia = false;

          if (Alicia::inferir(sensores, uv_ia, riego_ia)) {
            if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(50)) == pdTRUE) {
              sensores.ia_intensidad_uv = uv_ia;
              sensores.ia_riego         = riego_ia ? 1 : 0;
              sensores.ia_ventilacion   = 0;  // No controlado por la IA
              xSemaphoreGive(mutexDatos);
            }
            ejecutarActuadores(uv_ia, riego_ia, 0);
          } else {
            /* Fallback seguro si la inferencia falla */
            ejecutarActuadores(0, false, 0);
          }
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
  actuadores.add(uv);          // [0] Intensidad UV (PWM 0-255)
  actuadores.add(riego);       // [1] Riego (on/off)
  actuadores.add(ventilacion); // [2] Ventilación (reservado, siempre 0)
  serializeJson(doc, nervios);
  nervios.println();
}


/*╔══════════════════════════════════════════════════════════╗
  ║ [ NÚCLEO 1 ] PANTALLA LCD                                ║
  ╠══════════════════════════════════════════════════════════╣
  ║  Botón NO PRESIONADO → Sensores + salidas de la IA       ║
  ║    Fila 1: [T]##C [S]##% [A]##%                         ║
  ║    Fila 2: [UV]:### [R]:SI/NO                           ║
  ║                                                          ║
  ║  Botón PRESIONADO  → Info de red WiFi                    ║
  ║    Fila 1: [red_icon] NombreRed                         ║
  ║    Fila 2: [wifi_icon] IP                               ║
  ╚══════════════════════════════════════════════════════════╝*/
void actualizarLCD() {
  if (millis() - lastLCDUpdate < 1000) return;
  lcd.clear();
  lcd.setCursor(0, 0);

  if (!digitalRead(LCD_BOTON)) {
    // Botón presionado → info de red WiFi
    lcd.write(4);
    lcd.print(" ");
    lcd.print(wifiRed);
    lcd.setCursor(0, 1);
    lcd.write(2);
    lcd.print(" ");
    lcd.print(WiFi.localIP());
  } else {
    // Botón sin presionar → sensores + salidas de la IA
    if (xSemaphoreTake(mutexDatos, pdMS_TO_TICKS(50)) == pdTRUE) {
      /* FILA 1: temperatura ambiente | humedad suelo | nivel tanque */
      lcd.write(0);                                        // [T] termómetro
      lcd.print((int)sensores.sen_temperatura_ambiente);
      lcd.print("C ");
      lcd.write(1);                                        // [S] maceta
      lcd.print(min((int)sensores.sen_humedad_suelo, 100));
      lcd.print("% ");
      lcd.write(3);                                        // [A] tanque
      lcd.print(min((int)sensores.sen_nivel_tanque, 100));
      lcd.print("%");

      /* FILA 2: intensidad UV | riego (decisiones de Alicia) */
      lcd.setCursor(0, 1);
      lcd.write(5);                                        // [UV] sol
      lcd.print(":");
      lcd.print(sensores.ia_intensidad_uv);
      lcd.print("  ");
      lcd.write(6);                                        // [R] gotas
      lcd.print(":");
      lcd.print(sensores.ia_riego ? "SI" : "NO");

      xSemaphoreGive(mutexDatos);
    }
  }

  lastLCDUpdate = millis();
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
      informacion["sensores"]["temperatura_agua_a"]   = sensores.sen_temperatura_agua;
      informacion["sensores"]["temperatura_agua_b"]   = sensores.sen_temperatura_agua_b;
      informacion["sensores"]["humedad_ambiente"]     = sensores.sen_humedad_ambiente;
      informacion["sensores"]["humedad_suelo"]        = sensores.sen_humedad_suelo;
      informacion["sensores"]["intensidad_luz"]       = sensores.sen_intensidad_luz;
      informacion["sensores"]["nivel_tanque"]          = sensores.sen_nivel_tanque;
      informacion["sensores"]["ultrasonido"]            = sensores.sen_altura_planta;
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

    vTaskDelay(pdMS_TO_TICKS(500));

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


  while (WiFi.status() != WL_CONNECTED) { vTaskDelay(pdMS_TO_TICKS(500)); }
  servidor.begin();

  for (;;) {
    if (WiFi.status() == WL_CONNECTED) { servidor.handleClient(); }
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}
