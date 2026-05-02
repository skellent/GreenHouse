/*###############################
  ### SKELL'S GREENHOUSE V2.0 ###
  #####################################
  ###          Developed By:        ###
  ### - Robert Rodríguez "Skellent" ###
  ###    - Christopher Ramirez      ###
  ###     - Fabiana Hernandez       ###
  #####################################*/


/*#################################################
  ### IMPORTACION DE LIBRERIAS EXTERNAS/NATIVAS ###
  #################################################*/
#include <SPI.h> // COMUNICACION SERIAL CON LA PANTALLA
#include <WiFi.h> // CONEXION A LA RED
#include <WebServer.h> // SERVIDOR WEB
#include <TFT_eSPI.h> // RENDERIZADO EN PANTALLA
#include <OneWire.h> // TEMPERATURA ESP32
#include <DallasTemperature.h> // AYUDANTE DE OneWire
#include "qrcode_gh.h" // GENERADOR DE QRs
#include "DHT.h" // // TEMPERATURA Y HUMEDAD DEL AIRE
#include <Preferences.h> // MEMORIA PERSISTENTE


/*######################################
  ### IMPORTACION DE MODULOS PROPIOS ###
  ######################################*/
#include "GUI.h" // DIBUJADO DE INTERFAZ
#include "CONFIG.h" // CONFIGURACIONES BASICAS
#include "MONITOR.h" // HTML DEL SERVIDOR WEB


/*#########################################
  ### CREACION DE INSTANCIAS DE OBJETOS ###
  #########################################*/
DHT dht(pines.dht.PIN, pines.dht.MODEL);
OneWire oneWire(pines.esp32.TEMP);
DallasTemperature sensores(&oneWire);
TFT_eSPI tft = TFT_eSPI(); // PANTALLA
WebServer server(red.PUERTO); // SERVIDOR WEB
Preferences prefs;


/*##########################
  ### VARIABLES GLOBALES ###
  ##########################*/
String ipAddress = red.ESCUCHA;
// VARIABLES DE CONTROL DE ESTADO
volatile int estado = 1;
volatile bool forzarRedibujado = true;
unsigned long ultimoToque = 0;
static unsigned long ultimaActualizacion = 0;
static unsigned long ultimaLuz = 0;
const unsigned long debounceTime = 300;
// VARIABLES DE SENSORES
float humedadAire = 0, temperatura = 0, humedadTierra = 0, litrosAgua = 0, alturaPlanta = 0, temperaturaEsp32 = 0;
// VARIABLES PARA LUCES UV
float tiempoEncendido = pines.ultravioleta.TIME; 
unsigned long tiempoReferenciaLuz = 0;
bool lucesActivas = false;
// VARIABLES PARA EL TANQUE DE AGUA
float rangoHumedad = calibracion.riego.RANGO;
float uvON  = pines.ultravioleta.ON;
// VARIABLE DE PRESENTACION
bool presentar = true;

/*##################################
  ### CONFIGURACION DEL PROGRAMA ###
  ##################################*/
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n*********************************");
  Serial.println(">>> GREENHOUSE DEBUG START <<<");
  Serial.println("*********************************");

  Serial.println("Iniciando Pantalla... ");
  gui.iniciar(); 

  Serial.println("Iniciando Sensores y Pines... ");

  dht.begin(); // DHT22 (Temperatura y Humedad Ambiental)
  sensores.begin(); // ESP32 Temperatura

  pinMode(pines.ultrasonido.TRIG, OUTPUT); pinMode(pines.ultrasonido.ECHO, INPUT); // Ultrasonido (Altura de la Planta)

  // Configurar Canal para UV-A
  ledcSetup(pines.ultravioleta.A.CAN, pines.ultravioleta.FRQ, pines.ultravioleta.RES);
  ledcAttachPin(pines.ultravioleta.A.PIN, pines.ultravioleta.A.CAN);
  ledcWrite(pines.ultravioleta.A.CAN, pines.ultravioleta.OFF); // Apagada

  // Configurar Canal para UV-B
  ledcSetup(pines.ultravioleta.B.CAN, pines.ultravioleta.FRQ, pines.ultravioleta.RES);
  ledcAttachPin(pines.ultravioleta.B.PIN, pines.ultravioleta.B.CAN);
  ledcWrite(pines.ultravioleta.B.CAN, pines.ultravioleta.OFF); // Apagada
  
  // Bomba de Agua
  ledcSetup(pines.bomba.CAN, pines.bomba.FRQ, pines.bomba.RES);
  ledcAttachPin(pines.bomba.PIN, pines.bomba.CAN);
  ledcWrite(pines.bomba.CAN, pines.bomba.OFF); // Encendida

  Serial.println("Lanzando WebServer en Core 0... ");
  xTaskCreatePinnedToCore(ServidorWeb, "WebSrv", 10000, NULL, 1, NULL, 0); 

  // Memoria Editable y Luces UV y Humedad
  prefs.begin("gh-settings", true);
  // Si no existe "uTime", usa el valor por defecto de CONFIG.h
  tiempoEncendido = prefs.getFloat("uTime", pines.ultravioleta.TIME);
  rangoHumedad    = prefs.getFloat("uHum", calibracion.riego.RANGO);
  uvON            = prefs.getFloat("uvON", pines.ultravioleta.ON);
  prefs.end();
  tiempoReferenciaLuz = millis(); // Iniciar cronómetro de luces
}


/*####################################
  ### BUCLE PRINCIPAL DEL PROGRAMA ###
  ####################################*/
void loop() {
  if (forzarRedibujado) {
    tft.fillScreen(TFT_BLACK);
    switch (estado) {
      case 1: estadoUno();break;
      case 2: estadoCuatro(); break;
      case 3: estadoDos(); break;
      case 4: estadoTres(); break;
      //case 5: estadoCinco(); break;
      //case 6: estadoSeis(); break;
    }
    forzarRedibujado = false;
  }

  if (presentar) {
    presentacion();
    presentar = false;
  }

  actualizarSensores(); // MANTIENE ACTUALIZADO LOS SENSORES POR CADA ITERACION
  manejarTouch(); // GESTIONA LA PARTE TACTIL DE LA PANTALLA
  ejecutarCuidado(); // CUIDADO AUTONOMO

  if (estado == 1 && (millis() - ultimaActualizacion) > 1000) { // SE ENCARGA DE ACTUALIZAR LOS VALORES PERIODICAMENTE
    actualizarValoresPantalla();
    ultimaActualizacion = millis();
  }
}

void ejecutarCuidado() {
  // AGUA
  if (humedadTierra < rangoHumedad && litrosAgua > 40) {
    ledcWrite(pines.bomba.CAN, pines.bomba.ON);
    ledcWrite(pines.ultravioleta.A.CAN, pines.ultravioleta.OFF);
    ledcWrite(pines.ultravioleta.B.CAN, pines.ultravioleta.OFF);
  } else {
    ledcWrite(pines.bomba.CAN, pines.bomba.OFF);

    // LUZ UV
    unsigned long tiempoActual = millis();
    unsigned long tiempoTranscurrido = tiempoActual - tiempoReferenciaLuz;
    unsigned long cicloTotal = tiempoEncendido * 3;  // 1 parte encendido, 2 partes apagado

    if (tiempoTranscurrido < tiempoEncendido) { // FASE DE ENCENDIDO
      ledcWrite(pines.ultravioleta.A.CAN, uvON);
      ledcWrite(pines.ultravioleta.B.CAN, uvON);
    } 
    else if (tiempoTranscurrido < cicloTotal) { // FASE DE APAGADO (Dura el doble)
      ledcWrite(pines.ultravioleta.A.CAN, pines.ultravioleta.OFF);
      ledcWrite(pines.ultravioleta.B.CAN, pines.ultravioleta.OFF);
    } 
    else { tiempoReferenciaLuz = tiempoActual; } // REINICIO DEL CICLO
  }
}

float leerUltrasonido() {
  digitalWrite(pines.ultrasonido.TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(pines.ultrasonido.TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(pines.ultrasonido.TRIG, LOW);
  // pulseIn mide el tiempo en microsegundos que el pin ECHO está en HIGH
  long duracion = pulseIn(pines.ultrasonido.ECHO, HIGH, 30000); // timeout de 30ms
  // Convertir tiempo a distancia: Distancia = (Tiempo * Velocidad del sonido) / 2
  float distancia = duracion / 58.2;   // En cm: duracion / 58.2
  if (distancia <= 0 || distancia > 400) return 0; // Error o fuera de rango
  return distancia;
}

/*#################################
  ### ACTUALIZACION DE SENSORES ###
  #################################*/
void actualizarSensores() {
  static unsigned long ultimaActualizacionSensores = 0;
  float tempHumedadAire = 0;
  float tempTemperatura = 0;
  if (millis() - ultimaActualizacionSensores > 2000) {
    tempHumedadAire = dht.readHumidity();
    tempTemperatura = dht.readTemperature();
    if ( !std::isnan(tempHumedadAire) ) { humedadAire = tempHumedadAire; } // HUMEDAD DEL AIRE
    if ( !std::isnan(tempTemperatura) ) { temperatura = tempTemperatura; } // TEMPERATURA AMBIENTE

    // TEMPERATURA DEL ESP32 (POR SEGURIDAD)
    sensores.requestTemperatures(); 
    temperaturaEsp32 = sensores.getTempCByIndex(0);

    // HUMEDAD DE LA TIERRA
    humedadTierra = constrain(map(analogRead(pines.agua.TIERRA), calibracion.tierra.SECO, calibracion.tierra.MOJA, 0, 100), 0, 100);
    if (humedadTierra > 100) { humedadTierra = 100; } 

    // LITROS DE AGUA DISPONIBLES EN TANQUE
    int lecturaTanque = analogRead(pines.agua.TANQUE);
    litrosAgua = map(lecturaTanque, calibracion.tanque.SECO, calibracion.tanque.MOJA, 0, 100);
    litrosAgua = constrain(litrosAgua, 0, 100);  // Mejor que el if manual

    // ALTURA DE LA PLANTA (ULTRASONIDO)
    alturaPlanta = leerUltrasonido(); 
    Serial.printf("LOG | Hum: %.1f%% | Temp: %.1fC | S3-Temp: %.1fC | Soil: %.1f | Water: %.1f | H: %.1f cm\n", humedadAire, temperatura, temperaturaEsp32, humedadTierra, litrosAgua, alturaPlanta);
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
      estado = (estado % 4) + 1; // remplazar por estado % 6 para usar estados de Step
      forzarRedibujado = true;
      ultimoToque = millis();
    printf("Pantalla Tocada!");
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
  tft.printf("Wat: %.0f%%", litrosAgua);
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

/*################
  ### ESTADO 5 ###
  ########################################################
  # HACE GIRAR EL MOTOR PASO A PASO A SENTIDO DEL RELOJ  #
  ########################################################*/
void estadoCinco() {
  gui.icono.ganchoDerecha();
}

/*################
  ### ESTADO 6 ###
  ########################################################
  # HACE GIRAR EL MOTOR PASO A PASO A SENTIDO DEL RELOJ  #
  ########################################################*/
void estadoSeis() {
  gui.icono.ganchoIzquierda();
}

/*#####################################
### PRESENTACION DE FUNCIONALIDADES ###
#####################################*/
void presentacion() {
  estado = 2;
  forzarRedibujado = true;
  ultimoToque = millis();
  for (int i = 0; i < 4; i++) {
    for (int i = 256; i > 0; i--) {
      ledcWrite(pines.ultravioleta.A.CAN, i);
      ledcWrite(pines.ultravioleta.B.CAN, i);
      delay(10);
    }
    for (int i = 0; i < 256; i++) {
      ledcWrite(pines.ultravioleta.A.CAN, i);
      ledcWrite(pines.ultravioleta.B.CAN, i);
      delay(10);
    }
  }
  for (int i = 0; i < 4; i++) {
    ledcWrite(pines.bomba.CAN, pines.bomba.ON);
    delay(1000);
    ledcWrite(pines.bomba.CAN, pines.bomba.OFF);
    delay(1000);
  }
  estado = 1;
  forzarRedibujado = true;
  ultimoToque = millis();
}

// Función para limpiar valores NaN antes de enviarlos por JSON
String validarDato(float valor) { if (isnan(valor)) {return "0.0"; } return String(valor, 1); }


/*#############################
  ### SERVIDOR WEB DELEGADO ###
  #############################*/
void ServidorWeb(void* p) {
  // Conexión al WIFI
  WiFi.begin(red.NOMBRE, red.CLAVE);
  while (WiFi.status() != WL_CONNECTED) vTaskDelay(pdMS_TO_TICKS(500));
  ipAddress = WiFi.localIP().toString();

  // Servir página web monitor
  server.on("/", []() {
    server.send(200, "text/html", monitor.WEBPAGE);
  });

  // Modificar Configuracion de Luces
  server.on("/config", []() {
    if (server.hasArg("time")) {
      float nuevoTime = server.arg("time").toFloat();
      if (nuevoTime > 0) {
        tiempoEncendido = nuevoTime;
        prefs.begin("gh-settings", false);
        prefs.putFloat("uTime", tiempoEncendido);
        prefs.end();
        tiempoReferenciaLuz = millis();
        server.send(200, "text/plain", "TIEMPO DE ILUMINACIÓN MODIFICADO EXITOSAMENTE");
      }
    }

    if (server.hasArg("hum")) {
      float nuevoHum = server.arg("hum").toFloat();
      if (nuevoHum >= 0 && nuevoHum <= 100) {
        rangoHumedad = nuevoHum;
        prefs.begin("gh-settings", false);
        prefs.putFloat("uHum", rangoHumedad);
        prefs.end();
        server.send(200, "text/plain", "UMBRAL DE HUMEDAD MODIFICADO EXITOSAMENTE");
      } else { server.send(400, "text/plain", "Valor fuera de rango (0-100)"); }
    }

    if (server.hasArg("uv")) {
      float nuevoUVon = server.arg("uv").toFloat();
      if (nuevoUVon >= 0 && nuevoUVon <= 255) {
        uvON = nuevoUVon;
        prefs.begin("gh-settings", false);
        prefs.putFloat("uvON", uvON);
        prefs.end();
        server.send(200, "text/plain", "POTENCIA DE LA LUZ UV MODIFICADA EXITOSAMENTE");
      } else { server.send(400, "text/plain", "Valor fuera de rango (0-255)"); }
    }

  });

  // API de Datos de Sensores
  server.on("/data", []() {
    String json;
    json.reserve(150);
    json = "{";
    json += "\"ha\":" + validarDato(humedadAire) + ",";
    json += "\"te\":" + validarDato(temperatura) + ",";
    json += "\"ht\":" + validarDato(humedadTierra) + ",";
    json += "\"li\":" + validarDato(litrosAgua) + ",";
    json += "\"ap\":" + validarDato(alturaPlanta);
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();

  for (;;) {
    server.handleClient(); // Atiende peticiones
    vTaskDelay(pdMS_TO_TICKS(10)); // <--- VITAL: Libera el núcleo por 10ms
  }
}
