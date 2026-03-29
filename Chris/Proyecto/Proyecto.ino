//Bibliotecas
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>

//Pines
#define DHTPIN 22
const int Sensor_Tierra = 34; 
const int Sensor_Temp_ESP32 = 23; //sensores de temperatura

//Extras

  //DHT:
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
  //DS18B20 (temperatura del sistema)
OneWire oneWire(Sensor_Temp_ESP32);
DallasTemperature sensors(&oneWire);


//Variables globales
float Humedad_ambiental = 0;

float Temperatura_ambiental = 0;

int Humedad_Tierra = 0;

float temperatura_ESP32 = 0;
//leds UV
const int ledPin1 = 18; 
const int ledPin2 = 19; 
int brillo1 = 0;    


//Inicio
void setup() {
  Serial.begin(115200);

//Sensor DHT
  Serial.println("Iniciando sensor DHT (tmp y Hmd)");
  dht.begin();
  delay(1000);
//Sensor de temperatura DS18B20
  sensors.begin();

//configuracion de pines para leds UV
  ledcAttach(ledPin1, 20000, 8);
  ledcAttach(ledPin2, 20000, 8);
  //         /     /    /
  //Cofig:  Pin  freq  resolution
}
void loop() {
//control de brillo 

  ledcWrite(ledPin1, brillo1);
  ledcWrite(ledPin2, brillo2);
  


//Lectura del sensor DHT
  float Humedad_ambiental = dht.readHumidity();

  float Temperatura_ambiental = dht.readTemperature();

  //Verificacion del correcto funcinamiento del sensor DHT
  if (isnan(Humedad_ambiental) || isnan(Temperatura_ambiental)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(5000);
    return;
  }

//Lectura del sensor de humedad de tierra
  int Humedad_Tierra = analogRead(Sensor_Tierra);


//Lectura de DS18B20 (temperatura del sistema)
  sensors.requestTemperatures();
  float temperatura_ESP32 = sensors.getTempCByIndex(0);

  //Verificacion del correcto funcinamiento del sensor DHT
  if(temperatura_ESP32 == DEVICE_DISCONNECTED_C) {
    Serial.println("ERROR al leer el sensor de temperatura");
    return;
  }


  if(temperatura_ESP32 >= 60.00){
    Serial.print("Apagando sistema por sobrecalentamiento");


  }


//Impresion de los valores DHT
  Serial.print("Temperatura ambiental: ");
  Serial.print(Temperatura_ambiental);
  Serial.println("°C");
  Serial.print("Humedad ambiental: ");
  Serial.print(Humedad_ambiental);
  Serial.println("%");
  
//Impresion de los valores Humedad de tierra
  Serial.print("Humedad de la tierra: ");
  Serial.print(Humedad_Tierra);
  Serial.println("%");

//Impresion de la temperatura de la ESP32
  Serial.print("Temperatura de la ESP32: ");
  Serial.print(temperatura_ESP32);
  Serial.println("°C");




  //Control entre lecturas
  delay(5000);
  Serial.println("<--------->");


}
