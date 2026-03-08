/*
  ###################################################################################################
  ### SKELL'S GREENHOUSE V1.0 ### Developed By: Robert Rodríguez "Skellent" & Christopher Ramirez ###
  ###################################################################################################
*/

// IMPORTACION DE LIBRERIAS
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "qrcode_gh.h" 
#include "DHT.h"


// TEMPERATURA AMBIENTE
#define DHTPIN 16 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


// TEMPERATURA ESP32
const int pinDatos = 27;
OneWire oneWire(pinDatos);
DallasTemperature sensores(&oneWire);
float temperaturaEsp32 = 0;


// CREDENCIALES WIFI
const char* ssid = "mannqui";
const char* password = "576m12935266";


// INSTANCIAS DE OBJETOS
TFT_eSPI tft = TFT_eSPI(); // PANTALLA
WebServer server(80); // SERVIDOR WEB


// NIVEL DE ESCUCHA DEL SERVIDOR
String ipAddress = "0.0.0.0";


// VARIABLES DE ESTADO
volatile int estado = 1;
volatile bool forzarRedibujado = true;
unsigned long ultimoToque = 0;
const unsigned long debounceTime = 300;


// VARIABLES DE SENSORES
float humedadAire = 0;
float temperatura = 0;
float humedadTierra = 0;
float litrosAgua = 0;
float hA_old;
float temp_old;
float hT_old;
float litros_old; 


// CONFIGURACION DEL PROGRAMA
void setup() {


  Serial.begin(115200);
  

  // CONFIGURACION DE PANTALLA
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  

  // DELEGACION DE SERVIDOR WEB A NUCLEO SECUNDARIO
  xTaskCreatePinnedToCore(tareaServidorWeb, "WebSrv", 10000, NULL, 1, NULL, 0);
  

  // Inicio de sensores
  dht.begin();
  sensores.begin();
}


// BUCLE PRINCIPAL
void loop() {

  actualizarSensores(); // MANTIENE ACTUALIZADO LOS SENSORES POR CADA ITERACION
  manejarTouch(); // GESTIONA LA PARTE TACTIL DE LA PANTALLA
  

  /* ### MAQUINA DE ESTADOS ###
    Permite controlar el flujo de la GUI en la pantalla
    mediante un sistema rudimentario simple de estados */
  switch (estado) {
    // ESTADO 1: Mostrar los valores de los sensores.
    case 1: 
      if (forzarRedibujado) {
        estadoUno();
        actualizarValoresPantalla(true);
        forzarRedibujado = false;
      } else { actualizarValoresPantalla(false); } break;


    // ESTADO 2: Mostrar QR para accesibilidad de conexion
    case 2: 
      if (forzarRedibujado) { estadoDos(); } forzarRedibujado = false; break;


    // ESTADO 3: Mostrar creditos de desarrollo
    case 3: 
      if (forzarRedibujado) { estadoTres(); } forzarRedibujado = false; break;


    // ESTADO 4: Carita Feliz animada para fines decorativos
    case 4: 
      estadoCuatro(); break;
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


void actualizarValoresPantalla(bool forzar) {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (humedadAire != hA_old || forzar) { tft.fillRect(10, 90, 140, 22, TFT_BLACK); tft.setCursor(15, 90); tft.printf("H.Air: %.1f%%", humedadAire); hA_old = humedadAire; }
  if (temperatura != temp_old || forzar) { tft.fillRect(173, 90, 140, 25, TFT_BLACK); tft.setCursor(175, 90); tft.printf("Tmp: %.1fC", temperatura); temp_old = temperatura; }
  if (humedadTierra != hT_old || forzar) { tft.fillRect(10, 210, 140, 25, TFT_BLACK); tft.setCursor(15, 210); tft.printf("Soil:%.1f%%", humedadTierra); hT_old = humedadTierra; }
  if (litrosAgua != litros_old || forzar) { tft.fillRect(170, 210, 140, 25, TFT_BLACK); tft.setCursor(175, 210); tft.printf("Wat: %.1fL", litrosAgua); litros_old = litrosAgua; }
}


// PERMITE DIBUJAR GOTAS DE AGUA FACILMENTE
void dibujarGota(int x, int y, int size, uint16_t color) {
  tft.fillCircle(x, y, size, color);
  // Dibujar la punta de la gota (triángulo proporcional)
  // Punto 0: La punta superior (x centrada, y desplazada hacia arriba)
  // Punto 1: Esquina izquierda del triángulo (tangente al círculo)
  // Punto 2: Esquina derecha del triángulo (tangente al círculo)
  int puntaY = y - (size * 1.8); // Altura de la punta (1.8 veces el radio)
  int anchoBase = size * 0.9;    // Un poco más estrecho que el radio para suavizar la unión
  tft.fillTriangle(
    x, puntaY,            // P0: Punta arriba
    x - anchoBase, y,     // P1: Izquierda
    x + anchoBase, y,     // P2: Derecha
    color
  );
}


// ESTADO 1: Mostrar los valores de los sensores.
void estadoUno() {
  // DIBUJAR LOS ICONOS
  int x, y; // Para la posicion central de los iconos

  // Ancho: 320; Alto: 240;
  const int ancho = tft.width();
  const int alto = tft.height();
  const int anchoMedio = ancho / 2;
  const int altoMedio = alto / 2;

  // Dibujar el Marco de la GUI
  uint16_t marcoColor = TFT_GREEN;
  int marcoAncho = anchoMedio - 2;
  int marcoAlto = altoMedio - 2;
  int marcoRedondeo = 7;
  tft.drawRoundRect(2, 2, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Humedad del Aire
  tft.drawRoundRect(162, 2, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Temperatura
  tft.drawRoundRect(2, 122, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Humedad de la Tierra
  tft.drawRoundRect(162, 122, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Litros de Agua
  tft.fillCircle(anchoMedio, altoMedio, 14, TFT_WHITE);
  tft.fillCircle(anchoMedio - 5, altoMedio - 4, 3, TFT_BLACK);
  tft.fillCircle(anchoMedio + 5, altoMedio - 4, 3, TFT_BLACK);
  tft.drawArc(anchoMedio, altoMedio + 2, 7, 6, 270, 90, TFT_BLACK, TFT_WHITE, true);

  // Icono de Humedad del Aire
  tft.fillCircle(60, 45, 15, TFT_WHITE);
  tft.fillCircle(80, 35, 20, TFT_WHITE);
  tft.fillCircle(100, 45, 15, TFT_WHITE);
  dibujarGota(70, 55, 6, TFT_CYAN);
  dibujarGota(90, 65, 6, TFT_CYAN);

  // Icono de Temperatura
  tft.fillRoundRect(235, 15, 12, 50, 6, TFT_LIGHTGREY);
  tft.fillCircle(241, 70, 14, TFT_RED);
  
  // Icono de Humedad de la Tierra
  tft.fillCircle(80, 165, 18, 0x9381);
  tft.fillTriangle(62, 165, 98, 165, 80, 135, 0x9381);

  // Icono de Litros de Agua disponible
  tft.drawRect(220, 140, 40, 50, TFT_WHITE);
  tft.fillRect(222, 165, 36, 23, TFT_BLUE);

  
}


// ESTADO 2: Mostrar QR para accesibilidad de conexion
void estadoDos() {
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


// ESTADO 3: Mostrar creditos de desarrollo
void estadoTres() {
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


// ESTADO 4: Carita Feliz animada para fines decorativos
void estadoCuatro() {
  static unsigned long lastAnim = 0;
  static bool ojosCerrados = false;
  int cx = 160, cy = 90;

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


// SERVIDOR WEB
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