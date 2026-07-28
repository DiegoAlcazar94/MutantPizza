#include "fases.h"

void iniciarJuego() {
  juego.pantallaActual      = PANTALLA_ANIMACION_HUEVO;
  juego.hambre              = 5;
  juego.energia             = 5;
  juego.felicidad           = 5;
  juego.desobediencia       = 0;
  juego.enferma             = false;
  juego.sucia               = false;
  juego.maldita             = false;
  juego.viva                = true;
  juego.luzApagada          = false;
  juego.durmiendo           = false;
  juego.silenciado          = false;
  juego.hora                = 12;
  juego.minutos             = 0;
  juego.horaConfigurada     = false;
  juego.fase                = FASE_HUEVO;
  juego.edadEnMinutos       = 0;
  juego.minutosEnFaseActual = 0;
  juego.iconoSeleccionado   = 0;
  juego.diasSinComer        = 0;
  juego.diasSinLimpiar      = 0;
  juego.diasEnfermo         = 0;
  juego.diasSinFeliz        = 0;
}

// --- Comprueba si la mascota debe morir ---
static void comprobarMuerte() {
  if (juego.diasSinComer   >= DIAS_SIN_COMER)   juego.viva = false;
  if (juego.diasSinLimpiar >= DIAS_SIN_LIMPIAR)  juego.viva = false;
  if (juego.diasEnfermo    >= DIAS_ENFERMO)       juego.viva = false;
  if (juego.diasSinFeliz   >= DIAS_SIN_FELIZ)     juego.viva = false;

  if (!juego.viva) {
    juego.pantallaActual = PANTALLA_MUERTE;
  }
}

// --- Comprueba si debe evolucionar ---
static void comprobarEvolucion() {
  // Solo evoluciona si ya nació (no es huevo)
  if (juego.fase == FASE_HUEVO) return;

  // 5 días reales = 5 * 24 * 60 minutos
  uint32_t minutosPorFase = (uint32_t)DIAS_BEBE * 24 * 60;

  if (juego.minutosEnFaseActual >= minutosPorFase) {
    juego.minutosEnFaseActual = 0;

    if (juego.fase < FASE_ADULTO) {
      juego.fase++;

      // Si llegó a adulto con desobediencia máxima, es maldito
      if (juego.fase == FASE_ADULTO && juego.desobediencia >= 5) {
        juego.maldita = true;
        juego.fase    = FASE_MALDITO;
      }

      juego.pantallaActual = PANTALLA_EVOLUCION;
    }
  }
}

// --- Actualiza el reloj interno ---
static void actualizarReloj() {
  juego.minutos++;
  if (juego.minutos >= 60) {
    juego.minutos = 0;
    juego.hora    = (juego.hora + 1) % 24;
  }

  // Comprobar si es hora de dormir o despertar
  if (juego.hora == HORA_DORMIR)    juego.durmiendo = true;
  if (juego.hora == HORA_DESPERTAR) juego.durmiendo = false;
}

// --- Tick principal, se llama cada minuto ---
void tickJuego() {
  // Si no está viva no hacemos nada
  if (!juego.viva) return;

  // Si no se ha configurado la hora todavía, solo avanzamos
  // el contador para saber cuándo nacer
  if (!juego.horaConfigurada) return;

  actualizarReloj();

  // Fase huevo: contar minutos hasta nacer
  if (juego.fase == FASE_HUEVO) {
    juego.minutosEnFaseActual++;
    if (juego.minutosEnFaseActual >= MINUTOS_NACER) {
      juego.fase                = FASE_BEBE;
      juego.minutosEnFaseActual = 0;
      juego.pantallaActual      = PANTALLA_EVOLUCION;
    }
    return; // mientras es huevo no bajamos stats
  }

  // A partir de aquí ya nació — subimos edad
  juego.edadEnMinutos++;
  juego.minutosEnFaseActual++;

  // Bajar stats cada minuto (solo si no está durmiendo)
  if (!juego.durmiendo) {
    if (juego.hambre    > 0) juego.hambre--;
    if (juego.felicidad > 0) juego.felicidad--;
  }

  // La energía solo se recupera durmiendo con la luz apagada
  if (juego.durmiendo && juego.luzApagada) {
    if (juego.energia < 5) juego.energia++;
  } else if (!juego.durmiendo) {
    if (juego.energia > 0) juego.energia--;
  }

  // La suciedad aumenta con el tiempo (cada 60 minutos)
  if (juego.edadEnMinutos % 60 == 0) {
    if (!juego.sucia) juego.sucia = true;
  }

  // La desobediencia sube si está sucia o enferma sin curar
  if (juego.sucia   && juego.desobediencia < 5) juego.desobediencia++;
  if (juego.enferma && juego.desobediencia < 5) juego.desobediencia++;

  // Contadores de días en mal estado (cada 1440 minutos = 1 día)
  if (juego.edadEnMinutos % 1440 == 0) {
    if (juego.hambre    == 0) juego.diasSinComer++;    else juego.diasSinComer    = 0;
    if (juego.sucia)          juego.diasSinLimpiar++;  else juego.diasSinLimpiar  = 0;
    if (juego.enferma)        juego.diasEnfermo++;     else juego.diasEnfermo     = 0;
    if (juego.felicidad == 0) juego.diasSinFeliz++;    else juego.diasSinFeliz    = 0;
  }

  comprobarMuerte();
  comprobarEvolucion();
}