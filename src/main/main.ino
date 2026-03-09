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


// IMPORTACION DE MODULOS PROPIOS
#include "GUI.h"
#include "CONFIG.h"
#include "MONITOR.h"


// TEMPERATURA AMBIENTE
#define DHTPIN 16 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);


// TEMPERATURA ESP32
const int pinDatos = 27;
OneWire oneWire(pinDatos);
DallasTemperature sensores(&oneWire);
float temperaturaEsp32 = 0;


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

  // Inicializacion de la GUI
  gui.iniciar();

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
  

  // ### MAQUINA DE ESTADOS ###
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


// ESTADO 1: Mostrar los valores de los sensores.
void estadoUno() {
  gui.marco();
  gui.icono.aire();
  gui.icono.temp();
  gui.icono.tierra();
  gui.icono.agua();
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

// Función para limpiar valores NaN antes de enviarlos por JSON
String validarDato(float valor) { if (isnan(valor)) {return "0.0"; } return String(valor, 1); }

// SERVIDOR WEB
void tareaServidorWeb(void* p) {
  // Conexión al WIFI
  WiFi.begin(red.NOMBRE, red.CLAVE);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  ipAddress = WiFi.localIP().toString();

  // Servir página web monitor
  server.on("/", []() {
    server.send(200, "text/html", monitor.WEBPAGE);
  });

  // API para poder usar los datos en otras aplicaciones/sistemas
  server.on("/data", []() {
    String json = "{";
    json += "\"ha\":" + validarDato(humedadAire) + ",";
    json += "\"te\":" + validarDato(temperatura) + ",";
    json += "\"ht\":" + validarDato(humedadTierra) + ",";
    json += "\"li\":" + validarDato(litrosAgua);
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();
  while (true) { server.handleClient(); delay(10); }
}