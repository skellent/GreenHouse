/*##################
  ### MODULO GUI ###
  ##################*/

#pragma once
#include <TFT_eSPI.h>

extern TFT_eSPI tft;

// Sub-grupo de Iconos
struct Iconos {
  void aire();
  void temp();
  void tierra();
  void agua();
  void ganchoDerecha();
  void ganchoIzquierda();
  void gota(int x, int y, int size, uint16_t color);
};

// Grupo Principal
struct GUI {
  void iniciar();
  void marco();
 
 
  // Sub-grupos
  Iconos icono;
};

// Exportacion para alcance global
extern GUI gui;