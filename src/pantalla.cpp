#include "pantalla.h"
#include "minijuego.h"
#include "sprites/sprites.h"
#include "sonidos.h"

static uint8_t frameActual = 0;

static void dibujarSprite(const uint16_t* sprite, int x, int y, int w, int h) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      uint16_t pixel = pgm_read_word(&sprite[j * w + i]);
      if (pixel != 0xFFFF) {
        tft.drawPixel(x + i, y + j, pixel);
      }
    }
  }
}

static void dibujarFondo() {
  for (int ty = 21; ty < 108; ty += 32) {
    for (int tx = 0; tx < 160; tx += 32) {
      tft.drawRGBBitmap(tx, ty, Background, 32, 32);
    }
  }
}

static void obtenerSprite(const uint16_t** sprite, int* tamano) {
  bool usarSleep = juego.durmiendo && juego.luzApagada;
  bool usarIll   = juego.enferma;

  switch (juego.fase) {
    case FASE_BEBE:
      *tamano = 32;
      if (usarSleep)    *sprite = (frameActual == 0) ? Baby_Sleep1      : Baby_Sleep2;
      else if (usarIll) *sprite = (frameActual == 0) ? Baby_Ill0_Sheet  : Baby_Ill1_Sheet;
      else              *sprite = (frameActual == 0) ? Baby_Idle0       : Baby_Idle1;
      break;
    case FASE_TODDLER:
      *tamano = 40;
      if (usarSleep)    *sprite = (frameActual == 0) ? Toddler_Sleep0   : Toddler_Sleep1;
      else if (usarIll) *sprite = (frameActual == 0) ? Toddler_Ill0     : Toddler_Ill1;
      else              *sprite = (frameActual == 0) ? Toddler_Idle0    : Toddler_Idle1;
      break;
    case FASE_ADOLESCENTE:
      *tamano = 48;
      if (usarSleep)    *sprite = (frameActual == 0) ? Teenager_Sleep0  : Teenager_Sleep1;
      else if (usarIll) *sprite = (frameActual == 0) ? Teenager_Ill0    : Teenager_Ill1;
      else              *sprite = (frameActual == 0) ? Teenager_Idle0   : Teenager_Idle1;
      break;
    case FASE_ADULTO:
      *tamano = 56;
      if (usarSleep)    *sprite = (frameActual == 0) ? Adult_Sleep0     : Adult_Sleep1;
      else if (usarIll) *sprite = (frameActual == 0) ? Adult_Ill0       : Adult_Ill1;
      else              *sprite = (frameActual == 0) ? Adult_Idle0      : Adult_Idle1;
      break;
    case FASE_MALDITO:
      *tamano = 56;
      if (usarSleep)    *sprite = (frameActual == 0) ? Cursed_Sleep0    : Cursed_Sleep1;
      else if (usarIll) *sprite = (frameActual == 0) ? Cursed_Ill0      : Cursed_Ill1;
      else              *sprite = (frameActual == 0) ? Cursed_Idle0     : Cursed_Idle1;
      break;
    default:
      *tamano = 32;
      *sprite = Baby_Idle0;
      break;
  }
}

static void dibujarIconos() {
  const uint16_t* iconosArr[] = {
    Icon_Eat,
    juego.luzApagada ? Icon_LightOff : Icon_LightOn1,
    Icon_Play,
    Icon_Heal
  };

  for (int i = 0; i < 4; i++) {
    int x = i * 40 + 12;
    int y = 2;
    if (juego.iconoSeleccionado == i) {
      tft.fillRect(i * 40, 0, 39, 20, COLOR_NARANJA);
    } else {
      tft.fillRect(i * 40, 0, 39, 20, COLOR_NEGRO);
    }
    dibujarSprite(iconosArr[i], x, y, 16, 16);
  }

  const uint16_t* iconosAbj[] = {
    Icon_Clean,
    Icon_Stats,
    Icon_Discipline,
    Icon_Alarm
  };

  for (int i = 0; i < 4; i++) {
    int x = i * 40 + 12;
    int y = 110;

    if (i == 3) {
      bool hayProblema = juego.enferma ||
                         juego.sucia   ||
                         juego.desobediencia >= 4 ||
                         juego.temperatura == 0   ||
                         juego.felicidad == 0;
      tft.fillRect(i * 40, 108, 39, 20, COLOR_NEGRO);
      if (hayProblema) dibujarSprite(Icon_Alarm, x, y, 16, 16);
      continue;
    }

    if (juego.iconoSeleccionado == i + 4) {
      tft.fillRect(i * 40, 108, 39, 20, COLOR_NARANJA);
    } else {
      tft.fillRect(i * 40, 108, 39, 20, COLOR_NEGRO);
    }
    dibujarSprite(iconosAbj[i], x, y, 16, 16);
  }

  tft.drawLine(0, 20,  160, 20,  COLOR_GRIS);
  tft.drawLine(0, 107, 160, 107, COLOR_GRIS);
}

static void dibujarMascota() {
  dibujarFondo();

  const uint16_t* spriteActual = nullptr;
  int tamano = 32;
  obtenerSprite(&spriteActual, &tamano);

  int x = (160 - tamano) / 2;
  int y = 21 + (86 - tamano) / 2;
  dibujarSprite(spriteActual, x, y, tamano, tamano);

  tft.setTextSize(1);
  tft.setTextColor(COLOR_NEGRO);
  tft.setCursor(108, 24);
  if (juego.hora < 10) tft.print("0");
  tft.print(juego.hora);
  tft.print(":");
  if (juego.minutos < 10) tft.print("0");
  tft.print(juego.minutos);

  if (juego.temperatura <= 2) {
    tft.setTextColor(COLOR_AZUL);
    tft.setTextSize(2);
    tft.setCursor(5, 24);
    tft.print("*");
  }

  if (juego.durmiendo && juego.luzApagada) {
    tft.setTextColor(COLOR_NEGRO);
    tft.setTextSize(1);
    tft.setCursor(x + tamano + 2, y);
    tft.print(frameActual == 0 ? "z" : "z z");
  }

  if (juego.sucia) {
    tft.setTextColor(COLOR_NARANJA);
    tft.setTextSize(1);
    tft.setCursor(5, 95);
    tft.print("~ sucia");
  }
  if (juego.enferma) {
    tft.setTextColor(COLOR_ROJO);
    tft.setTextSize(1);
    tft.setCursor(100, 95);
    tft.print("+ enferma");
  }

  dibujarIconos();
}

// ============================================================
// HUEVO — rider con setos pasando por detrás
// xBush1 y xBush2 son las posiciones X de los dos setos
// Se actualizan cada frame para dar sensación de movimiento
// Cuando llegan al borde izquierdo vuelven a aparecer a la derecha
// ============================================================
static int xBush1 = 20;
static int xBush2 = 100;

static void dibujarHuevo() {
  // Fondo completo
  for (int ty = 0; ty < 128; ty += 32) {
    for (int tx = 0; tx < 160; tx += 32) {
      tft.drawRGBBitmap(tx, ty, Background, 32, 32);
    }
  }

  // Mover setos de derecha a izquierda
  // Borramos la posición anterior antes de mover
  tft.fillRect(xBush1, 60, 16, 16, 0xFFFF); // borra seto 1
  tft.fillRect(xBush2, 60, 16, 16, 0xFFFF); // borra seto 2

  xBush1 -= 4;
  xBush2 -= 4;

  if (xBush1 < -16) xBush1 = 160;
  if (xBush2 < -16) xBush2 = 160;

  // Dibujar setos en nueva posición
  // PLACEHOLDER: rectángulo verde hasta tener el sprite Deco_Bush
  tft.fillRect(xBush1, 60, 16, 16, COLOR_VERDE);
  tft.fillRect(xBush2, 60, 16, 16, COLOR_VERDE);
  // Cuando tengas Deco_Bush listo sustituye las dos líneas anteriores por:
  // dibujarSprite(Deco_Bush, xBush1, 60, 16, 16);
  // dibujarSprite(Deco_Bush, xBush2, 60, 16, 16);

  // Rider fijo en el centro, encima de los setos
  int xRider = 48;
  int yRider = 50;
  // Alterna frame para simular movimiento de ruedas
  if (frameActual == 0) {
    dibujarSprite(Rider_Frame0, xRider, yRider, 64, 32);
  } else {
    dibujarSprite(Rider_Frame1, xRider, yRider, 64, 32);
  }

  if (!juego.horaConfigurada) {
    tft.setTextColor(COLOR_NEGRO);
    tft.setTextSize(1);
    tft.setCursor(15, 105);
    tft.print("CENTRO: poner hora");
  }
}

static void dibujarSetearHora() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_AMARILLO);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.print("HORA:");
  tft.setTextColor(COLOR_BLANCO);
  tft.setCursor(20, 50);
  if (juego.hora < 10) tft.print("0");
  tft.print(juego.hora);
  tft.print(":");
  if (juego.minutos < 10) tft.print("0");
  tft.print(juego.minutos);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(5, 100);
  tft.print("IZQ:hora CEN:min DER:OK");
}

static void dibujarComer() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_BLANCO);
  tft.setTextSize(1);
  tft.setCursor(40, 30);
  tft.print("[MICROONDAS]");
  tft.setCursor(55, 50);
  tft.print("[  pizza  ]");
  tft.setCursor(55, 65);
  tft.print("[_________]");
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(40, 85);
  tft.print("Precalentando...");
  sonarComer();
  delay(750);

  tft.fillRect(55, 50, 70, 16, COLOR_NEGRO);
  tft.setTextColor(COLOR_AMARILLO);
  tft.setCursor(55, 50);
  tft.print("[ *pizza* ]");
  delay(750);

  juego.temperatura    = 5;
  juego.diasSinCalor   = 0;
  juego.pantallaActual = PANTALLA_MASCOTA;
  tft.fillScreen(COLOR_NEGRO);
}

static void dibujarLuz() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  if (juego.luzApagada) {
    tft.setTextColor(COLOR_GRIS);
    tft.print("LUZ: OFF");
  } else {
    tft.setTextColor(COLOR_AMARILLO);
    tft.print("LUZ:  ON");
  }
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(20, 100);
  tft.print("DER: volver");
}

static void dibujarJugar() {
  static uint8_t pantallaAnterior = PANTALLA_MASCOTA;
  if (pantallaAnterior != PANTALLA_JUGAR) {
    iniciarMinijuego();
  }
  pantallaAnterior = juego.pantallaActual;
  actualizarMinijuego();
}

static void dibujarCurar() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_AZUL);
  tft.setCursor(20, 30);
  tft.print("CURAR");
  if (juego.enferma) {
    tft.setTextColor(COLOR_BLANCO);
    tft.setCursor(10, 60);
    tft.print("Mascota enferma.");
    tft.setCursor(10, 75);
    tft.print("CENTRO: curar");
  } else {
    tft.setTextColor(COLOR_VERDE);
    tft.setCursor(10, 60);
    tft.print("Esta sanita!");
  }
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(10, 110);
  tft.print("DER: volver");
}

static void dibujarLimpiar() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_VERDE);
  tft.setCursor(20, 30);
  tft.print("LIMPIAR");
  if (juego.sucia) {
    tft.setTextColor(COLOR_BLANCO);
    tft.setCursor(10, 60);
    tft.print("Mascota sucia.");
    tft.setCursor(10, 75);
    tft.print("CENTRO: limpiar");
  } else {
    tft.setTextColor(COLOR_VERDE);
    tft.setCursor(10, 60);
    tft.print("Esta limpita!");
  }
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(10, 110);
  tft.print("DER: volver");
}

static void dibujarStats() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_BLANCO);
  tft.setCursor(5, 10);  tft.print("TEMP:     "); tft.print(juego.temperatura);
  tft.setCursor(5, 25);  tft.print("ENERGIA:  "); tft.print(juego.energia);
  tft.setCursor(5, 40);  tft.print("FELICIDAD:"); tft.print(juego.felicidad);
  tft.setCursor(5, 55);  tft.print("DESOB:    "); tft.print(juego.desobediencia);
  tft.setCursor(5, 70);  tft.print("SUCIA:    "); tft.print(juego.sucia   ? "SI" : "NO");
  tft.setCursor(5, 85);  tft.print("ENFERMA:  "); tft.print(juego.enferma ? "SI" : "NO");
  tft.setCursor(5, 100); tft.print("FASE:     "); tft.print(juego.fase);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(5, 115); tft.print("DER: volver");
}

static void dibujarDisciplina() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_ROJO);
  tft.setCursor(10, 30);
  tft.print("DISCIPLINA");
  tft.setTextColor(COLOR_BLANCO);
  tft.setCursor(10, 55);
  tft.print("Desob: ");
  tft.print(juego.desobediencia);
  tft.print("/5");
  if (juego.desobediencia > 0) {
    tft.setCursor(10, 75);
    tft.print("CENTRO: reprimenda");
  } else {
    tft.setTextColor(COLOR_VERDE);
    tft.setCursor(10, 75);
    tft.print("Bien portada!");
  }
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(10, 110);
  tft.print("DER: volver");
}

static void dibujarEstadoAlterado() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_ROJO);
  tft.setTextSize(1);
  tft.setCursor(5, 15);
  tft.print("ESTADO ALTERADO");
  tft.setTextColor(COLOR_BLANCO);
  int y = 35;
  if (juego.enferma)            { tft.setCursor(5, y); tft.print("! ENFERMA");           y += 15; }
  if (juego.sucia)              { tft.setCursor(5, y); tft.print("! SUCIA");              y += 15; }
  if (juego.desobediencia >= 4) { tft.setCursor(5, y); tft.print("! PELIGRO MALDICION"); y += 15; }
  if (juego.temperatura == 0)   { tft.setCursor(5, y); tft.print("! SIN TEMPERATURA");   y += 15; }
  if (juego.felicidad == 0)     { tft.setCursor(5, y); tft.print("! SIN FELICIDAD");      y += 15; }
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(5, 110);
  tft.print("DER: volver");
}

// ============================================================
// PANTALLA DE MUERTE — fantasmita + fecha + reinicio 10s
// ============================================================
static void dibujarMuerte() {
  if (!juego.sonidoMuerteReproducido) {
    sonarMuerte();
    juego.sonidoMuerteReproducido = true;
  }

  tft.fillScreen(COLOR_NEGRO);

  // Fantasmita — PLACEHOLDER hasta tener Ghost_Frame0/1
  // Sustituye el fillRect por dibujarSprite(Ghost_Frame0/1, ...)
  tft.fillRect(20, 30, 16, 16, COLOR_BLANCO);
  // dibujarSprite(frameActual == 0 ? Ghost_Frame0 : Ghost_Frame1, 20, 30, 16, 16);

  // Texto de muerte
  tft.setTextColor(COLOR_ROJO);
  tft.setTextSize(1);
  tft.setCursor(45, 30);
  tft.print("DESCANSA EN PAZ");
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(45, 45);
  tft.print("Murio el ");
  if (juego.hora < 10) tft.print("0");
  tft.print(juego.hora);
  tft.print(":");
  if (juego.minutos < 10) tft.print("0");
  tft.print(juego.minutos);

  // Barra de progreso del reinicio
  if (juego.botonesLateralesMuerte && juego.tiempoBotonMuerte > 0) {
    unsigned long transcurrido = millis() - juego.tiempoBotonMuerte;
    int progreso = map(transcurrido, 0, SEGUNDOS_REINICIO * 1000, 0, 100);
    progreso = constrain(progreso, 0, 100);
    tft.fillRect(14, 90, progreso, 6, COLOR_ROJO);
    tft.drawRect(14, 90, 100, 6, COLOR_GRIS);
  }

  tft.setTextColor(COLOR_GRIS);
  tft.setTextSize(1);
  tft.setCursor(5, 105);
  tft.print("[manten laterales 10s]");
}

// ============================================================
// PANTALLA DE EVOLUCIÓN — parpadeo + sprite grande que encoge
// ============================================================
static void dibujarEvolucion() {
  sonarEvolucion();

  // Parpadeo 3 veces
  for (int p = 0; p < 3; p++) {
    tft.fillScreen(COLOR_NEGRO);
    delay(150);
    tft.fillScreen(COLOR_BLANCO);
    delay(150);
  }

  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_AMARILLO);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.print("EVOLUCION!");
  tft.setTextSize(1);
  tft.setTextColor(COLOR_BLANCO);
  tft.setCursor(30, 35);
  tft.print("Fase ");
  tft.print(juego.fase);

  // Obtener sprite de la nueva fase
  const uint16_t* spriteNuevo = nullptr;
  int tamanoFinal = 32;
  obtenerSprite(&spriteNuevo, &tamanoFinal);

  // Animación: aparece grande y encoge hasta tamaño real
  // Dibujamos en 3 pasos usando drawRGBBitmap con escalado
  // Como ST7735 no escala, simulamos con rectángulos de relleno
  for (int tam = tamanoFinal * 2; tam >= tamanoFinal; tam -= tamanoFinal / 2) {
    tft.fillRect(0, 45, 160, 80, COLOR_NEGRO);
    int xi = (160 - tam) / 2;
    int yi = 45 + (80 - tam) / 2;
    // Placeholder: rectángulo del tamaño del sprite
    tft.drawRect(xi, yi, tam, tam, COLOR_AMARILLO);
    // Cuando quieras sustituir por el sprite real descomenta esto
    // y comenta el drawRect de arriba:
    // dibujarSprite(spriteNuevo, xi, yi, tamanoFinal, tamanoFinal);
    delay(300);
  }

  // Frame final: sprite real centrado
  tft.fillRect(0, 45, 160, 80, COLOR_NEGRO);
  int xf = (160 - tamanoFinal) / 2;
  int yf = 45 + (80 - tamanoFinal) / 2;
  dibujarSprite(spriteNuevo, xf, yf, tamanoFinal, tamanoFinal);
  delay(1500);

  juego.pantallaActual = PANTALLA_MASCOTA;
  tft.fillScreen(COLOR_NEGRO);
}

// ============================================================
// FUNCIÓN PRINCIPAL
// ============================================================
void actualizarPantalla() {
  frameActual = !frameActual;

  switch (juego.pantallaActual) {
    case PANTALLA_ANIMACION_HUEVO:  dibujarHuevo();          break;
    case PANTALLA_SETEAR_HORA:      dibujarSetearHora();     break;
    case PANTALLA_MASCOTA:          dibujarMascota();        break;
    case PANTALLA_COMER:            dibujarComer();          break;
    case PANTALLA_LUZ:              dibujarLuz();            break;
    case PANTALLA_JUGAR:            dibujarJugar();          break;
    case PANTALLA_CURAR:            dibujarCurar();          break;
    case PANTALLA_LIMPIAR:          dibujarLimpiar();        break;
    case PANTALLA_STATS:            dibujarStats();          break;
    case PANTALLA_DISCIPLINA:       dibujarDisciplina();     break;
    case PANTALLA_ESTADO_ALTERADO:  dibujarEstadoAlterado(); break;
    case PANTALLA_MUERTE:           dibujarMuerte();         break;
    case PANTALLA_EVOLUCION:        dibujarEvolucion();      break;
  }
}