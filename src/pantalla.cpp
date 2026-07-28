#include "pantalla.h"

// Frame actual de animación (alterna entre 0 y 1 cada 500ms)
static uint8_t frameActual = 0;

// --- Pantalla de animación del huevo (rider en moto) ---
static void dibujarHuevo() {
  // Placeholder hasta que tengas los sprites
  // Alterna entre frame 0 y frame 1
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_BLANCO);
  tft.setTextSize(1);
  tft.setCursor(10, 50);
  if (frameActual == 0) {
    tft.print("  [RIDER ->]   ");
  } else {
    tft.print("   [RIDER ->]  ");
  }
  tft.setCursor(20, 70);
  tft.print("Pulsa CENTRO");
  tft.setCursor(20, 82);
  tft.print("para poner hora");
}

// --- Pantalla de configurar hora ---
static void dibujarSetearHora() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_AMARILLO);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.print("HORA:");

  // Mostrar hora actual
  tft.setTextColor(COLOR_BLANCO);
  tft.setCursor(20, 50);
  if (juego.hora < 10) tft.print("0");
  tft.print(juego.hora);
  tft.print(":");
  if (juego.minutos < 10) tft.print("0");
  tft.print(juego.minutos);

  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(5, 95);
  tft.print("IZQ:hora CEN:min DER:OK");
}

// --- Pantalla principal con la mascota ---
static void dibujarMascota() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_BLANCO);
  tft.setTextSize(1);
  tft.setCursor(50, 55);
  tft.print("[MASCOTA]");
  tft.setCursor(30, 75);
  tft.print("Pulsa CENTRO: menu");

  // Mostrar reloj arriba a la derecha
  tft.setCursor(100, 5);
  if (juego.hora < 10) tft.print("0");
  tft.print(juego.hora);
  tft.print(":");
  if (juego.minutos < 10) tft.print("0");
  tft.print(juego.minutos);
}

// --- Menú de 8 funciones ---
static void dibujarMenu() {
  tft.fillScreen(COLOR_NEGRO);

  // Nombres de las 8 funciones
  const char* funciones[] = {
    "COMER", "LUZ", "JUGAR", "CURAR",
    "LIMPIAR", "STATS", "DISCIP", "ESTADO"
  };

  // Fila de arriba: funciones 0-3
  for (int i = 0; i < 4; i++) {
    int x = i * 40 + 5;
    int y = 20;
    if (juego.iconoSeleccionado == i) {
      tft.fillRect(x - 3, y - 3, 36, 20, COLOR_NARANJA);
      tft.setTextColor(COLOR_NEGRO);
    } else {
      tft.setTextColor(COLOR_BLANCO);
    }
    tft.setTextSize(1);
    tft.setCursor(x, y);
    tft.print(funciones[i]);
  }

  // Fila de abajo: funciones 4-7
  for (int i = 4; i < 8; i++) {
    int x = (i - 4) * 40 + 5;
    int y = 70;
    if (juego.iconoSeleccionado == i) {
      tft.fillRect(x - 3, y - 3, 36, 20, COLOR_NARANJA);
      tft.setTextColor(COLOR_NEGRO);
    } else {
      tft.setTextColor(COLOR_BLANCO);
    }
    tft.setTextSize(1);
    tft.setCursor(x, y);
    tft.print(funciones[i]);
  }

  tft.setTextColor(COLOR_GRIS);
  tft.setTextSize(1);
  tft.setCursor(5, 110);
  tft.print("IZQ:navegar CEN:entrar DER:salir");
}

// --- Pantallas de funciones (placeholders) ---
static void dibujarComer() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_NARANJA);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.print("COMER");
}

static void dibujarLuz() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_AMARILLO);
  tft.setTextSize(2);
  tft.setCursor(20, 40);
  if (juego.luzApagada) {
    tft.print("LUZ: OFF");
  } else {
    tft.print("LUZ: ON");
  }
  // Al entrar en esta función alternamos la luz directamente
  juego.luzApagada = !juego.luzApagada;
}

static void dibujarJugar() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_VERDE);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.print("JUGAR");
}

static void dibujarCurar() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_AZUL);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.print("CURAR");
  if (juego.enferma) {
    juego.enferma        = false;
    juego.diasEnfermo    = 0;
  }
}

static void dibujarLimpiar() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_VERDE);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.print("LIMPIAR");
  if (juego.sucia) {
    juego.sucia          = false;
    juego.diasSinLimpiar = 0;
  }
}

static void dibujarStats() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_BLANCO);
  tft.setCursor(5, 10);  tft.print("HAMBRE:   "); tft.print(juego.hambre);
  tft.setCursor(5, 25);  tft.print("ENERGIA:  "); tft.print(juego.energia);
  tft.setCursor(5, 40);  tft.print("FELICIDAD:"); tft.print(juego.felicidad);
  tft.setCursor(5, 55);  tft.print("DESOB:    "); tft.print(juego.desobediencia);
  tft.setCursor(5, 70);  tft.print("SUCIA:    "); tft.print(juego.sucia ? "SI" : "NO");
  tft.setCursor(5, 85);  tft.print("ENFERMA:  "); tft.print(juego.enferma ? "SI" : "NO");
  tft.setCursor(5, 100); tft.print("FASE:     "); tft.print(juego.fase);
}

static void dibujarDisciplina() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_ROJO);
  tft.setTextSize(2);
  tft.setCursor(10, 40);
  tft.print("DISCIPLINA");
  if (juego.desobediencia > 0) juego.desobediencia--;
}

static void dibujarEstadoAlterado() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_ROJO);
  tft.setTextSize(1);
  tft.setCursor(5, 20);
  tft.print("ESTADO ALTERADO");
  tft.setCursor(5, 40);
  if (juego.enferma)             tft.print("! ENFERMA");
  if (juego.sucia)               { tft.setCursor(5, 55); tft.print("! SUCIA"); }
  if (juego.desobediencia >= 4)  { tft.setCursor(5, 70); tft.print("! PELIGRO MALDICION"); }
  if (juego.hambre == 0)         { tft.setCursor(5, 85); tft.print("! SIN HAMBRE"); }
}

static void dibujarMuerte() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_ROJO);
  tft.setTextSize(2);
  tft.setCursor(20, 30);
  tft.print("FIN");
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(15, 70);
  tft.print("Pulsa CENTRO");
  tft.setCursor(15, 85);
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

  // Barra de progreso
  for (int i = 100; i >= 0; i -= 10) {
    tft.fillRect(14, 90, i, 8, COLOR_NARANJA);
    tft.drawRect(14, 90, 100, 8, COLOR_BLANCO);
    delay(200);
  }

  // Al terminar vuelve a la pantalla de mascota
  juego.pantallaActual = PANTALLA_MASCOTA;
  tft.fillScreen(COLOR_NEGRO);
}

// ============================================================
// FUNCIÓN PRINCIPAL — se llama cada 500ms desde main.cpp
// ============================================================
void actualizarPantalla() {
  frameActual = !frameActual; // alterna 0/1 para animaciones

  switch (juego.pantallaActual) {
    case PANTALLA_ANIMACION_HUEVO:    dibujarHuevo();          break;
    case PANTALLA_SETEAR_HORA:        dibujarSetearHora();     break;
    case PANTALLA_MASCOTA:            dibujarMascota();        break;
    case PANTALLA_MENU:               dibujarMenu();           break;
    case PANTALLA_COMER:              dibujarComer();          break;
    case PANTALLA_LUZ:                dibujarLuz();            break;
    case PANTALLA_JUGAR:              dibujarJugar();          break;
    case PANTALLA_CURAR:              dibujarCurar();          break;
    case PANTALLA_LIMPIAR:            dibujarLimpiar();        break;
    case PANTALLA_STATS:              dibujarStats();          break;
    case PANTALLA_DISCIPLINA:         dibujarDisciplina();     break;
    case PANTALLA_ESTADO_ALTERADO:    dibujarEstadoAlterado(); break;
    case PANTALLA_MUERTE:             dibujarMuerte();         break;
    case PANTALLA_EVOLUCION:          dibujarEvolucion();      break;
  }
}