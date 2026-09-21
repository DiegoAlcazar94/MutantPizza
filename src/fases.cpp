#include "fases.h"

void iniciarJuego() {
  juego.pantallaActual          = PANTALLA_ANIMACION_HUEVO;
  juego.temperatura             = 5;
  juego.energia                 = 5;
  juego.felicidad               = 5;
  juego.desobediencia           = 0;
  juego.enferma                 = false;
  juego.sucia                   = false;
  juego.maldita                 = false;
  juego.viva                    = true;
  juego.luzApagada              = false;
  juego.durmiendo               = false;
  juego.silenciado              = false;
  juego.hora                    = 12;
  juego.minutos                 = 0;
  juego.horaConfigurada         = false;
  juego.fase                    = FASE_HUEVO;
  juego.edadEnMinutos           = 0;
  juego.minutosEnFaseActual     = 0;
  juego.iconoSeleccionado       = 0;
  juego.diasSinCalor            = 0;
  juego.diasSinLimpiar          = 0;
  juego.diasEnfermo             = 0;
  juego.diasSinFeliz            = 0;
  juego.sonidoMuerteReproducido = false;
  juego.tiempoBotonMuerte       = 0;
  juego.botonesLateralesMuerte  = false;
}

static void comprobarMuerte() {
  if (juego.diasSinCalor    >= DIAS_SIN_CALOR)    juego.viva = false;
  if (juego.diasSinLimpiar  >= DIAS_SIN_LIMPIAR)  juego.viva = false;
  if (juego.diasEnfermo     >= DIAS_ENFERMO)       juego.viva = false;
  if (juego.diasSinFeliz    >= DIAS_SIN_FELIZ)     juego.viva = false;

  if (!juego.viva) {
    juego.pantallaActual = PANTALLA_MUERTE;
  }
}

static void comprobarEvolucion() {
  if (juego.fase == FASE_HUEVO) return;
  if (juego.fase >= FASE_ADULTO && !juego.maldita) return;
  if (juego.fase == FASE_MALDITO) return;

  uint32_t minutosPorFase = (uint32_t)DIAS_BEBE * 24 * 60;

  if (juego.minutosEnFaseActual >= minutosPorFase) {
    juego.minutosEnFaseActual = 0;
    juego.fase++;

    if (juego.fase == FASE_ADULTO && juego.desobediencia >= 5) {
      juego.maldita = true;
      juego.fase    = FASE_MALDITO;
    }

    juego.pantallaActual = PANTALLA_EVOLUCION;
  }
}

static void actualizarReloj() {
  juego.minutos++;
  if (juego.minutos >= 60) {
    juego.minutos = 0;
    juego.hora    = (juego.hora + 1) % 24;
  }

  if (juego.hora == HORA_DORMIR)    juego.durmiendo = true;
  if (juego.hora == HORA_DESPERTAR) juego.durmiendo = false;
}

void tickJuego() {
  if (!juego.viva) return;
  if (!juego.horaConfigurada) return;

  actualizarReloj();

  if (juego.fase == FASE_HUEVO) {
    juego.minutosEnFaseActual++;
    if (juego.minutosEnFaseActual >= MINUTOS_NACER) {
      juego.fase                = FASE_BEBE;
      juego.minutosEnFaseActual = 0;
      juego.pantallaActual      = PANTALLA_EVOLUCION;
    }
    return;
  }

  juego.edadEnMinutos++;
  juego.minutosEnFaseActual++;

  // Temperatura baja siempre
  if(juego.edadEnMinutos % 30 == 0){
    if (juego.temperatura > 0) juego.temperatura--;
  }

  // Felicidad baja si no duerme
  if (!juego.durmiendo && (juego.edadEnMinutos % 45 =0 0)) {
    if (juego.felicidad > 0) juego.felicidad--;
  }

  // Energía
  if (juego.durmiendo && juego.luzApagada) {
    if(juego.edadEnMinutos % 15 == 0){
      if (juego.energia < 5) juego.energia++;
    }
  } else if (!juego.durmiendo) {
    if(juego.edadEnMinutos % 60 == 0){
      if (juego.energia > 0) juego.energia--;
    }
  }

  // Suciedad 
  if (juego.edadEnMinutos % 120 == 0) {
    if (!juego.sucia) juego.sucia = true;
  }

  // Desobediencia
  if(juego.edadEnMinutos % 90 == 0){
    if (juego.sucia   && juego.desobediencia < 5) juego.desobediencia++;
    if (juego.enferma && juego.desobediencia < 5) juego.desobediencia++;
  }

  // Contadores diarios
  if (juego.edadEnMinutos % 1440 == 0) {
    if (juego.temperatura == 0) juego.diasSinCalor++;    else juego.diasSinCalor   = 0;
    if (juego.sucia)            juego.diasSinLimpiar++;  else juego.diasSinLimpiar = 0;
    if (juego.enferma)          juego.diasEnfermo++;     else juego.diasEnfermo    = 0;
    if (juego.felicidad == 0)   juego.diasSinFeliz++;    else juego.diasSinFeliz   = 0;
  }

  comprobarMuerte();
  comprobarEvolucion();
}
