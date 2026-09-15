#pragma once
#include <Arduino.h>

// ============================================================
// PINES
// ============================================================
#define TFT_CS   5
#define TFT_RST  4
#define TFT_DC   2
#define PIN_IZQ  27
#define PIN_CEN  26
#define PIN_DER  25
#define PIN_BUZZER 32

// ============================================================
// PANTALLA
// ============================================================
#define ANCHO  160
#define ALTO   128

// ============================================================
// COLORES
// ============================================================
#define COLOR_NEGRO     0x0000
#define COLOR_BLANCO    0xFFFF
#define COLOR_ROJO      0xF800
#define COLOR_VERDE     0x07E0
#define COLOR_AMARILLO  0xFFE0
#define COLOR_GRIS      0x8410
#define COLOR_NARANJA   0xFD20
#define COLOR_AZUL      0x001F

// ============================================================
// TIEMPOS
// ============================================================
#define MS_TICK         60000UL
#define DIAS_BEBE       5
#define MINUTOS_NACER   15
#define HORA_DORMIR     23
#define HORA_DESPERTAR  7
#define DIAS_SIN_CALOR  2
#define DIAS_SIN_LIMPIAR 5
#define DIAS_ENFERMO    3
#define DIAS_SIN_FELIZ  5
#define SEGUNDOS_REINICIO 10

// ============================================================
// FASES
// ============================================================
#define FASE_HUEVO       0
#define FASE_BEBE        1
#define FASE_TODDLER     2
#define FASE_ADOLESCENTE 3
#define FASE_ADULTO      4
#define FASE_MALDITO     5

// ============================================================
// PANTALLAS
// ============================================================
#define PANTALLA_ANIMACION_HUEVO  0
#define PANTALLA_SETEAR_HORA      1
#define PANTALLA_MASCOTA          2
#define PANTALLA_MENU             3
#define PANTALLA_COMER            4
#define PANTALLA_LUZ              5
#define PANTALLA_JUGAR            6
#define PANTALLA_CURAR            7
#define PANTALLA_LIMPIAR          8
#define PANTALLA_STATS            9
#define PANTALLA_DISCIPLINA       10
#define PANTALLA_ESTADO_ALTERADO  11
#define PANTALLA_MUERTE           12
#define PANTALLA_EVOLUCION        13

// ============================================================
// ESTADO DEL JUEGO
// ============================================================
struct EstadoJuego {
  uint8_t pantallaActual;

  // Stats
  uint8_t temperatura;
  uint8_t energia;
  uint8_t felicidad;
  uint8_t desobediencia;

  // Salud
  bool enferma;
  bool sucia;

  // Contadores de días en mal estado
  uint8_t diasSinCalor;
  uint8_t diasSinLimpiar;
  uint8_t diasEnfermo;
  uint8_t diasSinFeliz;

  // Fase
  uint8_t fase;
  bool    maldita;
  bool    viva;

  // Reloj
  uint8_t hora;
  uint8_t minutos;
  bool    horaConfigurada;
  bool    luzApagada;
  bool    durmiendo;
  bool    silenciado;

  // Progresión
  uint32_t edadEnMinutos;
  uint32_t minutosEnFaseActual;

  // Menú
  uint8_t iconoSeleccionado;

  // Muerte
  bool sonidoMuerteReproducido;

  // Reinicio muerte (contador de botones)
  unsigned long tiempoBotonMuerte;
  bool botonesLateralesMuerte;
};

extern EstadoJuego juego;