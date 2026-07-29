#pragma once
#include <Arduino.h>
#include "juego.h"
#include "pantalla.h"

// Inicializa el minijuego desde cero
void iniciarMinijuego();

// Se llama cada frame desde actualizarPantalla cuando estamos en PANTALLA_JUGAR
void actualizarMinijuego();

// Se llama desde botones.cpp cuando estamos en el minijuego
void botonMinijuegoIzq();
void botonMinijuegoCen();
void botonMinijuegoDer();
extern bool minijuegoActivo;
