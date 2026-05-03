#pragma once
#include <Arduino.h>
#include "DHT.h"

struct DatosRed {
  const char* NOMBRE  = "<NOMBRE>";
  const char* CLAVE   = "<CLAVE>";
  const char* ESCUCHA = "<ESCUCHA>";
  const uint8_t   PUERTO  = <PUERTO>;
};

struct ConfiguracionPines {
  struct {
    const uint8_t TRIG = 38;
    const uint8_t ECHO = 39;
  } ultrasonido;

  struct {
    const uint8_t PIN   = 20;
    const uint8_t MODEL = DHT11;
  } dht;

  struct {
    const uint8_t TIERRA = 1;
    const uint8_t TANQUE = 2;
  } agua;

  struct {
    const uint8_t PIN = 45;
    const uint8_t VEL = 200;
    const int FRQ = 5000;
    const uint8_t RES = 8;
    const uint8_t CAN = 0;
    const uint8_t ON  = 255;
    const uint8_t OFF = 0;
  } bomba;

  struct {
    struct {
      const uint8_t PIN = 47;  // ERA: 8  → conflicto con CAM D2
      const uint8_t CAN = 1;
    } A;
    struct {
      const uint8_t PIN = 46;  // Sin conflicto — se mantiene
      const uint8_t CAN = 2;
    } B;
    const uint8_t RES  = 8;
    const int     FRQ  = 5000;
    const uint8_t ON   = 200;
    const uint8_t OFF  = 255;
    const float   TIME = 2000;
  } ultravioleta;

  struct {
    const uint8_t TEMP = 3;
  } esp32;

  struct {
    const uint8_t IN1  = 19;   // ERA: 0  → reservado/boot, mejor evitar
    const uint8_t IN2  = 20;   // ERA: 35 → no existe en S3, era inválido
    const uint8_t IN3  = 21;   // ERA: 36 → no existe en S3, era inválido
    const uint8_t IN4  = 42;   // Sin conflicto — se mantiene
    const int RNG  = 2048;
    const int ADD90 = 512;
    const int SUS90 = -512;
  } step;

  struct {
    const uint8_t PIN = 40;    // ERA: 12 → conflicto con CAM D4
  } fotoresistencia;

  struct {
    const int8_t PWDN_GPIO_NUM  = -1;
    const int8_t RESET_GPIO_NUM = -1;
    const uint8_t XCLK_GPIO_NUM  = 15;
    const uint8_t SIOD_GPIO_NUM  = 4;
    const uint8_t SIOC_GPIO_NUM  = 5;

    const uint8_t Y9_GPIO_NUM    = 16;
    const uint8_t Y8_GPIO_NUM    = 17;
    const uint8_t Y7_GPIO_NUM    = 18;
    const uint8_t Y6_GPIO_NUM    = 12;
    const uint8_t Y5_GPIO_NUM    = 10;
    const uint8_t Y4_GPIO_NUM    = 8;
    const uint8_t Y3_GPIO_NUM    = 9;
    const uint8_t Y2_GPIO_NUM    = 11;
    const uint8_t VSYNC_GPIO_NUM = 6;
    const uint8_t HREF_GPIO_NUM  = 7;
    const uint8_t PCLK_GPIO_NUM  = 13;
  } cam;
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

  bool LOGGIN = true;
};

/*##################
### ABREVIATURAS ###
####################
 - PIN = Pin físico
 - TRIG = Trigger de Ultrasonido
 - ECHO = Echo de Ultrasonido
 - MODEL = Modelo de DHT
 - VEL = Velocidad
 - FRQ = Frecuencia
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
