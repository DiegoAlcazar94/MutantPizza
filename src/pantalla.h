#pragma once
#include "sprites/sprites.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include "juego.h"

// La pantalla es global, definida en main.cpp
extern Adafruit_ST7735 tft;

// Se llama cada 500ms desde el loop para actualizar lo que se ve
void actualizarPantalla();

// Para parar el sonido de muerte
void resetearSonidoMuerte();