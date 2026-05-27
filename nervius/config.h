#ifndef CONFIG
#define CONFIG
#include <Arduino.h>
#include <DHT.h>

/* COMUNICACION UART */
#define RX_PIN 16
#define TX_PIN 17
#define BAUDIOS 115200

/* CONFIGURACION DE LECTURA */
#define INTERVALO 2

/* LED INTEGRADO */
#define LED_PIN 2

/* PINES DE SENSORES */
#define DHT_PIN 4
#define DHT_MODEL DHT22
#define FC28_PIN  34
#define FC28_HIGH 2410
#define FC28_LOW  4095
#define FTRT_PIN  35
#define FTRT_HIGH 4095
#define FTRT_LOW  0
#define TRIG_PIN 5
#define ECHO_PIN 18

/* PINES DE ACTUADORES */
#define UV_PIN      25   // LED UV — salida PWM (LEDC canal 0, 8-bit, 5 kHz)
#define RIEGO_PIN   26   // Bomba de riego — salida digital a relé (HIGH = encendido)

/* CONFIGURACIÓN PWM PARA LUZ UV */
#define UV_LEDC_CANAL      0
#define UV_LEDC_FRECUENCIA 5000
#define UV_LEDC_RESOLUCION 8     // 8 bits → valores 0–255

#endif
