#ifndef CONFIG
#define CONFIG
#include <Arduino.h>
#include <DHT.h>

/* COMUNICACION UART */
#define RX_PIN 16
#define TX_PIN 17
#define BAUDIOS 115200

/* CONFIGURACION DE LECTURA */
#define INTERVALO 1

/* PINES DE SENSORES */
#define DHT_PIN 19
#define DHT_MODEL DHT22
#define FC28_PIN  2
#define FC28_HIGH 2410
#define FC28_LOW  4095
#define FTRT_PIN  15
#define FTRT_HIGH 4095
#define FTRT_LOW  0
#define TRIG_PIN 5
#define ECHO_PIN 18
#define DS18_PIN 22

/* SENSOR DE NIVEL DE AGUA (analógico resistivo) */
#define NIVEL_PIN   4        // pin ADC conectado a la salida del sensor
#define NIVEL_LOW   0        // valor ADC con el sensor en seco (vacío)
#define NIVEL_HIGH  3200     // valor ADC con el sensor totalmente sumergido (lleno)

/* PINES DE ACTUADORES */
#define UV_PIN      23
#define RIEGO_PIN   35

/* CONFIGURACIÓN PWM PARA LUZ UV */
#define UV_LEDC_CANAL      0
#define UV_LEDC_FRECUENCIA 5000
#define UV_LEDC_RESOLUCION 8

/* CANDELABRO DECORATIVO */
#define CANDELABRO_CANAL 1
#define CANDELABRO_FRECUENCIA 5000
#define CANDELABRO_RESOLUCION 8
#define CANDELABRO_PIN 34

#endif