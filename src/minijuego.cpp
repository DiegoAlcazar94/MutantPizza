#include "minijuego.h"

// ============================================================
// CONSTANTES DEL MINIJUEGO
// ============================================================
#define NUM_CARRILES     3
#define MAX_OBJETOS      4    // máximo de objetos en pantalla a la vez
#define VELOCIDAD_INICIO 4    // píxeles que baja cada frame
#define VIDAS_INICIO     3
#define COMIDA_PARA_GANAR 20

// Posiciones X del centro de cada carril
// Pantalla 160px / 3 carriles = ~53px por carril
const int X_CARRILES[3] = { 26, 80, 133 };

// Y donde está la mascota (fila de abajo)
#define Y_MASCOTA  105
#define Y_INICIO   -16  // donde aparecen los objetos (fuera de pantalla arriba)

// Tipos de objeto
#define OBJ_VACIO      0
#define OBJ_CHAMPINON  1
#define OBJ_PEPERONI   2
#define OBJ_BACON      3
#define OBJ_RATA       4

// ============================================================
// ESTADO DEL MINIJUEGO
// ============================================================
struct Objeto {
  int     y;          // posición vertical actual
  uint8_t carril;     // 0, 1 o 2
  uint8_t tipo;       // OBJ_VACIO, OBJ_CHAMPIÑON, etc.
  bool    activo;
};

static Objeto  objetos[MAX_OBJETOS];
static uint8_t carrilMascota;   // 0, 1 o 2
static uint8_t vidas;
static uint8_t comidaRecogida;
static uint8_t velocidad;
static uint8_t framesSinSpawn; // contador para saber cuándo spawnear
static uint8_t spawnsHastaAhora;
bool    minijuegoActivo;
static bool    minijuegoGanado;

// Cuántos frames esperamos entre spawns (empieza en 30, baja con velocidad)
#define FRAMES_ENTRE_SPAWNS 25

// ============================================================
// DIBUJAR OBJETO en pantalla (placeholder de texto hasta tener sprites)
// ============================================================
static void dibujarObjeto(Objeto& obj) {
  if (!obj.activo) return;

  int x = X_CARRILES[obj.carril] - 8;

  switch (obj.tipo) {
    case OBJ_CHAMPINON: tft.setTextColor(COLOR_BLANCO);  break;
    case OBJ_PEPERONI:  tft.setTextColor(COLOR_ROJO);    break;
    case OBJ_BACON:     tft.setTextColor(COLOR_NARANJA); break;
    case OBJ_RATA:      tft.setTextColor(COLOR_GRIS);    break;
  }

  tft.setTextSize(1);
  tft.setCursor(x, obj.y);

  switch (obj.tipo) {
    case OBJ_CHAMPINON: tft.print("[C]"); break;
    case OBJ_PEPERONI:  tft.print("[P]"); break;
    case OBJ_BACON:     tft.print("[B]"); break;
    case OBJ_RATA:      tft.print("[R]"); break;
  }
}

// ============================================================
// BORRAR OBJETO de pantalla (pinta negro encima)
// ============================================================
static void borrarObjeto(Objeto& obj) {
  if (!obj.activo) return;
  int x = X_CARRILES[obj.carril] - 8;
  tft.fillRect(x, obj.y, 18, 10, COLOR_NEGRO);
}

// ============================================================
// SPAWNEAR un objeto nuevo en un carril aleatorio
// ============================================================
static void spawnearObjeto() {
  // Buscar un slot libre
  for (int i = 0; i < MAX_OBJETOS; i++) {
    if (!objetos[i].activo) {
      objetos[i].activo = true;
      objetos[i].y      = Y_INICIO;
      objetos[i].carril = random(0, 3);

      // 25% de probabilidad de rata, 75% de comida
      // Pero no más de 1 rata cada 5 spawns para no ser injusto
      if (random(0, 4) == 0 && spawnsHastaAhora % 5 == 0) {
        objetos[i].tipo = OBJ_RATA;
      } else {
        objetos[i].tipo = random(OBJ_CHAMPINON, OBJ_BACON + 1);
      }

      spawnsHastaAhora++;
      return;
    }
  }
}

// ============================================================
// DIBUJAR MASCOTA
// ============================================================
static void dibujarMascotaJuego() {
  // Borrar la fila de la mascota entera primero
  tft.fillRect(0, Y_MASCOTA, 160, 16, COLOR_NEGRO);

  tft.setTextColor(COLOR_AMARILLO);
  tft.setTextSize(1);
  int x = X_CARRILES[carrilMascota] - 8;
  tft.setCursor(x, Y_MASCOTA);
  tft.print("[M]"); // placeholder hasta tener sprite
}

// ============================================================
// DIBUJAR HUD (vidas, comida recogida)
// ============================================================
static void dibujarHUD() {
  tft.fillRect(0, 0, 160, 12, COLOR_NEGRO);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_ROJO);
  tft.setCursor(2, 2);
  for (int i = 0; i < vidas; i++) tft.print("v ");

  tft.setTextColor(COLOR_VERDE);
  tft.setCursor(90, 2);
  tft.print(comidaRecogida);
  tft.print("/");
  tft.print(COMIDA_PARA_GANAR);
}

// ============================================================
// PANTALLA DE FIN DE MINIJUEGO
// ============================================================
static void pantallaFinMinijuego() {
  tft.fillScreen(COLOR_NEGRO);

  if (minijuegoGanado) {
    tft.setTextColor(COLOR_VERDE);
    tft.setTextSize(2);
    tft.setCursor(20, 30);
    tft.print("GANASTE!");
    tft.setTextSize(1);
    tft.setTextColor(COLOR_BLANCO);
    tft.setCursor(10, 65);
    tft.print("Felicidad +1");

    // Subir felicidad
    if (juego.felicidad < 5) juego.felicidad++;

  } else {
    tft.setTextColor(COLOR_ROJO);
    tft.setTextSize(2);
    tft.setCursor(20, 30);
    tft.print("PERDISTE");
    tft.setTextSize(1);
    tft.setTextColor(COLOR_BLANCO);
    tft.setCursor(10, 65);

    // Bajar felicidad un 10% (redondeado, mínimo 0)
    int bajada = max(1, (int)(juego.felicidad * 0.1));
    juego.felicidad = (uint8_t)max(0, juego.felicidad - bajada);
    tft.print("Felicidad -10%");
  }

  tft.setTextColor(COLOR_GRIS);
  tft.setCursor(10, 100);
  tft.print("DER: volver al menu");

  // El minijuego ha terminado, esperamos que el jugador pulse DER
  minijuegoActivo = false;
}

// ============================================================
// INICIAR MINIJUEGO
// ============================================================
void iniciarMinijuego() {
  // Limpiar todos los objetos
  for (int i = 0; i < MAX_OBJETOS; i++) {
    objetos[i].activo = false;
    objetos[i].y      = Y_INICIO;
    objetos[i].carril = 0;
    objetos[i].tipo   = OBJ_VACIO;
  }

  carrilMascota    = 1;  // empieza en el carril central
  vidas            = VIDAS_INICIO;
  comidaRecogida   = 0;
  velocidad        = VELOCIDAD_INICIO;
  framesSinSpawn   = 0;
  spawnsHastaAhora = 0;
  minijuegoActivo  = true;
  minijuegoGanado  = false;

  tft.fillScreen(COLOR_NEGRO);

  // Líneas divisorias de carriles
  tft.drawLine(53,  12, 53,  Y_MASCOTA, COLOR_GRIS);
  tft.drawLine(106, 12, 106, Y_MASCOTA, COLOR_GRIS);

  dibujarHUD();
  dibujarMascotaJuego();
}

// ============================================================
// ACTUALIZAR MINIJUEGO — se llama cada 500ms desde pantalla.cpp
// ============================================================
void actualizarMinijuego() {
  if (!minijuegoActivo) return;

  // Mover todos los objetos hacia abajo
  for (int i = 0; i < MAX_OBJETOS; i++) {
    if (!objetos[i].activo) continue;

    // Borrar posición anterior
    borrarObjeto(objetos[i]);

    // Mover
    objetos[i].y += velocidad;

    // ¿Ha llegado a la fila de la mascota?
    if (objetos[i].y >= Y_MASCOTA - 4) {

      // ¿Está en el mismo carril que la mascota?
      if (objetos[i].carril == carrilMascota) {

        if (objetos[i].tipo == OBJ_RATA) {
          // Colisión con rata — perder vida
          vidas--;
          tft.fillRect(0, 0, 160, 12, COLOR_NEGRO);
          tft.setTextColor(COLOR_ROJO);
          tft.setTextSize(1);
          tft.setCursor(50, 2);
          tft.print("RATA!");
          delay(300);

          if (vidas == 0) {
            minijuegoGanado = false;
            pantallaFinMinijuego();
            return;
          }

        } else {
          // Colisión con comida — recoger
          comidaRecogida++;

          if (comidaRecogida >= COMIDA_PARA_GANAR) {
            minijuegoGanado = true;
            pantallaFinMinijuego();
            return;
          }
        }
      }

      // Desactivar objeto (salió de pantalla o fue recogido)
      objetos[i].activo = false;

    } else {
      // Dibujar en nueva posición
      dibujarObjeto(objetos[i]);
    }
  }

  // Spawnear nuevo objeto
  framesSinSpawn++;
  if (framesSinSpawn >= FRAMES_ENTRE_SPAWNS) {
    framesSinSpawn = 0;
    spawnearObjeto();

    // Aumentar velocidad muy suavemente cada 5 spawns
    if (spawnsHastaAhora % 5 == 0 && velocidad < 8) {
      velocidad++;
    }
  }

  dibujarHUD();
  dibujarMascotaJuego();
}

// ============================================================
// BOTONES DEL MINIJUEGO
// ============================================================
void botonMinijuegoIzq() {
  if (!minijuegoActivo) return;
  if (carrilMascota > 0) {
    // Borrar mascota en posición actual
    tft.fillRect(0, Y_MASCOTA, 160, 16, COLOR_NEGRO);
    carrilMascota--;
    dibujarMascotaJuego();
  }
}

void botonMinijuegoCen() {
  if (!minijuegoActivo) return;
  // El central no hace nada en el minijuego por ahora
  // lo podemos usar más adelante para algo especial
}

void botonMinijuegoDer() {
  if (!minijuegoActivo) return;
  if (carrilMascota < 2) {
    tft.fillRect(0, Y_MASCOTA, 160, 16, COLOR_NEGRO);
    carrilMascota++;
    dibujarMascotaJuego();
  }
}