#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"

#include "config.h"

// Variables del Servidor Web
httpd_handle_t stream_httpd = NULL;

// Constantes para el protocolo de streaming (MJPEG)
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// Función que maneja el envío continuo de fotos (El video)
esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t * fb = NULL;
  esp_err_t res = ESP_OK;
  char part_buf[64];

  // Configurar el tipo de respuesta como "streaming continuo"
  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if(res != ESP_OK) return res;

  // Bucle infinito: Tomar foto -> Enviar -> Repetir
  while(true){
    fb = esp_camera_fb_get(); // Tomar la foto
    if (!fb) {
      Serial.println("Error al capturar la imagen");
      res = ESP_FAIL;
      break;
    }

    // Enviar las cabeceras de la parte actual del video
    res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    if(res == ESP_OK){
      size_t hlen = snprintf(part_buf, 64, _STREAM_PART, fb->len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    // Enviar los datos binarios de la imagen JPEG
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
    }

    // Limpiar la memoria para la siguiente foto
    esp_camera_fb_return(fb);
    fb = NULL;

    // Si el usuario cierra el navegador, salir del bucle
    if(res != ESP_OK) break;
  }
  return res;
}

// Iniciar el Servidor
void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = 80;

  httpd_uri_t index_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };

  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &index_uri);
  }
}

void setup() {
  Serial.begin(115200);
  
  // 1. Conectar al WiFi
  WiFi.begin(WIFI_RED, WIFI_PSW);
  Serial.print("Conectando al WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");

  // 2. Configurar la Cámara
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
  config.pin_pwdn     = CAM_PWDN;
  config.pin_reset    = CAM_RESET;
  config.xclk_freq_hz = 20000000;
  config.frame_size   = FRAMESIZE_VGA;      // Resolución 640x480
  config.pixel_format = PIXFORMAT_JPEG;     // Formato de salida JPEG
  config.grab_mode    = CAMERA_GRAB_LATEST; // Siempre agarrar la imagen más reciente
  config.fb_location  = CAMERA_FB_IN_PSRAM; // Guardar en memoria PSRAM
  config.jpeg_quality = 10;                 // Calidad de 0 a 63 (menor es mejor calidad)
  config.fb_count     = 2;                  // Usar 2 buffers en PSRAM para mayor fluidez

  // 3. Inicializar la Cámara
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Error inicializando la cámara: 0x%x", err);
    return;
  }

  // 4. Arrancar el servidor web
  startCameraServer();
  Serial.print("¡Cámara lista! Abre en tu navegador: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  // El loop queda vacío. El servidor HTTP se ejecuta en segundo plano.
  delay(10000);
}