#ifndef CAM
#define CAM


#include "esp_camera.h"
#include <Arduino.h>


/* CÁMARA */
#define CAM_PWDN  -1
#define CAM_RESET -1
// ------------------
#define CAM_XCLK 15
#define CAM_SIOD  4
#define CAM_SIOC  5
// ------------------
#define CAM_Y9    16
#define CAM_Y8    17
#define CAM_Y7    18
#define CAM_Y6    12
#define CAM_Y5    10
#define CAM_Y4     8
#define CAM_Y3     9
#define CAM_Y2    11
#define CAM_VSYNC  6
#define CAM_HREF   7
#define CAM_PCLK  13


bool initCamara() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = CAM_Y2;
  config.pin_d1       = CAM_Y3;
  config.pin_d2       = CAM_Y4;
  config.pin_d3       = CAM_Y5;
  config.pin_d4       = CAM_Y6;
  config.pin_d5       = CAM_Y7;
  config.pin_d6       = CAM_Y8;
  config.pin_d7       = CAM_Y9;
  config.pin_xclk     = CAM_XCLK;
  config.pin_pclk     = CAM_PCLK;
  config.pin_vsync    = CAM_VSYNC;
  config.pin_href     = CAM_HREF;
  config.pin_sccb_sda = CAM_SIOD;
  config.pin_sccb_scl = CAM_SIOC;
  config.pin_pwdn     = -1;
  config.pin_reset    = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  config.frame_size = FRAMESIZE_QVGA; // Configuración de resolución menor a VGA por rendimiento (QVGA = 320x240)
  config.jpeg_quality = 10;           // Calidad de compresión (10 es alta calidad, 63 es baja)
  config.fb_count = 1;                // Un solo buffer es suficiente si no haces streaming continuo de alta tasa

  esp_err_t err = esp_camera_init(&config); // Comando nativo de Espressif para levantar el hardware de la cámara
  if (err != ESP_OK) {
    Serial.printf("Error crítico de cámara (0x%x)\n", err);
    return false;
  }

  return true;
}


#endif