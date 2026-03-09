#include "gui.h"

/*
  ######################
  ### SECCION DE GUI ###
  ######################
*/
void GUI::iniciar() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

void GUI::marco() {
  // Medidas de la pantalla
  const int ancho = tft.width();
  const int alto = tft.height();
  const int anchoMedio = ancho / 2;
  const int altoMedio = alto / 2;
  // Configuracion de los marcos
  const uint16_t marcoColor = TFT_GREEN;
  const int marcoAncho = anchoMedio - 3;
  const int marcoAlto = altoMedio - 3;
  const int marcoRedondeo = 7;
  // Dibujado
  tft.drawRoundRect(2, 2, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Humedad del Aire
  tft.drawRoundRect(162, 2, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Temperatura
  tft.drawRoundRect(2, 122, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Humedad de la Tierra
  tft.drawRoundRect(162, 122, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Litros de Agua
  // Carita de Decoracion
  tft.fillCircle(anchoMedio, altoMedio, 14, TFT_WHITE);
  tft.fillCircle(anchoMedio - 5, altoMedio - 4, 3, TFT_BLACK);
  tft.fillCircle(anchoMedio + 5, altoMedio - 4, 3, TFT_BLACK);
  tft.drawArc(anchoMedio, altoMedio + 2, 7, 6, 270, 90, TFT_BLACK, TFT_WHITE, true);
}

/*
  #########################
  ### SECCION DE ICONOS ###
  #########################
*/
void Iconos::gota(int x, int y, int size, uint16_t color) {
  tft.fillCircle(x, y, size, color);
  const int puntaY = y - (size * 1.8);
  const int anchoBase = size * 0.9;
  tft.fillTriangle(
    x, puntaY,            // P0: Punta arriba
    x - anchoBase, y,     // P1: Izquierda
    x + anchoBase, y,     // P2: Derecha
    color
  );
}

void Iconos::aire() {
  tft.fillCircle(60, 45, 15, TFT_WHITE);
  tft.fillCircle(80, 35, 20, TFT_WHITE);
  tft.fillCircle(100, 45, 15, TFT_WHITE);
  gota(70, 55, 6, TFT_CYAN);
  gota(90, 65, 6, TFT_CYAN);
}

void Iconos::temp(){
  tft.fillRoundRect(235, 15, 12, 50, 6, TFT_LIGHTGREY);
  tft.fillCircle(241, 70, 14, TFT_RED);
}

void Iconos::tierra() {
  tft.fillCircle(80, 165, 18, 0x9381);
  tft.fillTriangle(62, 165, 98, 165, 80, 135, 0x9381);
}

void Iconos::agua() {
  tft.drawRect(220, 140, 40, 50, TFT_WHITE);
  tft.fillRect(222, 165, 36, 23, TFT_BLUE);
}

GUI gui;