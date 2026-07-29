#include "botones.h"
#include "minijuego.h"

// Estado anterior de cada botón para detectar solo el momento de la pulsación
static int ultimoIzq = HIGH;
static int ultimoCen = HIGH;
static int ultimoDer = HIGH;
static unsigned long ultimoTiempo = 0;
const unsigned long TIEMPO_ESPERA = 150;

void iniciarBotones() {
  pinMode(PIN_IZQ, INPUT_PULLUP);
  pinMode(PIN_CEN, INPUT_PULLUP);
  pinMode(PIN_DER, INPUT_PULLUP);
}

void leerBotones() {
  unsigned long ahora = millis();
  if (ahora - ultimoTiempo < TIEMPO_ESPERA) return;

  int izq = digitalRead(PIN_IZQ);
  int cen = digitalRead(PIN_CEN);
  int der = digitalRead(PIN_DER);

  // Detectamos pulsación (paso de HIGH a LOW)
  bool pulsadoIzq = (izq == LOW && ultimoIzq == HIGH);
  bool pulsadoCen = (cen == LOW && ultimoCen == HIGH);
  bool pulsadoDer = (der == LOW && ultimoDer == HIGH);

  // Silenciar con los dos laterales a la vez
  if (pulsadoIzq && pulsadoDer) {
    juego.silenciado = !juego.silenciado;
    ultimoTiempo = ahora;
    ultimoIzq = izq;
    ultimoDer = der;
    return;
  }

  // Actuar según qué pantalla estamos viendo
  switch (juego.pantallaActual) {

    case PANTALLA_ANIMACION_HUEVO:
      // Solo el botón central lleva a configurar la hora
      if (pulsadoCen) {
        juego.pantallaActual = PANTALLA_SETEAR_HORA;
        ultimoTiempo = ahora;
      }
      break;

    case PANTALLA_SETEAR_HORA:
      // Izquierda sube la hora
      if (pulsadoIzq) {
        juego.hora = (juego.hora + 1) % 24;
        ultimoTiempo = ahora;
      }
      // Central cambia entre hora y minutos (lo gestionamos con una variable local)
      if (pulsadoCen) {
        juego.minutos = (juego.minutos + 1) % 60;
        ultimoTiempo = ahora;
      }
      // Derecha guarda la hora y vuelve al huevo
      if (pulsadoDer) {
        juego.horaConfigurada = true;
        juego.pantallaActual  = PANTALLA_ANIMACION_HUEVO;
        ultimoTiempo = ahora;
      }
      break;

    case PANTALLA_MASCOTA:
      // Central abre el menú
      if (pulsadoCen) {
        juego.iconoSeleccionado = 0;
        juego.pantallaActual    = PANTALLA_MENU;
        ultimoTiempo = ahora;
      }
      break;

    case PANTALLA_MENU:
      // Izquierda navega por los iconos
      if (pulsadoIzq) {
        juego.iconoSeleccionado = (juego.iconoSeleccionado + 7) % 8;
        ultimoTiempo = ahora;
      }
      // Central entra en la función seleccionada
      if (pulsadoCen) {
        // 0=Comer 1=Luz 2=Jugar 3=Curar 4=Limpiar 5=Stats 6=Disciplina 7=EstadoAlterado
        // La luz se activa/desactiva directamente sin pantalla intermedia
        if (juego.iconoSeleccionado == 1) {
          juego.luzApagada = !juego.luzApagada;
          // Nos quedamos en el menú para que el jugador vea el cambio
        } else {
          uint8_t destinos[] = {
            PANTALLA_COMER, PANTALLA_LUZ, PANTALLA_JUGAR, PANTALLA_CURAR,
            PANTALLA_LIMPIAR, PANTALLA_STATS, PANTALLA_DISCIPLINA, PANTALLA_ESTADO_ALTERADO
          };
          juego.pantallaActual = destinos[juego.iconoSeleccionado];
        }
        ultimoTiempo = ahora;
      }
      // Derecha sale del menú y vuelve a la mascota
      if (pulsadoDer) {
        juego.pantallaActual = PANTALLA_MASCOTA;
        ultimoTiempo = ahora;
      }
      break;

    case PANTALLA_LUZ:
      if (pulsadoDer) {
        juego.pantallaActual = PANTALLA_MENU;
        ultimoTiempo = ahora;
      }
      break;

    case PANTALLA_CURAR:
      if (pulsadoCen) {
        if (juego.enferma) {
          juego.enferma     = false;
          juego.diasEnfermo = 0;
        }
        ultimoTiempo = ahora;
      }
      if (pulsadoDer) {
        juego.pantallaActual = PANTALLA_MENU;
        ultimoTiempo = ahora;
      }
      break;

    case PANTALLA_LIMPIAR:
      if (pulsadoCen) {
        if (juego.sucia) {
          juego.sucia          = false;
          juego.diasSinLimpiar = 0;
        }
        ultimoTiempo = ahora;
      }
      if (pulsadoDer) {
        juego.pantallaActual = PANTALLA_MENU;
        ultimoTiempo = ahora;
      }
      break;

    case PANTALLA_DISCIPLINA:
      if (pulsadoCen) {
        if (juego.desobediencia > 0) juego.desobediencia--;
        ultimoTiempo = ahora;
      }
      if (pulsadoDer) {
        juego.pantallaActual = PANTALLA_MENU;
        ultimoTiempo = ahora;
      }
      break;

    case PANTALLA_STATS:
    case PANTALLA_ESTADO_ALTERADO:
      if (pulsadoDer) {
        juego.pantallaActual = PANTALLA_MENU;
        ultimoTiempo = ahora;
      }
      break;

    case PANTALLA_JUGAR:
      if (minijuegoActivo) {
        if (pulsadoIzq) { botonMinijuegoIzq(); ultimoTiempo = ahora; }
        if (pulsadoCen) { botonMinijuegoCen(); ultimoTiempo = ahora; }
        if (pulsadoDer) { botonMinijuegoDer(); ultimoTiempo = ahora; }
      } else {
        // Minijuego terminado, DER vuelve al menú
        if (pulsadoDer) {
          juego.pantallaActual = PANTALLA_MENU;
          ultimoTiempo = ahora;
        }
      }
      break;

    case PANTALLA_MUERTE:
      // Cualquier botón reinicia
      if (pulsadoCen) {
        juego.pantallaActual = PANTALLA_ANIMACION_HUEVO;
        ultimoTiempo = ahora;
      }
      break;
  }

  ultimoIzq = izq;
  ultimoCen = cen;
  ultimoDer = der;
}