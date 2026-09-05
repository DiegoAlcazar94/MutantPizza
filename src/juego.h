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

// ============================================================
// PANTALLA
// Con setRotation(1): 160px ancho x 128px alto
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
#define MS_TICK         60000UL  // 1 minuto real
#define DIAS_BEBE       5        // días que dura cada fase
#define MINUTOS_NACER   15       // minutos desde guardar hora hasta nacer
#define HORA_DORMIR     23       // 11pm
#define HORA_DESPERTAR  7        // 7am

// Días sin atender antes de morir por cada causa
#define DIAS_SIN_CALOR    2
#define DIAS_SIN_LIMPIAR  5
#define DIAS_ENFERMO      3
#define DIAS_SIN_FELIZ    5

// ============================================================
// FASES DE VIDA
// ============================================================
#define FASE_HUEVO       0
#define FASE_BEBE        1
#define FASE_TODDLER     2
#define FASE_ADOLESCENTE 3
#define FASE_ADULTO      4
#define FASE_MALDITO     5

// ============================================================
// PANTALLAS / ESTADOS DEL JUEGO
// Esto controla qué se muestra en cada momento
// ============================================================
#define PANTALLA_ANIMACION_HUEVO  0  // rider en moto, bucle infinito
#define PANTALLA_SETEAR_HORA      1  // configurar hora con botones
#define PANTALLA_MASCOTA          2  // pantalla principal con la pet
#define PANTALLA_MENU             3  // menú de 8 funciones
#define PANTALLA_COMER            4  // función comer
#define PANTALLA_LUZ              5  // función luz
#define PANTALLA_JUGAR            6  // minijuego
#define PANTALLA_CURAR            7  // función curar
#define PANTALLA_LIMPIAR          8  // función limpiar
#define PANTALLA_STATS            9  // función stats
#define PANTALLA_DISCIPLINA       10 // función disciplina
#define PANTALLA_ESTADO_ALTERADO  11 // función estado alterado
#define PANTALLA_MUERTE           12 // pantalla de muerte
#define PANTALLA_EVOLUCION        13 // animación de evolución

// Zumbador
#define PIN_BUZZER  32

// Reinicio en pantalla de muerte
#define SEGUNDOS_REINICIO  10

// ============================================================
// ESTADO DEL JUEGO
// ============================================================
struct EstadoJuego {

  // Qué se muestra ahora mismo
  uint8_t pantallaActual;

  // --- Stats (0 a 5) ---
  uint8_t temperatura;
  uint8_t energia;
  uint8_t felicidad;
  uint8_t desobediencia; // al llegar a 5 en fases iniciales -> adulto maldito

  // --- Salud ---
  bool enferma;
  bool sucia;

  // --- Contadores de días en mal estado (para muerte) ---
  uint8_t diasSinCalor;
  uint8_t diasSinLimpiar;
  uint8_t diasEnfermo;
  uint8_t diasSinFeliz;

  // --- Fase de vida ---
  uint8_t fase;
  bool    maldita;       // true si llegó al máximo de desobediencia
  bool    viva;

  // --- Reloj ---
  uint8_t hora;
  uint8_t minutos;
  bool    horaConfigurada;
  bool    luzApagada;
  bool    durmiendo;
  bool    silenciado;

  // --- Progresión ---
  uint32_t edadEnMinutos;     // minutos desde que nació
  uint32_t minutosEnFaseActual; // para saber cuándo evolucionar

  // --- Menú ---
  uint8_t iconoSeleccionado;  // 0-7, qué icono está seleccionado ahora
};

// ============================================================
// VARIABLE GLOBAL
// extern significa "este juego existe, está definido en main.cpp"
// Cualquier archivo que incluya juego.h puede usarlo
// ============================================================
extern EstadoJuego juego;