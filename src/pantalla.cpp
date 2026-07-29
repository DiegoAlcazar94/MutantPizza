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

  // Reloj arriba a la derecha
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(110, 5);
  if (juego.hora < 10) tft.print("0");
  tft.print(juego.hora);
  tft.print(":");
  if (juego.minutos < 10) tft.print("0");
  tft.print(juego.minutos);

  // Copo de nieve arriba a la izquierda si temperatura <= 2
  if (juego.temperatura <= 2) {
    tft.setTextColor(COLOR_AZUL);
    tft.setTextSize(2);
    tft.setCursor(5, 2);
    tft.print("*");  // placeholder del copo hasta tener sprite
  }

  // Zs de sueño si es de noche y luz apagada
  if (juego.durmiendo && juego.luzApagada) {
    tft.setTextColor(COLOR_BLANCO);
    tft.setTextSize(1);
    tft.setCursor(70, 20);
    if (frameActual == 0) {
      tft.print("z");
    } else {
      tft.print("z z");
    }
  }

  // Mascota en el centro (placeholder hasta tener sprites)
  tft.setTextColor(COLOR_BLANCO);
  tft.setTextSize(1);
  tft.setCursor(50, 60);
  tft.print("[MASCOTA]");

  // Indicador de suciedad abajo a la izquierda
  if (juego.sucia) {
    tft.setTextColor(COLOR_NARANJA);
    tft.setCursor(5, 110);
    tft.print("~ sucia ~");
  }

  // Indicador de enfermedad abajo a la derecha
  if (juego.enferma) {
    tft.setTextColor(COLOR_ROJO);
    tft.setCursor(100, 110);
    tft.print("+ enferma");
  }

  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(35, 100);
  tft.print("CENTRO: menu");
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
  // Solo ejecutamos la animación una vez al entrar
  // El copo de nieve en pantalla principal avisará cuando haga falta

  // Frame 1 — microondas cerrado
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_BLANCO);
  tft.setTextSize(1);
  tft.setCursor(40, 30);
  tft.print("[MICROONDAS]");
  tft.setCursor(55, 50);
  tft.print("[  pizza  ]");
  tft.setCursor(55, 65);
  tft.print("[_________]");
  tft.setCursor(40, 85);
  tft.setTextColor(COLOR_GRIS);
  tft.print("Precalentando...");
  delay(750);

  // Frame 2 — microondas girando (punto rotatorio simple)
  tft.fillRect(55, 50, 70, 16, COLOR_NEGRO);
  tft.setTextColor(COLOR_AMARILLO);
  tft.setCursor(55, 50);
  tft.print("[ *pizza* ]");
  delay(750);

  // Subir temperatura al máximo
  juego.temperatura    = 5;
  juego.diasSinCalor   = 0;

  // Volver al menú automáticamente
  juego.pantallaActual = PANTALLA_MENU;
}

static void dibujarLuz() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextSize(2);
  tft.setCursor(20, 40);
  if (juego.luzApagada) {
    tft.setTextColor(COLOR_GRIS);
    tft.print("LUZ: OFF");
  } else {
    tft.setTextColor(COLOR_AMARILLO);
    tft.print("LUZ:  ON");
  }
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(20, 90);
  tft.print("DER: volver");
}

static void dibujarCurar() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_AZUL);
  tft.setCursor(20, 30);
  tft.print("CURAR");
  if (juego.enferma) {
    tft.setTextColor(COLOR_BLANCO);
    tft.setCursor(10, 55);
    tft.print("Mascota enferma.");
    tft.setCursor(10, 70);
    tft.print("CENTRO: curar");
  } else {
    tft.setTextColor(COLOR_VERDE);
    tft.setCursor(10, 55);
    tft.print("Esta sanita!");
  }
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(10, 100);
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
    tft.setCursor(10, 55);
    tft.print("Mascota sucia.");
    tft.setCursor(10, 70);
    tft.print("CENTRO: limpiar");
  } else {
    tft.setTextColor(COLOR_VERDE);
    tft.setCursor(10, 55);
    tft.print("Esta limpita!");
  }
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(10, 100);
  tft.print("DER: volver");
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
  tft.setCursor(10, 100);
  tft.print("DER: volver");
}

static void dibujarStats() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_BLANCO);
  tft.setCursor(5, 10);  tft.print("TEMP:"); tft.print(juego.temperatura);
  tft.setCursor(5, 25);  tft.print("ENERGIA:  "); tft.print(juego.energia);
  tft.setCursor(5, 40);  tft.print("FELICIDAD:"); tft.print(juego.felicidad);
  tft.setCursor(5, 55);  tft.print("DESOB:    "); tft.print(juego.desobediencia);
  tft.setCursor(5, 70);  tft.print("SUCIA:    "); tft.print(juego.sucia ? "SI" : "NO");
  tft.setCursor(5, 85);  tft.print("ENFERMA:  "); tft.print(juego.enferma ? "SI" : "NO");
  tft.setCursor(5, 100); tft.print("FASE:     "); tft.print(juego.fase);
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
  if (juego.temperatura == 0)         { tft.setCursor(5, 85); tft.print("! SIN CALOR"); }
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

static void dibujarJugar() {
  tft.fillScreen(COLOR_NEGRO);
  tft.setTextColor(COLOR_VERDE);
  tft.setTextSize(2);
  tft.setCursor(20, 50);
  tft.print("JUGAR");
  tft.setTextSize(1);
  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(10, 90);
  tft.print("Minijuego proximamente");
  tft.setCursor(10, 105);
  tft.print("DER: volver");
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