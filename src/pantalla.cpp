#include "pantalla.h"
#include "minijuego.h"
#include "sprites/sprites.h"
#include "sonidos.h"
#include <pgmspace.h>

static uint8_t frameActual = 0;
static int posX = 64; 
static int dirX = 2;
static bool necesidadRedibujarFondo = true;

// Buffer en RAM para hacer la copia de seguridad de la zona donde se moverá la mascota (hasta 56x56)
static uint16_t bufferFondoMascota[56 * 56]; 
static int posXAnterior = 64;
static int posYAnterior = 48;
static int tamanoAnterior = 56;
static bool primerRenderMascota = true;

#define COLOR_FONDO_HUEVO 0xE734

// 1. Guarda en la RAM los píxeles del fondo exactos donde se va a dibujar la mascota
static void guardarFondoBajoMascota(int16_t x, int16_t y, int16_t w, int16_t h) {
  tft.startWrite();
  for (int16_t j = 0; j < h; j++) {
    int16_t py = y + j;
    if (py < 0 || py >= 128) continue;

    for (int16_t i = 0; i < w; i++) {
      int16_t px = x + i;
      if (px < 0 || px >= 160) continue;

      // Lectura desde Flash a la RAM solo en el área reducida de la mascota
      bufferFondoMascota[j * w + i] = pgm_read_word(&Background[py * 160 + px]);
    }
  }
  tft.endWrite();
}

// 2. Restaura desde la RAM la zona que fue pisada por la mascota en el frame anterior
static void restaurarFondoMascota() {
  if (primerRenderMascota) return; // Si es el primer frame, no hay nada que borrar

  // Dibuja directo desde la RAM (súper rápido)
  tft.drawRGBBitmap(posXAnterior, posYAnterior, bufferFondoMascota, tamanoAnterior, tamanoAnterior);
}

// Fuerza a que en el próximo frame se repinte el fondo y los iconos
void forzarRedibujadoPantalla() {
  necesidadRedibujarFondo = true;
}

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
  tft.drawRGBBitmap(0, 0, Background, 160, 128);
}

static void obtenerSprite(const uint16_t** sprite, int* tamano) {
  bool usarSleep = juego.durmiendo && juego.luzApagada;
  bool usarIll   = juego.enferma;

  switch (juego.fase) {
    case FASE_BEBE:
      *tamano = 32;
      if (usarSleep)    *sprite = (frameActual == 0) ? Baby_Sleep1    : Baby_Sleep2;
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
      tft.drawRect(i * 40 + 2, 0, 36, 20, COLOR_NARANJA);
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
      bool hayProblema = juego.enferma || juego.sucia || 
                         juego.desobediencia >= 4 || juego.temperatura == 0 || juego.felicidad == 0;
      if (hayProblema) dibujarSprite(Icon_Alarm, x, y, 16, 16);
      continue;
    }

    if (juego.iconoSeleccionado == i + 4) {
      tft.drawRect(i * 40 + 2, 108, 36, 20, COLOR_NARANJA);
    }
    dibujarSprite(iconosAbj[i], x, y, 16, 16);
  }
}

static void dibujarMascota() {
  const uint16_t* spriteActual = nullptr;
  int tamano = 32;
  obtenerSprite(&spriteActual, &tamano);

  int y = 20 + (88 - tamano) / 2;

  // 1. GESTIÓN DEL FONDO
  if (necesidadRedibujarFondo) {
    dibujarFondo();
    dibujarIconos();
    necesidadRedibujarFondo = false;
    primerRenderMascota = true;
  } else {
    // Restauramos únicamente el área que tapaba la mascota antes desde la RAM
    restaurarFondoMascota();
  }

  // 2. MOVER LA MASCOTA
  posX += dirX;
  if (posX <= 2) {
    posX = 2;
    dirX = -dirX;
  } else if (posX >= (160 - tamano - 2)) {
    posX = 160 - tamano - 2;
    dirX = -dirX;
  }

  // 3. RESPALDO DEL FONDO DE LA NUEVA POSICIÓN
  guardarFondoBajoMascota(posX, y, tamano, tamano);

  // Guardamos las coordenadas para el siguiente frame
  posXAnterior = posX;
  posYAnterior = y;
  tamanoAnterior = tamano;
  primerRenderMascota = false;

  // 4. DIBUJAR MASCOTA Y TEXTOS/ALERTAS
  dibujarSprite(spriteActual, posX, y, tamano, tamano);

  if (juego.temperatura <= 2) {
    tft.setTextColor(COLOR_AZUL);
    tft.setTextSize(2);
    tft.setCursor(5, 24);
    tft.print("*");
  }

  if (juego.durmiendo && juego.luzApagada) {
    tft.setTextColor(COLOR_NEGRO);
    tft.setTextSize(1);
    int xText = (posX + tamano + 10 < 150) ? (posX + tamano + 2) : (posX - 20);
    tft.setCursor(xText, y);
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
}

// FASE HUEVO
static int xBush1 = 20;
static int xBush2 = 100;

static void dibujarHuevo() {
  tft.fillRect(0, 0, 160, 128, COLOR_FONDO_HUEVO);

  xBush1 -= 4;
  xBush2 -= 4;

  if (xBush1 > 160) xBush1 = -16;
  if (xBush2 > 160) xBush2 = -16;

  dibujarSprite(Deco_Bush, xBush1, 60, 16, 16);
  dibujarSprite(Deco_Bush, xBush2, 60, 16, 16);

  int xRider = 48;
  int yRider = 30;

  if (frameActual == 0) {
    dibujarSprite(Rider_Frame0, xRider, yRider, 64, 64);
  } else {
    dibujarSprite(Rider_Frame1, xRider, yRider, 64, 64);
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
  forzarRedibujadoPantalla();
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
  if (juego.enferma)            { tft.setCursor(5, y); tft.print("! ENFERMA");            y += 15; }
  if (juego.sucia)              { tft.setCursor(5, y); tft.print("! SUCIA");              y += 15; }
  if (juego.desobediencia >= 4) { tft.setCursor(5, y); tft.print("! PELIGRO MALDICION"); y += 15; }
  if (juego.temperatura == 0)   { tft.setCursor(5, y); tft.print("! SIN TEMPERATURA");   y += 15; }
  if (juego.felicidad == 0)     { tft.setCursor(5, y); tft.print("! SIN FELICIDAD");      y += 15; }
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(5, 110);
  tft.print("DER: volver");
}

static void dibujarMuerte() {
  if (!juego.sonidoMuerteReproducido) {
    sonarMuerte();
    juego.sonidoMuerteReproducido = true;
  }

  tft.fillScreen(COLOR_BLANCO);
  dibujarSprite(frameActual == 0 ? Ghost_Frame0 : Ghost_Frame1, 20, 20, 32, 42);

  tft.setTextColor(COLOR_ROJO);
  tft.setTextSize(1);
  tft.setCursor(58, 25);
  tft.print("DESCANSA");
  tft.setCursor(58, 37);
  tft.print("EN PAZ");
  
  tft.setTextColor(COLOR_NEGRO);
  tft.setCursor(58, 52);
  tft.print("Murio: ");
  if (juego.hora < 10) tft.print("0");
  tft.print(juego.hora);
  tft.print(":");
  if (juego.minutos < 10) tft.print("0");
  tft.print(juego.minutos);

  if (juego.botonesLateralesMuerte && juego.tiempoBotonMuerte > 0) {
    unsigned long transcurrido = millis() - juego.tiempoBotonMuerte;
    int progreso = map(transcurrido, 0, SEGUNDOS_REINICIO * 1000, 0, 100);
    progreso = constrain(progreso, 0, 100);
    tft.fillRect(14, 90, progreso, 6, COLOR_ROJO);
    tft.drawRect(14, 90, 100, 6, COLOR_NEGRO);
  }

  tft.setTextColor(COLOR_NEGRO);
  tft.setTextSize(1);
  tft.setCursor(5, 105);
  tft.print("[manten laterales 10s]");
}

static void dibujarEvolucion() {
  sonarEvolucion();

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

  const uint16_t* spriteNuevo = nullptr;
  int tamanoFinal = 32;
  obtenerSprite(&spriteNuevo, &tamanoFinal);

  for (int tam = tamanoFinal * 2; tam >= tamanoFinal; tam -= tamanoFinal / 2) {
    tft.fillRect(0, 45, 160, 80, COLOR_NEGRO);
    int xi = (160 - tam) / 2;
    int yi = 45 + (80 - tam) / 2;
    tft.drawRect(xi, yi, tam, tam, COLOR_AMARILLO);
    delay(300);
  }

  tft.fillRect(0, 45, 160, 80, COLOR_NEGRO);
  int xf = (160 - tamanoFinal) / 2;
  int yf = 45 + (80 - tamanoFinal) / 2;
  dibujarSprite(spriteNuevo, xf, yf, tamanoFinal, tamanoFinal);
  delay(1500);

  juego.pantallaActual = PANTALLA_MASCOTA;
  forzarRedibujadoPantalla();
  tft.fillScreen(COLOR_NEGRO);
}

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