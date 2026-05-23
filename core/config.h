#ifndef CONFIG
#define CONFIG
#include <Arduino.h>

/* CREDENCIALES WIFI */
#define WIFI_RED "Rodriguez's Net"
#define WIFI_PSW "7kyxj2fd"

#define PIN_RGB 48 // LED RGB INTEGRADO

/* COMUNICACION UART */
#define PIN_RX  44
#define PIN_TX  43
#define BAUDIOS 115200

/* PANTALLA LCD */
#define LCD_SDA 39
#define LCD_SCL 40
#define LCD_PROTOCOLO 0x27

/* BUZZER */
#define PIN_BUZZER 38

#endif