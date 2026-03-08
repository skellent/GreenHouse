#include <SPI.h>
#include <TFT_eSPI.h>        // Display library

// Define tus pines (¡CÁMBIALOS si los conectaste diferente!)
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  4
#define TFT_MOSI 23           // <-- AÑADIDO: Pin para datos SPI
#define TFT_MISO 19           // <-- AÑADIDO: Pin para recibir datos (¡importante!)
#define TFT_SCLK 18           // <-- AÑADIDO: Pin para el reloj SPI

#define TOUCH_CS 33           // Chip select para el touch
#define REPEAT_CAL true   
TFT_eSPI tft = TFT_eSPI();    // Crear objeto para la pantalla

void setup() {
  Serial.begin(115200);
  delay(1000); // Pequeño retardo para tu ESP32-S3

  // --- INICIALIZAR EL BUS SPI ---
  // ¡Este es el paso que faltaba! Le decimos al ESP32 qué pines usará para SPI.
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
  // O también puedes usar: SPI.begin(); // Si usas los pines SPI por defecto (VSPI: 18, 19, 23)
  
  // Inicializar pantalla
  tft.init();
  tft.setRotation(1);         // Ajusta la orientación
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE);
  tft.drawString("Touch screen", 50, 100, 2);

  // Touch setup
  pinMode(TOUCH_CS, OUTPUT);
  digitalWrite(TOUCH_CS, HIGH);
  Serial.println("Sistema listo. Toca la pantalla.");
}

void loop() {
  uint16_t x, y;
  uint8_t pressure;

  if (readTouch(&x, &y, &pressure)) {
    int screenX = map(x, 200, 3800, 0, 239);
    int screenY = map(y, 200, 3800, 0, 319);
    screenX = constrain(screenX, 0, 239);
    screenY = constrain(screenY, 0, 319);
    tft.fillCircle(screenX, screenY, 5, TFT_WHITE);
    Serial.printf("Raw(%d,%d) -> Screen(%d,%d)\n", x, y, screenX, screenY);
  }
}

// Función para leer el XPT2046 (¡ahora el SPI ya funciona!)
bool readTouch(uint16_t *x, uint16_t *y, uint8_t *z) {
  digitalWrite(TOUCH_CS, LOW);
  delayMicroseconds(1); // Pequeña pausa para estabilidad

  SPI.transfer(0x90);
  *x = SPI.transfer(0) << 4;
  *x |= SPI.transfer(0) >> 4;

  SPI.transfer(0xD0);
  *y = SPI.transfer(0) << 4;
  *y |= SPI.transfer(0) >> 4;

  SPI.transfer(0x80);
  *z = SPI.transfer(0);

  digitalWrite(TOUCH_CS, HIGH);
  return (*z > 10);
}