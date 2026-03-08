/*
  ###############################
  ### SKELL'S GREENHOUSE V1.0 ### Developed By: Robert Rodríguez "Skellent" & Christopher Ramirez
  ###############################
*/

#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "qrcode_gh.h" 
#include "DHT.h"

#define DHTPIN 16 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
float temperaturaEsp32 = 0;

const int pinDatos = 27;
OneWire oneWire(pinDatos);
DallasTemperature sensores(&oneWire);

const char* ssid = "mannqui";
const char* password = "576m12935266";

TFT_eSPI tft = TFT_eSPI();
WebServer server(80);
String ipAddress = "0.0.0.0";



// --- Variables de Estado y Sensores ---
volatile int estado = 1;
volatile bool forzarRedibujado = true;
unsigned long ultimoToque = 0;
const unsigned long debounceTime = 300;

float humedadAire = 0, temperatura = 0, humedadTierra = 0, litrosAgua = 0;
float hA_old, temp_old, hT_old, litros_old; 

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  // Calibración táctil si fuera necesaria (depende de tu setup)
  xTaskCreatePinnedToCore(tareaServidorWeb, "WebSrv", 10000, NULL, 1, NULL, 0);
  dht.begin();
  sensores.begin();
}

void loop() {
  actualizarSensores();
  manejarTouch();
  
  if (estado == 1) {
    if (forzarRedibujado) {
      dibujarIconosGrandes();
      actualizarValoresPantalla(true);
      forzarRedibujado = false;
    } else {
      actualizarValoresPantalla(false);
    }
  } else if (estado == 4) {
    animarCaritaGigante(); 
  } else if (forzarRedibujado) {
    switch (estado) {
      case 2: mostrarEstado2XL(); break;
      case 3: mostrarEstado3(); break;
    }
    forzarRedibujado = false;
  }
  delay(30); 
}

void actualizarSensores() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 2000) {
    humedadAire = dht.readHumidity();
    temperatura = dht.readTemperature();
    Serial.print(temperatura);
    sensores.requestTemperatures();
    float temperaturaEsp32 = sensores.getTempCByIndex(0);
    humedadTierra = random(100, 900) / 10.0;
    litrosAgua = random(10, 600) / 10.0;
    lastUpdate = millis();
    
  }
  if(temperaturaEsp32 == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Sensor no encontrado");
  }
}

void manejarTouch() {
  uint16_t x, y;
  if (tft.getTouch(&x, &y)) {
    if (millis() - ultimoToque > debounceTime) {
      estado = (estado % 4) + 1;
      forzarRedibujado = true;
      ultimoToque = millis();
      tft.fillScreen(TFT_BLACK);
    }
  }
}

// -------------------------------------------------------------------
// PANTALLA: ESTADO 1 (MONITOREO)
// -------------------------------------------------------------------
void dibujarIconosGrandes() {
  tft.drawLine(160, 0, 160, 240, TFT_DARKGREY);
  tft.drawLine(0, 120, 320, 120, TFT_DARKGREY);
  tft.fillCircle(60, 45, 15, TFT_WHITE); tft.fillCircle(80, 35, 20, TFT_WHITE); tft.fillCircle(100, 45, 15, TFT_WHITE); // Aire
  tft.fillRoundRect(235, 15, 12, 50, 6, TFT_LIGHTGREY); tft.fillCircle(241, 70, 14, TFT_RED); // Temp
  tft.fillCircle(80, 165, 18, 0x9381); tft.fillTriangle(62, 165, 98, 165, 80, 135, 0x9381); // Tierra
  tft.drawRect(220, 140, 40, 50, TFT_WHITE); tft.fillRect(222, 165, 36, 23, TFT_BLUE); // Agua
}

void actualizarValoresPantalla(bool forzar) {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (humedadAire != hA_old || forzar) { tft.fillRect(10, 90, 140, 25, TFT_BLACK); tft.setCursor(15, 90); tft.printf("Air: %.1f%%", humedadAire); hA_old = humedadAire; }
  if (temperatura != temp_old || forzar) { tft.fillRect(170, 90, 140, 25, TFT_BLACK); tft.setCursor(175, 90); tft.printf("Tmp: %.1fC", temperatura); temp_old = temperatura; }
  if (humedadTierra != hT_old || forzar) { tft.fillRect(10, 210, 140, 25, TFT_BLACK); tft.setCursor(15, 210); tft.printf("Soil:%.1f%%", humedadTierra); hT_old = humedadTierra; }
  if (litrosAgua != litros_old || forzar) { tft.fillRect(170, 210, 140, 25, TFT_BLACK); tft.setCursor(175, 210); tft.printf("Wat: %.1fL", litrosAgua); litros_old = litrosAgua; }
}

// -------------------------------------------------------------------
// PANTALLA: ESTADO 2 (QR XL)
// -------------------------------------------------------------------
void mostrarEstado2XL() {
  tft.fillScreen(TFT_BLACK);
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, ("http://" + ipAddress).c_str());
  
  int tam = 6; // Aumentado a 6 para máxima visibilidad
  int offX = (320 - qrcode.size * tam) / 2;
  int offY = (240 - qrcode.size * tam) / 2 - 15;

  for (int y = 0; y < qrcode.size; y++) {
    for (int x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) tft.fillRect(offX + x*tam, offY + y*tam, tam, tam, TFT_WHITE);
    }
  }
  tft.setTextSize(2);
  tft.setCursor((320 - (ipAddress.length()+7)*12)/2, 220); 
  tft.print("http://" + ipAddress);
}

// -------------------------------------------------------------------
// PANTALLA: ESTADO 3 (CREDITOS)
// -------------------------------------------------------------------
void mostrarEstado3() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextSize(2);
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.setCursor(10, 40); tft.println("Skell's GreenHouse");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setCursor(10, 80); tft.println("V1.3 - ESP32-S3");
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(10, 140); tft.println("Robert Rodriguez");
  tft.setCursor(10, 170); tft.println("Christopher Ramirez");
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

// -------------------------------------------------------------------
// PANTALLA: ESTADO 4 (EMOJI GIGANTE)
// -------------------------------------------------------------------
void animarCaritaGigante() {
  static unsigned long lastAnim = 0;
  static bool ojosCerrados = false;
  int cx = 160, cy = 60;

  if (millis() - lastAnim > (ojosCerrados ? 120 : 3500)) {
    ojosCerrados = !ojosCerrados;
    lastAnim = millis();
    
    if (ojosCerrados) {
      tft.fillRect(cx - 80, cy - 40, 160, 50, TFT_BLACK); 
      tft.fillRect(cx - 70, cy - 10, 40, 4, TFT_WHITE); // Ojo L cerrado XL
      tft.fillRect(cx + 30, cy - 10, 40, 4, TFT_WHITE); // Ojo R cerrado XL
    } else {
      tft.fillRect(cx - 80, cy - 40, 160, 50, TFT_BLACK);
      tft.fillCircle(cx - 50, cy - 15, 18, TFT_WHITE); // Ojo L XL
      tft.fillCircle(cx + 50, cy - 15, 18, TFT_WHITE); // Ojo R XL
    }
    tft.drawArc(cx, cy + 30, 70, 60, 270, 90, TFT_WHITE, TFT_BLACK, true); // Sonrisa XL
  }
}

// -------------------------------------------------------------------
// SERVIDOR WEB (RESPONSIVE FULLSCREEN)
// -------------------------------------------------------------------
void tareaServidorWeb(void* p) {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  ipAddress = WiFi.localIP().toString();

  server.on("/", []() {
    String html = R"rawliteral(
    <!DOCTYPE html><html><head>
    <meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no' charset='UTF-8'>
    <style>
      * { box-sizing: border-box; margin: 0; padding: 0; }
      body { font-family: -apple-system, sans-serif; background: #000; color: white; height: 100vh; display: flex; flex-direction: column; }
      header { padding: 15px; text-align: center; background: #1a1a1a; color: #00adb5; font-size: 1.2em; font-weight: bold; }
      .container { flex: 1; display: flex; flex-direction: column; padding: 10px; gap: 10px; }
      .row { flex: 1; display: flex; gap: 10px; }
      .card { flex: 1; background: #1e1e1e; border-radius: 15px; display: flex; flex-direction: column; justify-content: center; align-items: center; border-bottom: 6px solid #444; }
      .label { font-size: 0.9em; text-transform: uppercase; color: #aaa; margin-bottom: 5px; }
      .val { font-size: 2.5em; font-weight: bold; }
      .unit { font-size: 0.4em; color: #00adb5; }
      footer { padding: 5px; text-align: center; font-size: 0.7em; color: #444; }
    </style>
    <script>
      function update() {
        fetch('/data').then(r => r.json()).then(d => {
          document.getElementById('ha').innerText = d.ha;
          document.getElementById('te').innerText = d.te;
          document.getElementById('ht').innerText = d.ht;
          document.getElementById('li').innerText = d.li;
        });
      }
      setInterval(update, 2000);
    </script>
    </head><body onload='update()'>
      <header>Skell's GreenHouse Live</header>
      <div class='container'>
        <div class='row'>
          <div class='card' style='border-color: #00adb5'><div class='label'>Aire</div><div class='val'><span id='ha'>--</span><span class='unit'>%</span></div></div>
          <div class='card' style='border-color: #ff5722'><div class='label'>Temp</div><div class='val'><span id='te'>--</span><span class='unit'>°C</span></div></div>
        </div>
        <div class='row'>
          <div class='card' style='border-color: #8bc34a'><div class='label'>Tierra</div><div class='val'><span id='ht'>--</span><span class='unit'>%</span></div></div>
          <div class='card' style='border-color: #2196f3'><div class='label'>Agua</div><div class='val'><span id='li'>--</span><span class='unit'>L</span></div></div>
        </div>
      </div>
      <footer>Skell was here</footer>
    </body></html>)rawliteral";
    server.send(200, "text/html", html);
  });

  server.on("/data", []() {
    String json = "{\"ha\":" + String(humedadAire, 1) + ",\"te\":" + String(temperatura, 1) + 
                  ",\"ht\":" + String(humedadTierra, 1) + ",\"li\":" + String(litrosAgua, 1) + "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  while (true) { server.handleClient(); delay(10); }
}