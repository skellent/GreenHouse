#ifndef CONFIG
#define CONFIG
#include <Arduino.h>

/* CREDENCIALES WIFI */
#define WIFI_RED "Skellent"
#define WIFI_PSW "greenhouse"

#define PIN_RGB 48 // LED RGB INTEGRADO

/* COMUNICACION UART */
#define PIN_RX  44
#define PIN_TX  43
#define BAUDIOS 115200

/* PANTALLA LCD */
#define LCD_SDA 39
#define LCD_SCL 40
#define LCD_PROTOCOLO 0x27
#define LCD_BOTON 19

#endif