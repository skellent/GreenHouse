#pragma once
#include <Arduino.h>
#include "DHT.h"

struct DatosRed {
  const char* NOMBRE = "<NOMBRE DE RED>";
  const char* CLAVE  = "<CONTRASENA>";
  const char* ESCUCHA = "<ESCUCHA>";
  const int PUERTO  = <PUERTO>;
};

struct ConfiguracionPines {
  struct {
    const int TRIG = 12;
    const int ECHO = 14;
  } ultrasonido;

  struct {
    const int PIN = 13;
    const int MODEL = DHT22;
  } dht;

  struct {
    const int TIERRA = 35;
    const int TANQUE = 34;
  } agua;

  struct {
    const int PIN = 33;
    const int VEL = 200;
    const int FRQ = 5000;
    const int RES = 8;
    const int CAN = 0;
    const int ON  = 255;
    const int OFF = 0;
  } bomba;

  struct {
    struct {
      const int PIN = 25;
      const int CAN = 1;
    } A;
    struct {
      const int PIN = 27;
      const int CAN = 2;
    } B;
    const int RES = 8;
    const int FRQ = 5000;
    const int ON  = 200;
    const int OFF = 255;
  } ultravioleta;

  struct {
    const int TEMP = 23; // THIS IS NOT THE CORRECT (MAYBE)
  } esp32;
};

struct Calibracion {
  struct {
    const int SECO = 4095;
    const int MOJA = 1200;
  } tierra;

  struct {
    const int SECO = 0;
    const int MOJA = 1600;
  } tanque;

  struct {
    const int RANGO = 50;
  } riego;

  struct {
    const int RANGO = 28;
  } luz;
};

/*##################
### ABREVIATURAS ###
####################
 - PIN = Pin físico
 - TRIG = Trigger de Ultrasonido
 - ECHO = Echo de Ultrasonido
 - MODEL = Modelo de DHT
 - VEL = Velocidad
 - FRQ =  Frecuencia
 - RES = Resolución
 - CAN = Canal PWM
 - TEMP = Temperatura
 - ON = Encendido
 - OFF = Apagado
 - SECO = Humedad Mínima
 - MOJA = Humedad Máxima
*/

extern DatosRed red;
extern ConfiguracionPines pines;
extern Calibracion calibracion;