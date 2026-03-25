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


/*############################
### CONFIGURACION DE PINES ###
##############################*/
// ULTRASONIDO
#define TRIG_PIN 12
#define ECHO_PIN 14
// DHT11
#define DHTPIN 16 
#define DHTTYPE DHT11
// HUMEDAD-TIERRA
#define SOIL_PIN 4

/*#########################################
  ### CREACION DE INSTANCIAS DE OBJETOS ###
  #########################################*/

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
static unsigned long ultimaActualizacion = 0;
const unsigned long debounceTime = 300;
// VARIABLES DE SENSORES
float humedadAire = 0, temperatura = 0, humedadTierra = 0, litrosAgua = 0, alturaPlanta = 0;


/*##################################
  ### CONFIGURACION DEL PROGRAMA ###
  ##################################*/
void setup() {
  // 1. Inicializar Serial inmediatamente
  Serial.begin(115200);
  
  // 2. Espera real y visual para sincronizar el USB del PC
  delay(1000); 
  Serial.println("\n\n*********************************");
  Serial.println(">>> GREENHOUSE DEBUG START <<<");
  Serial.println("*********************************");

  // 3. Inicializar componentes uno por uno con mensajes de progreso
  Serial.print("Iniciando Pantalla... ");
  gui.iniciar(); 
  Serial.println("OK");

  Serial.print("Iniciando Sensores... ");
  dht.begin(); 
  sensores.begin();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("OK");

  Serial.print("Lanzando WebServer en Core 0... ");
  // Aumentamos el stack de la tarea por si acaso
  xTaskCreatePinnedToCore(ServidorWeb, "WebSrv", 10000, NULL, 1, NULL, 0); 
  Serial.println("OK");

  Serial.println(">>> TODO INICIALIZADO CORRECTAMENTE <<<");
}


/*####################################
  ### BUCLE PRINCIPAL DEL PROGRAMA ###
  ####################################*/
void loop() {
  actualizarSensores(); // MANTIENE ACTUALIZADO LOS SENSORES POR CADA ITERACION
  manejarTouch(); // GESTIONA LA PARTE TACTIL DE LA PANTALLA
  // ejecutarCuidado(); GESTIONA LA PARTE DE CUIDAR FISICAMENTE LA PLANTA
  
  if (forzarRedibujado) {
    tft.fillScreen(TFT_BLACK); // Limpiamos la pantalla una sola vez aquí
    switch (estado) {
      case 1: estadoUno();break;
      case 2: estadoDos(); break;
      case 3: estadoTres(); break;
      case 4: estadoCuatro(); break; // En el estado 4 no hacemos nada especial al "entrar" 
    }
    forzarRedibujado = false;
  }

  if (estado == 1 && (millis() - ultimaActualizacion) > 2000) { // SE ENCARGA DE ACTUALIZAR LOS VALORES PERIODICAMENTE
    actualizarValoresPantalla();
    ultimaActualizacion = millis();
  }

  delay(25);
}


float leerUltrasonido() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // pulseIn mide el tiempo en microsegundos que el pin ECHO está en HIGH
  long duracion = pulseIn(ECHO_PIN, HIGH, 30000); // timeout de 30ms
  // Convertir tiempo a distancia: Distancia = (Tiempo * Velocidad del sonido) / 2
  // En cm: duracion / 58.2
  float distancia = duracion / 58.2;
  if (distancia <= 0 || distancia > 400) return 0; // Error o fuera de rango
  return distancia;
}


/*#################################
  ### ACTUALIZACION DE SENSORES ###
  #################################*/
void actualizarSensores() {
  static unsigned long ultimaActualizacionSensores = 0;
  if (millis() - ultimaActualizacionSensores > 2000) {
    // HUMEDAD DEL AIRE
    humedadAire = dht.readHumidity();
    // TEMPERATURA AMBIENTE
    temperatura = dht.readTemperature();
    // TEMPERATURA DEL ESP32 (POR SEGURIDAD)
    sensores.requestTemperatures();
    float temperaturaEsp32 = sensores.getTempCByIndex(0);
    // HUMEDAD DE LA TIERRA
    humedadTierra = 
    // LITROS DE AGUA DISPONIBLES EN TANQUE
    litrosAgua = 0;
    // ALTURA DE LA PLANTA (ULTRASONIDO)
    alturaPlanta = leerUltrasonido();

  Serial.printf("LOG | Hum: %.1f%% | Temp: %.1fC | S3-Temp: %.1fC | Soil: %.1f | H: %.1f cm\n", humedadAire, temperatura, temperaturaEsp32, humedadTierra, alturaPlanta);
    ultimaActualizacionSensores = millis();    
  }
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
    }
  }
}

/*#########################################
  ### ACTUALIZACION GRAFICA DE SENSORES ###
  #########################################*/
void actualizarValoresPantalla() {
  // CONFIGURACION
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // HUMEDAD DEL AIRE
  tft.fillRect(10, 90, 140, 22, TFT_BLACK);
  tft.setCursor(15, 90);
  tft.printf("Air: %.0f%%", humedadAire);
  // TEMPERATURA AMBIENTE
  tft.fillRect(173, 90, 140, 25, TFT_BLACK);
  tft.setCursor(180, 90);
  tft.printf("Tmp: %.0fC", temperatura);
  // HUMEDAD DE LA TIERRA
  tft.fillRect(10, 210, 140, 25, TFT_BLACK);
  tft.setCursor(15, 210);
  tft.printf("Soil: %.0f%%", humedadTierra);
  // LITROS DE AGUA DISPONIBLE
  tft.fillRect(170, 210, 140, 25, TFT_BLACK);
  tft.setCursor(175, 210);
  tft.printf("Wat: %.1fL", litrosAgua);
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
  actualizarValoresPantalla();
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
  // CONFIGURACIONES DEL QR
  uint8_t qrcodeData[qrcode_getBufferSize(3)]; // EN LA MEMORIA
  qrcode_initText(&qrcode, qrcodeData, 3, 0, ("http://" + ipAddress).c_str()); // CON LA IP
  int tam = 6; // TAMAÑO; Aumentado a 6 para máxima visibilidad
  int offX = (320 - qrcode.size * tam) / 2; // POSICION X
  int offY = (240 - qrcode.size * tam) / 2 - 15; // POSICION Y
  // DIBUJA FILA POR FILA EL CODIGO QR
  for (int y = 0; y < qrcode.size; y++) {
    for (int x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) tft.fillRect(offX + x*tam, offY + y*tam, tam, tam, TFT_WHITE);
    }
  }
  // ESCRIBE LA IP PARA ACCESO MANUAL O USO DE LA API
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
  tft.setCursor(10, 80); tft.println("V1.0 - ESP32-S3");
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setCursor(10, 140); tft.println("Robert Rodriguez");
  tft.setCursor(10, 170); tft.println("Christopher Ramirez");
  tft.setCursor(10, 200); tft.println("Fabiana Hernandez");
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
  int x = 160, y = 90;
  tft.fillRect(x - 80, y - 40, 160, 50, TFT_BLACK);
  tft.fillCircle(x - 50, y - 15, 18, TFT_WHITE); // Ojo L XL
  tft.fillCircle(x + 50, y - 15, 18, TFT_WHITE); // Ojo R XL
  tft.drawArc(x, y + 30, 70, 60, 270, 90, TFT_WHITE, TFT_BLACK, true); // Sonrisa XL
}


// Función para limpiar valores NaN antes de enviarlos por JSON
String validarDato(float valor) { if (isnan(valor)) {return "0.0"; } return String(valor, 1); }


/*#############################
  ### SERVIDOR WEB DELEGADO ###
  #############################*/
void ServidorWeb(void* p) {
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