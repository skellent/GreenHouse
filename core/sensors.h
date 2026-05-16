#pragma once
#include <Arduino.h>


struct Sensores {
  float sen_temperatura_ambiente ;
  float sen_temperatura_agua     ;
  float sen_humedad_ambiente     ;
  float sen_humedad_suelo        ;
  int   sen_intensidad_luz       ;
  int   sen_ultrasonido          ;
  int   ia_perfilplanta  ;
  int   ia_ventilacion   ;
  int   ia_intensidad_uv ;
  int   ia_riego         ;
  float esp32_temperatura  ;
  bool  esp32_camStatus    ;
};