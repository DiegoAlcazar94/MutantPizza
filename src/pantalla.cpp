#include "pantalla.h"
#include "minijuego.h"

static uint8_t frameActual = 0;

// ============================================================
// DIBUJAR ICONOS (siempre visibles en pantalla principal)
// ============================================================
static void dibujarIconos() {
  // Nombres de las funciones
  const char* iconosArriba[] = { "COME", "LUZ", "JUGA", "CURA" };
  const char* iconosAbajo[]  = { "LIMP", "STAT", "DISC", "(!)" };

  // Fila de arriba (y=0 a y=20)
  for (int i = 0; i < 4; i++) {
    int x = i * 40;

    // Fondo resaltado si está seleccionado
    if (juego.iconoSeleccionado == i) {
      tft.fillRect(x, 0, 39, 20, COLOR_NARANJA);
      tft.setTextColor(COLOR_NEGRO);
    } else {
      tft.fillRect(x, 0, 39, 20, COLOR_NEGRO);
      tft.setTextColor(COLOR_BLANCO);
    }

    tft.setTextSize(1);
    tft.setCursor(x + 2, 6);
    tft.print(iconosArriba[i]);
  }

  // Fila de abajo (y=108 a y=128)
  for (int i = 0; i < 4; i++) {
    int x = i * 40;

    // El icono 7 (estado alterado) solo aparece si hay problema
    if (i == 3) {
      bool hayProblema = juego.enferma ||
                         juego.sucia   ||
                         juego.desobediencia >= 4 ||
                         juego.temperatura == 0   ||
                         juego.felicidad == 0;

      tft.fillRect(x, 108, 39, 20, COLOR_NEGRO);

      if (hayProblema) {
        tft.setTextColor(COLOR_ROJO);
        tft.setTextSize(1);
        tft.setCursor(x + 2, 114);
        tft.print("(!)");
      }
      continue; // no dibujamos resaltado ni texto normal para este icono
    }

    // Iconos 4, 5 y 6 (posiciones 0,1,2 de la fila de abajo)
    if (juego.iconoSeleccionado == i + 4) {
      tft.fillRect(x, 108, 39, 20, COLOR_NARANJA);
      tft.setTextColor(COLOR_NEGRO);
    } else {
      tft.fillRect(x, 108, 39, 20, COLOR_NEGRO);
      tft.setTextColor(COLOR_BLANCO);
    }

    tft.setTextSize(1);
    tft.setCursor(x + 2, 114);
    tft.print(iconosAbajo[i]);
  }

  // Líneas separadoras
  tft.drawLine(0, 20,  160, 20,  COLOR_GRIS);
  tft.drawLine(0, 107, 160, 107, COLOR_GRIS);
}

// ============================================================
// PANTALLA PRINCIPAL — mascota con iconos
// ============================================================
static void dibujarMascota() {
  // Solo borramos la zona central (entre los iconos)
  tft.fillRect(0, 21, 160, 86, COLOR_NEGRO);

  // Reloj arriba a la derecha (dentro de la zona central)
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(108, 24);
  if (juego.hora < 10) tft.print("0");
  tft.print(juego.hora);
  tft.print(":");
  if (juego.minutos < 10) tft.print("0");
  tft.print(juego.minutos);

  // Copo de nieve si temperatura <= 2
  if (juego.temperatura <= 2) {
    tft.setTextColor(COLOR_AZUL);
    tft.setTextSize(2);
    tft.setCursor(5, 24);
    tft.print("*");
  }

  // Zs de sueño si es de noche y luz apagada
  if (juego.durmiendo && juego.luzApagada) {
    tft.setTextColor(COLOR_BLANCO);
    tft.setTextSize(1);
    tft.setCursor(70, 35);
    if (frameActual == 0) {
      tft.print("z");
    } else {
      tft.print("z z");
    }
  }

  // Mascota centrada en la zona disponible (y=21 a y=107)
  tft.setTextColor(COLOR_BLANCO);
  tft.setTextSize(1);
  tft.setCursor(60, 60);
  tft.print("[MASCOTA]");

  // Indicadores de estado dentro de la zona central
  if (juego.sucia) {
    tft.setTextColor(COLOR_NARANJA);
    tft.setCursor(5, 95);
    tft.print("~ sucia");
  }
  if (juego.enferma) {
    tft.setTextColor(COLOR_ROJO);
    tft.setCursor(100, 95);
    tft.print("+ enferma");
  }

  // Redibujar iconos encima para que no se borren
  dibujarIconos();
}

// ============================================================
// PANTALLA DEL HUEVO
// ============================================================
static void dibujarHuevo() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_BLANCO);
  tft.setTextSize(1);
  tft.setCursor(10, 50);
  if (frameActual == 0) {
    tft.print("  [RIDER ->]   ");
  } else {
    tft.print("   [RIDER ->]  ");
  }
  tft.setCursor(20, 75);
  tft.print("Pulsa CENTRO");
  tft.setCursor(20, 87);
  tft.print("para poner hora");
}

// ============================================================
// PANTALLA SETEAR HORA
// ============================================================
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

// ============================================================
// FUNCIONES — pantalla completa sin iconos
// ============================================================
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
  delay(750);

  tft.fillRect(55, 50, 70, 16, COLOR_NEGRO);
  tft.setTextColor(COLOR_AMARILLO);
  tft.setCursor(55, 50);
  tft.print("[ *pizza* ]");
  delay(750);

  juego.temperatura  = 5;
  juego.diasSinCalor = 0;
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
  static uint8_t pantallaAnterior = PANTALLA_MENU;
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
  tft.setCursor(5, 70);  tft.print("SUCIA:    "); tft.print(juego.sucia    ? "SI" : "NO");
  tft.setCursor(5, 85);  tft.print("ENFERMA:  "); tft.print(juego.enferma  ? "SI" : "NO");
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

static void dibujarMuerte() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_ROJO);
  tft.setTextSize(2);
  tft.setCursor(30, 30);
  tft.print("FIN");
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(15, 80);
  tft.print("Pulsa CENTRO");
  tft.setCursor(15, 95);
  tft.print("para reiniciar");
}

static void dibujarEvolucion() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_AMARILLO);
  tft.setTextSize(2);
  tft.setCursor(10, 40);
  tft.print("EVOLUCION!");
  tft.setTextSize(1);
  tft.setTextColor(COLOR_BLANCO);
  tft.setCursor(30, 70);
  tft.print("Fase ");
  tft.print(juego.fase);
  for (int i = 100; i >= 0; i -= 10) {
    tft.fillRect(14, 90, i, 8, COLOR_NARANJA);
    tft.drawRect(14, 90, 100, 8, COLOR_BLANCO);
    delay(200);
  }
  juego.pantallaActual = PANTALLA_MASCOTA;
  tft.fillScreen(COLOR_NEGRO);
}

// ============================================================
// FUNCIÓN PRINCIPAL
// ============================================================
void actualizarPantalla() {
  frameActual = !frameActual;

  switch (juego.pantallaActual) {
    case PANTALLA_ANIMACION_HUEVO:   dibujarHuevo();          break;
    case PANTALLA_SETEAR_HORA:       dibujarSetearHora();     break;
    case PANTALLA_MASCOTA:           dibujarMascota();        break;
    case PANTALLA_COMER:             dibujarComer();          break;
    case PANTALLA_LUZ:               dibujarLuz();            break;
    case PANTALLA_JUGAR:             dibujarJugar();          break;
    case PANTALLA_CURAR:             dibujarCurar();          break;
    case PANTALLA_LIMPIAR:           dibujarLimpiar();        break;
    case PANTALLA_STATS:             dibujarStats();          break;
    case PANTALLA_DISCIPLINA:        dibujarDisciplina();     break;
    case PANTALLA_ESTADO_ALTERADO:   dibujarEstadoAlterado(); break;
    case PANTALLA_MUERTE:            dibujarMuerte();         break;
    case PANTALLA_EVOLUCION:         dibujarEvolucion();      break;
  }
}