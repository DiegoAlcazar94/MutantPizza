#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "juego.h"
#include "botones.h"
#include "pantalla.h"
#include "fases.h"
#include "sonidos.h"

// Variable global del juego
EstadoJuego juego;

// Configuración de la pantalla TFT
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Temporizadores
unsigned long ultimoTick  = 0;
unsigned long ultimoFrame = 0; // Animaciones cada 500 ms

void setup() {
  Serial.begin(115200);

  // Iniciar pantalla
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(COLOR_NEGRO);

  // Iniciar periféricos y estado
  iniciarBotones();
  iniciarSonido();
  iniciarJuego();

  Serial.println("--- MutantPizza Tamagotchi ---");
}

void loop() {
  unsigned long ahora = millis();

  // 1. Leer entradas de los botones
  leerBotones();

  // 2. Paso del tiempo del juego (cada minuto)
  if (ahora - ultimoTick >= MS_TICK) {
    ultimoTick = ahora;
    tickJuego();
  }

  // 3. Refresco de pantalla y animaciones (cada 500 ms)
  if (ahora - ultimoFrame >= 500) {
    ultimoFrame = ahora;
    actualizarPantalla();
  }
}
