#pragma once
#include <Arduino.h>
#include "juego.h"

// Inicializa el estado del juego desde cero
void iniciarJuego();

// Se llama cada minuto — baja stats, comprueba muertes, evoluciones
void tickJuego();