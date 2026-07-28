#pragma once
#include <Arduino.h>
#include "juego.h"

// Inicializa los pines de los botones
void iniciarBotones();

// Lee el estado de los botones y actúa según la pantalla actual
void leerBotones();