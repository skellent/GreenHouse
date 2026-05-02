/*#####################################
  ###    SKELL'S GREENHOUSE V2.0    ###
  #####################################
  ###          Developed By:        ###
  ### - Robert Rodríguez "Skellent" ###
  ###    - Christopher Ramirez      ###
  ###     - Fabiana Hernandez       ###
  #####################################*/


/*#################################################
  ### IMPORTACION DE LIBRERIAS EXTERNAS/NATIVAS ###
  #################################################*/
#include <WiFi.h>                 // CONEXION A LA RED
#include <WebServer.h>           // SERVIDOR WEB
#include <OneWire.h>            // TEMPERATURA ESP32
#include <DallasTemperature.h> // AYUDANTE DE OneWire
#include "DHT.h"              // TEMPERATURA Y HUMEDAD DEL AIRE
#include <Preferences.h>     // MEMORIA PERSISTENTE


/*###################################
### IMPORTACION DE TENSORFLOW LITE###
#####################################*/
#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"


/*######################################
  ### IMPORTACION DE MODULOS PROPIOS ###
  ######################################*/
#include "CONFIG.h"         // CONFIGURACIONES BASICAS
#include "skells_model.h" // RED NEURONAL


/*#########################################
  ### CREACION DE INSTANCIAS DE OBJETOS ###
  #########################################*/
DHT dht(pines.dht.PIN, pines.dht.MODEL); // TEMPERATURA AMBIENTE Y HUMEDAD DEL AIRE
OneWire oneWire(pines.esp32.TEMP);      // TEMPERATURA DEL ESP32
DallasTemperature sensores(&oneWire);  // complemento de Arriba
WebServer server(red.PUERTO);         // SERVIDOR WEB
Preferences prefs;                   // MEMORIA PERSISTENTE


/*##################################################################
### MONTURA DE PUNTEROS Y MEMORIA RESERVADA PARA LA RED NEURONAL ###
####################################################################*/
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;
TfLiteTensor* output = nullptr;
// MEMORIA RESERVADA
constexpr int kTensorArenaSize = 16 * 1024; 
alignas(16) uint8_t tensor_arena[kTensorArenaSize];

/*##########################
  ### VARIABLES GLOBALES ###
  ##########################*/
String ipAddress = red.ESCUCHA;
// VARIABLES DE SENSORES
float sen_humedadAire      = 0.0;
float sen_humedadTierra    = 0.0;
float sen_litrosAgua       = 0.0;
float sen_alturaPlanta     = 0.0;
float sen_temperatura      = 0.0;
float sen_temperaturaEsp32 = 0.0;
float sen_temperaturaAgua  = 0.0;
float sen_luz              = 0.0;
// VARIABLES DE LA RED NEURONAL
float uv = 0.0;
float bomba = 0.0;
// ------------------------------
float raw_humedadAire      = 0.0;
float raw_humedadTierra    = 0.0;
float raw_litrosAgua       = 0.0;
float raw_alturaPlanta     = 0.0;
float raw_temperatura      = 0.0;
float raw_temperaturaEsp32 = 0.0;
float raw_temperaturaAgua  = 0.0;
float raw_luz              = 0.0;


/*##################################
  ### CONFIGURACION DEL PROGRAMA ###
  ##################################*/
void setup() {
  Serial.begin(115200);
  delay(1000);


  /* ### ACTIVACION DE SENSORES */
  dht.begin();       // TEMPERATURA AMBIENTE Y HUMEDAD DEL AIRE
  sensores.begin(); // TEMPERATURA DEL ESP32
  pinMode(pines.ultrasonido.TRIG, OUTPUT);
  pinMode(pines.ultrasonido.ECHO, INPUT); // ALTURA DE LA PLANTA


  /* ### ACTIVACION DE ACTUADORES */
  // Configurar Canal para UV-A
  ledcSetup(pines.ultravioleta.A.CAN, pines.ultravioleta.FRQ, pines.ultravioleta.RES);
  ledcAttachPin(pines.ultravioleta.A.PIN, pines.ultravioleta.A.CAN);
  ledcWrite(pines.ultravioleta.A.CAN, pines.ultravioleta.OFF);
  // Configurar Canal para UV-B
  ledcSetup(pines.ultravioleta.B.CAN, pines.ultravioleta.FRQ, pines.ultravioleta.RES);
  ledcAttachPin(pines.ultravioleta.B.PIN, pines.ultravioleta.B.CAN);
  ledcWrite(pines.ultravioleta.B.CAN, pines.ultravioleta.OFF);
  // Bomba de Agua
  ledcSetup(pines.bomba.CAN, pines.bomba.FRQ, pines.bomba.RES);
  ledcAttachPin(pines.bomba.PIN, pines.bomba.CAN);
  ledcWrite(pines.bomba.CAN, pines.bomba.OFF); // Encendida


  /* ### ASIGNACION DE PUNTEROS DE LA RED NEURONAL ### */
  static tflite::MicroErrorReporter micro_error_reporter; // Depurador de Errores
  error_reporter = &micro_error_reporter;                //

  model = tflite::GetModel(skells_model);               // Red Neuronal Casera

  static tflite::AllOpsResolver resolver;              // Operaciones Matematicas Complejas
  static tflite::MicroInterpreter static_interpreter( // Carga las Operaciones a la Memoria
    model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  interpreter->AllocateTensors();    // CONTRUYE EL INTERPRETE (LA RED NEURONAL)
  input = interpreter->input(0);    // Distribucion de Inputs en Memoria
  output = interpreter->output(0); // Distribucion de Outputs en Memoria


  /* ### DELEGACION DE SERVIDOR WEB A NUCLEO SECUNDARIO ###*/
  xTaskCreatePinnedToCore(ServidorWeb, "WebSrv", 10000, NULL, 1, NULL, 0); // SERVIDOR WEB
}


/*####################################
  ### BUCLE PRINCIPAL DEL PROGRAMA ###
  ####################################*/
void loop() {
  input = interpreter->input(0);
  output = interpreter->output(0);

  actualizarSensores(); // MANTIENE ACTUALIZADO LOS SENSORES POR CADA ITERACION
  formatearSensores();

  input->data.f[0] = raw_temperatura;
  input->data.f[1] = raw_humedadAire;
  input->data.f[2] = raw_humedadTierra;
  input->data.f[3] = raw_luz;
  input->data.f[4] = raw_litrosAgua;
  
  if (interpreter->Invoke() == kTfLiteOk) {
    neopixelWrite(48, random(0, 255), random(0, 255), random(0, 255));
    bomba = output->data.f[0]; 
    uv    = output->data.f[1];
  } else { Serial.println("❌ ERROR CRÍTICO: La IA falló al procesar los datos (Falta de Memoria o Tensor corrupto)."); }

  log(true);

  delay(1000);
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
  float tempHumedadAire = 0;
  float tempTemperatura = 0;

  tempHumedadAire = dht.readHumidity();
  tempTemperatura = dht.readTemperature();
  if ( !std::isnan(tempHumedadAire) ) { sen_humedadAire = tempHumedadAire; } // HUMEDAD DEL AIRE
  if ( !std::isnan(tempTemperatura) ) { sen_temperatura = tempTemperatura; } // TEMPERATURA AMBIENTE

  // TEMPERATURA DEL ESP32 (POR SEGURIDAD)
  sensores.requestTemperatures(); 
  sen_temperaturaEsp32 = sensores.getTempCByIndex(0);

  // HUMEDAD DE LA TIERRA
  sen_humedadTierra = constrain(map(analogRead(pines.agua.TIERRA), calibracion.tierra.SECO, calibracion.tierra.MOJA, 0, 100), 0, 100);
  if (sen_humedadTierra > 100) { sen_humedadTierra = 100; } 

  // LITROS DE AGUA DISPONIBLES EN TANQUE
  int lecturaTanque = analogRead(pines.agua.TANQUE);
  sen_litrosAgua = map(lecturaTanque, calibracion.tanque.SECO, calibracion.tanque.MOJA, 0, 100);
  sen_litrosAgua = constrain(sen_litrosAgua, 0, 100);  // Mejor que el if manual

  // ALTURA DE LA PLANTA (ULTRASONIDO)
  sen_alturaPlanta = leerUltrasonido();

  // ILUMINACION INDIRECTA
  sen_luz = (float)(analogRead(pines.fotoresistencia.PIN));
}

// PARA PRUEBAS
/*void actualizarSensores() {
  sen_humedadAire     = random(60, 90);
  sen_humedadTierra   = random(0, 4095); 
  sen_temperatura     = random(18, 30);
  sen_temperaturaAgua = random(18, 30);
  sen_temperaturaEsp32 = random(18, 30);
  sen_litrosAgua       = random(0, 4095);
  sen_alturaPlanta     = random(0, 30);
  sen_luz              = random(0, 4095);
}*/


void formatearSensores() {
  /* FORMATEO REAL */
  raw_temperatura   = (sen_temperatura - 0.0) / (50.0 - 0.0);       // Temp (0 a 50)
  raw_humedadAire   = (sen_humedadAire - 0.0) / (100.0 - 0.0);     // Hum Aire (0 a 100)
  raw_humedadTierra = (sen_humedadTierra - 0.0) / (100.0 - 0.0); // Hum Suelo (0 a 100)
  raw_luz           = (sen_luz - 0.0) / (4095.0 - 0.0);          // Luz (0 a 4095)
  raw_litrosAgua    = (sen_litrosAgua - 0.0) / (100.0 - 0.0);  // Tanque (0 a 100)
}


void log(bool activo) {
  if (activo) {
    Serial.println("#[SKELL-LOG]> Sensores");
    Serial.printf( " - HUM. TIERRA:    %.1f%%\n", sen_humedadTierra);
    Serial.printf( " - HUM. AIRE:      %.1f%%\n", sen_humedadAire);
    Serial.printf( " - TEMP. AMBIENTE: %.1fC\n", sen_temperatura);
    Serial.printf( " - TEMP. TANQUE:   %.1fC\n", sen_temperaturaAgua);
    Serial.printf( " - NIVEL TANQUE:   %.1f%%\n", sen_litrosAgua);
    Serial.printf( " - ALTURA PLANT:   %.1f cm\n", sen_alturaPlanta);
    Serial.printf( " - LUZ:            %.1f%\n", sen_luz);
    Serial.println("-------------------------");
    Serial.println("#[SKELL-LOG]> Conexiones");
    Serial.printf( " - IP DE ACCESO:   %s\n", ipAddress.c_str()); 
    Serial.println("-------------------------");
    Serial.println("#[SKELL-LOG]> Pensamiento de la IA");
    Serial.printf( " - TIPO DE INPUT: %d\n", input->type);
    Serial.printf( " - UV:      %.4f\n", uv);
    Serial.printf( " - Bomba:   %.4f\n", bomba);
    Serial.println("-------------------------");
    Serial.printf(" - Tensor In[0] (Temp): %.4f\n", input->data.f[0]);
    Serial.printf(" - Tensor In[1] (HumA): %.4f\n", input->data.f[1]);
    Serial.printf(" - Tensor In[2] (HumT): %.4f\n", input->data.f[2]);
    Serial.printf(" - Tensor In[3] (Luz) : %.4f\n", input->data.f[3]);
    Serial.printf(" - Tensor In[4] (Tanq): %.4f\n", input->data.f[4]);
    Serial.println("-------------------------");
    Serial.println("#[SKELL-LOG]> ESTADO DEL ESP32");
    Serial.printf( " - TEMP. ESP32: %.1fC\n", sen_temperaturaEsp32);
    Serial.printf( " - SRAM INTERNA:  %.0f / %.0f KB \n", ESP.getFreeHeap() / 1024.0, ESP.getHeapSize() / 1024.0);
    if (psramFound()) { Serial.printf( " - PSRAM EXTERNA: %.0f / %.0f KB \n", ESP.getFreePsram() / 1024.0, ESP.getPsramSize() / 1024.0);
    } else { Serial.println( " - PSRAM EXTERNA: NO DETECTADA (Revisa la config OPI/QSPI)"); }
    Serial.println("");
  }
}
// Función para limpiar valores NaN antes de enviarlos por JSON
String validarDato(float valor) { if (isnan(valor)) {return "0.0"; } return String(valor, 1); }

/*#############################
  ### SERVIDOR WEB DELEGADO ###
  #############################*/
void ServidorWeb(void* p) {
  WiFi.begin(red.NOMBRE, red.CLAVE);
  while (WiFi.status() != WL_CONNECTED) vTaskDelay(pdMS_TO_TICKS(500));
  ipAddress = WiFi.localIP().toString();

  server.on("/data", []() {
    String json;
    json.reserve(150);
    json = "{";
    json += "\"humaire\":"   + validarDato(sen_humedadAire)      + ",";
    json += "\"tempambi\":"  + validarDato(sen_temperatura)      + ",";
    json += "\"tempesp\":"   + validarDato(sen_temperaturaEsp32) + ",";
    json += "\"tempagua\":"  + validarDato(sen_temperaturaAgua)  + ",";
    json += "\"humtierra\":" + validarDato(sen_humedadTierra)    + ",";
    json += "\"litagua\":"   + validarDato(sen_litrosAgua)       + ",";
    json += "\"luz\":"       + validarDato(sen_luz)              + ",";
    json += "\"alturpl\":"   + validarDato(sen_alturaPlanta)     + ",";
    json += "\"uv\":"        + validarDato(uv)     + ",";
    json += "\"bomba\":"     + validarDato(bomba);
    json += "}";
    server.send(200, "application/json", json);
  });

  server.begin();

  for (;;) {
    server.handleClient();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
