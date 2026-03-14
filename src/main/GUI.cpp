/*##################
  ### MODULO GUI ###
  ##################*/

#include "GUI.h"

/*######################
  ### SECCION DE GUI ###
  ######################*/
void GUI::iniciar() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

/*#################################
  ### MARCOS VERDES REDONDEADOS ###
  #################################*/
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
  const int marcoRedondeo = 15;

  // Dibujado
  tft.drawRoundRect(3, 3, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Humedad del Aire
  tft.drawRoundRect(163, 3, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Temperatura
  tft.drawRoundRect(3, 123, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Humedad de la Tierra
  tft.drawRoundRect(163, 123, marcoAncho, marcoAlto, marcoRedondeo, marcoColor); // Icono de Litros de Agua

  // Carita de Decoracion
  tft.fillCircle(anchoMedio, altoMedio, 14, TFT_WHITE);
  tft.fillCircle(anchoMedio - 5, altoMedio - 4, 3, TFT_BLACK);
  tft.fillCircle(anchoMedio + 5, altoMedio - 4, 3, TFT_BLACK);
  tft.drawArc(anchoMedio, altoMedio + 2, 7, 6, 270, 90, TFT_BLACK, TFT_WHITE, true);
}

/*#########################
  ### SECCION DE ICONOS ###
  #########################*/

/*############################
### ICONO DE GOTA DE AGUA ###
############################*/
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

/*#####################
  ### ICONO DE AIRE ###
  #####################*/
void Iconos::aire() {
  tft.fillEllipse(50, 48, 18, 18, 0xBDF7);
  tft.fillEllipse(53, 45, 15, 15, 0xFFFF);
  tft.fillEllipse(75, 28, 22, 22, 0xBDF7);
  tft.fillEllipse(78, 25, 19, 19, 0xFFFF);
  tft.fillEllipse(77, 57, 18, 18, 0xBDF7);
  tft.fillEllipse(80, 54, 15, 15, 0xFFFF);
  tft.fillEllipse(104, 43, 18, 18, 0xBDF7);
  tft.fillEllipse(107, 40, 15, 15, 0xFFFF);
  gota(60, 55, 7, 0x24BE);
  gota(100, 65, 7, 0x24BE);
}

/*############################
  ### ICONO DE TEMPERATURA ###
  ############################*/
  void Iconos::temp(){
  tft.fillEllipse(200, 41, 19, 19, 0xFFFF);
  tft.fillRoundRect(194, 32, 104, 20, 8, 0xFFFF);
  tft.fillEllipse(200, 41, 14, 14, 0x0);
  tft.fillRoundRect(207, 35, 87, 13, 5, 0x0);
  tft.fillEllipse(200, 41, 10, 10, 0xE8EC);
  tft.fillRoundRect(204, 38, 87, 7, 2, 0xE8EC);
  tft.fillRect(222, 34, 3, 9, 0xFFFF);
  tft.fillRect(236, 34, 3, 9, 0xFFFF);
  tft.fillRect(250, 34, 3, 9, 0xFFFF);
  tft.fillRect(263, 34, 3, 9, 0xFFFF);
}

/*##################################
  ### ICONO DE HUMEDAD DE TIERRA ###
  ##################################*/
void Iconos::tierra() {
  tft.fillRoundRect(33, 146, 68, 50, 10, 0x8283);
  tft.fillRoundRect(33, 150, 68, 40, 10, 0xAB45);
  tft.fillRoundRect(33, 142, 68, 26, 7, 0x8283);
  tft.fillRoundRect(30, 142, 74, 18, 8, 0x4D6A);
  tft.fillRoundRect(30, 142, 69, 14, 4, 0x8E09);
  tft.fillEllipse(43, 176, 2, 2, 0xBDF7);
  tft.fillEllipse(73, 173, 2, 2, 0xBDF7);
  tft.fillEllipse(85, 183, 1, 1, 0x73AE);
  tft.fillEllipse(60, 183, 2, 2, 0xBDF7);
  tft.fillEllipse(56, 172, 1, 1, 0x73AE);
  tft.fillEllipse(92, 172, 1, 1, 0xBDF7);
  tft.fillEllipse(72, 147, 2, 2, 0x4D6A);
  tft.fillEllipse(85, 152, 1, 1, 0x4D6A);
  tft.fillEllipse(57, 150, 2, 2, 0x4D6A);
  tft.fillEllipse(41, 144, 1, 1, 0x4D6A);
  tft.fillEllipse(94, 146, 1, 1, 0x4D6A);
  gota(120, 175, 15, 0x24BE);
}

/*################################
  ### ICONO DE AGUA DISPONIBLE ###
  ################################*/
void Iconos::agua() {
  tft.drawEllipse(243, 141, 33, 7, 0xFFFF);
  tft.drawEllipse(243, 192, 33, 7, 0xFFFF);
  tft.drawLine(210, 141, 210, 193, 0xFFFF);
  tft.drawLine(276, 192, 276, 141, 0xFFFF);
  tft.fillEllipse(243, 187, 30, 7, 0x24BE);
  tft.fillRect(213, 162, 61, 25, 0x24BE);
  tft.fillEllipse(243, 163, 30, 7, 0x65DF);
}

GUI gui;