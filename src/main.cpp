#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "juego.h"
#include "botones.h"
#include "pantalla.h"
#include "fases.h"

// ============================================================
// AQUÍ se define la variable global del juego
// (en juego.h solo se declara con extern)
// ============================================================
EstadoJuego juego;

// Pantalla — también global para que todos los archivos la usen
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Temporizadores
unsigned long ultimoTick    = 0;
unsigned long ultimoFrame   = 0; // para animaciones (cada 500ms)

void setup() {
  Serial.begin(115200);

  // Iniciar pantalla
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(COLOR_NEGRO);

  // Iniciar botones
  iniciarBotones();

  // Iniciar estado del juego
  iniciarJuego();

  Serial.println("--- MutantPizza Tamagotchi ---");
}

void loop() {
  unsigned long ahora = millis();

  // Leer botones (siempre, en cada frame)
  leerBotones();

  // Tick del juego cada minuto
  if (ahora - ultimoTick >= MS_TICK) {
    ultimoTick = ahora;
    tickJuego();
  }

  // Actualizar animaciones cada 500ms
  if (ahora - ultimoFrame >= 500) {
    ultimoFrame = ahora;
    actualizarPantalla();
  }
}