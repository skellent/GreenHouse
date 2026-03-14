/*###################################################################################################
  ### SKELL'S GREENHOUSE V1.0 ### Developed By: Robert Rodríguez "Skellent" & Christopher Ramirez ###
  ###################################################################################################
  #
  #  <Breve texto informativo o descripcion del proyecto>
  #
  ###################################################################################################*/


/*#################################################
  ### IMPORTACION DE LIBRERIAS EXTERNAS/NATIVAS ###
  #################################################*/
#include <SPI.h> // COMUNICACION SERIAL CON LA PANTALLA
#include <WiFi.h> // CONEXION A LA RED
#include <WebServer.h> // SERVIDOR WEB
#include <TFT_eSPI.h> // RENDERIZADO EN PANTALLA
#include <OneWire.h> // NO SE ##############
#include <DallasTemperature.h> // NO SE #############
#include "qrcode_gh.h" // GENERADOR DE QRs
#include "DHT.h" // NO SE ###############


/*######################################
  ### IMPORTACION DE MODULOS PROPIOS ###
  ######################################*/
#include "GUI.h" // DIBUJADO DE INTERFAZ
#include "CONFIG.h" // CONFIGURACIONES BASICAS
#include "MONITOR.h" // HTML DEL SERVIDOR WEB


/*#########################################
  ### CREACION DE INSTANCIAS DE OBJETOS ###
  #########################################*/
#define DHTPIN 16 
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
const int pinDatos = 27;
float temperaturaEsp32 = 0;
OneWire oneWire(pinDatos);
DallasTemperature sensores(&oneWire);
TFT_eSPI tft = TFT_eSPI(); // PANTALLA
WebServer server(red.PUERTO); // SERVIDOR WEB
String ipAddress = red.ESCUCHA;

/*##########################
  ### VARIABLES GLOBALES ###
  ##########################*/
// VARIABLES DE CONTROL DE ESTADO
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


/*##################################
  ### CONFIGURACION DEL PROGRAMA ###
  ##################################*/
void setup() {
  Serial.begin(115200);
  gui.iniciar(); // Inicializacion de la GUI
  xTaskCreatePinnedToCore(tareaServidorWeb, "WebSrv", 10000, NULL, 1, NULL, 0); // DELEGACION DE SERVIDOR WEB A NUCLEO SECUNDARIO
  dht.begin(); // Inicio de sensores
  sensores.begin(); // Inicio de sensores
}


/*####################################
  ### BUCLE PRINCIPAL DEL PROGRAMA ###
  ####################################*/
void loop() {
  actualizarSensores(); // MANTIENE ACTUALIZADO LOS SENSORES POR CADA ITERACION
  manejarTouch(); // GESTIONA LA PARTE TACTIL DE LA PANTALLA
  
  // GESTIÓN DE PANTALLAS ESTÁTICAS (Se dibujan solo una vez al cambiar)
  if (forzarRedibujado) {
    tft.fillScreen(TFT_BLACK); // Limpiamos la pantalla una sola vez aquí
    switch (estado) {
      case 1:
        estadoUno();
        actualizarValoresPantalla(true); // Forzamos los textos iniciales
        break;
      case 2: estadoDos(); break;
      case 3: estadoTres(); break;
      case 4: break; // En el estado 4 no hacemos nada especial al "entrar" 
    }
    forzarRedibujado = false; // ¡El switch terminó! Bajamos la bandera.
  }
  // 2. GESTIÓN DE LÓGICA DINÁMICA (Se ejecuta siempre, sin importar el 'if')
  switch (estado) {
    case 1: actualizarValoresPantalla(false); break; // Actualiza solo si el número cambió 
    case 4: estadoCuatro(); break; // Mantiene la carita parpadeando
  }
  delay(30);
  


  /* CODIGO ORIGINAL DE SKELL
  /*########################
  ### MAQUINA DE ESTADOS ###
  ##########################
  switch (estado) {
    case 1: // ### ESTADO 1 ###: Mostrar los valores de los sensores.
      if (forzarRedibujado) {
        estadoUno();
        actualizarValoresPantalla(true);
        forzarRedibujado = false;
      } else { actualizarValoresPantalla(false); } break;
 
    case 2: // ### ESTADO 2 ###: Mostrar QR para accesibilidad de conexion
      if (forzarRedibujado) { estadoDos(); } forzarRedibujado = false; break;

    case 3: // ### ESTADO 3 ###: Mostrar creditos de desarrollo
      if (forzarRedibujado) { estadoTres(); } forzarRedibujado = false; break;

    case 4: // ### ESTADO 4 ###: Carita Feliz animada para fines decorativos
      estadoCuatro(); break;
  }
  delay(30); 
  */
}

/*#################################
  ### ACTUALIZACION DE SENSORES ###
  #################################*/
void actualizarSensores() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 2000) {
    humedadAire = dht.readHumidity();
    temperatura = dht.readTemperature();
    sensores.requestTemperatures();
    float temperaturaEsp32 = sensores.getTempCByIndex(0);
    humedadTierra = random(100, 900) / 10.0;
    litrosAgua = random(10, 600) / 10.0;
    lastUpdate = millis();
    
  } if(temperaturaEsp32 == DEVICE_DISCONNECTED_C) { Serial.println("Error: Sensor no encontrado"); }
}


/*####################################
  ### SISTEMA DE CONTROL POR TACTO ###
  ####################################*/
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

/*#########################################
  ### ACTUALIZACION GRAFICA DE SENSORES ###
  #########################################*/
void actualizarValoresPantalla(bool forzar) {
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (humedadAire != hA_old || forzar) {
    tft.fillRect(10, 90, 140, 22, TFT_BLACK);
    tft.setCursor(15, 90);
    tft.printf("Air: %.1f%%", humedadAire);
    hA_old = humedadAire;
  }
  if (temperatura != temp_old || forzar) {
    tft.fillRect(173, 90, 140, 25, TFT_BLACK);
    tft.setCursor(180, 90);
    tft.printf("Tmp: %.1fC", temperatura);
    temp_old = temperatura;
  }
  if (humedadTierra != hT_old || forzar) {
    tft.fillRect(10, 210, 140, 25, TFT_BLACK);
    tft.setCursor(15, 210);
    tft.printf("Soil:%.1f%%", humedadTierra);
    hT_old = humedadTierra;
  }
  if (litrosAgua != litros_old || forzar) {
    tft.fillRect(170, 210, 140, 25, TFT_BLACK);
    tft.setCursor(175, 210);
    tft.printf("Wat: %.1fL", litrosAgua);
    litros_old = litrosAgua;
  }
}


/*################
  ### ESTADO 1 ###
  ######################################################
  # ESTE ESTADO SE ENCARGA DE MOSTRAR CON UNA INTERFAZ #
  # SIMPLE LOS VALORES OBTENIDOS POR LOS SENSORES      #
  ######################################################*/
void estadoUno() {
  gui.marco();
  gui.icono.aire();
  gui.icono.temp();
  gui.icono.tierra();
  gui.icono.agua();
}


/*################
  ### ESTADO 2 ###
  ########################################################
  # ESTE ESTADO SE ENCARGA DE MOSTRAR UN CODIGO QR PARA  #
  # ACCEDER AL SERVIDOR WEB QUE SIRVE UN MONITOR CON LOS #
  # DATOS DE LOS SENSORES                                #
  ########################################################*/
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


/*################
  ### ESTADO 3 ###
  #####################################################
  # ESTE ESTADO SE ENCARGA DE MOSTRAR LOS CREDITOS DE #
  # DESARROLLO DE SKELL'S GREENHOUSE                  #
  #####################################################*/
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


/*################
  ### ESTADO 4 ###
  ########################################################
  # ESTE ESTADO SE ENCARGA DE MOSTRAR UNA CARITA ANIMADA #
  # QUE DEPENDIENDO DEL ESTADO DEL INVERNADERO, SE ANIME #
  # EJ: CALOR ALTO = LENTES DE SOL Y SUDOR EN LA CARITA  #
  ########################################################*/
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

/*#############################
  ### SERVIDOR WEB DELEGADO ###
  #############################*/
void tareaServidorWeb(void* p) {
  // Conexión al WIFI
  WiFi.begin(red.NOMBRE, red.CLAVE);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  ipAddress = WiFi.localIP().toString();

  // Servir página web monitor
  server.on("/", []() {
    server.send(200, "text/html", monitor.WEBPAGE);
  });

  /*################################
    ### API DE DATOS DE SENSORES ###
    ################################*/
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