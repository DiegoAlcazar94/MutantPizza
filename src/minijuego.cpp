#include "minijuego.h"
#include "sprites/sprites.h"

// Color verde LCD retro estilo Tamagotchi / Game Boy (RGB565)
#define COLOR_FONDO_LCD 0x8CA0 

static void dibujarSpriteMin(const uint16_t* sprite, int x, int y, int w, int h) {
  for (int j = 0; j < h; j++) {
    for (int i = 0; i < w; i++) {
      uint16_t pixel = pgm_read_word(&sprite[j * w + i]);
      if (pixel != 0xFFFF) {
        tft.drawPixel(x + i, y + j, pixel);
      }
    }
  }
}

// ============================================================
// CONSTANTES DEL MINIJUEGO
// ============================================================
#define NUM_CARRILES     3
#define MAX_OBJETOS      4    // máximo de objetos en pantalla a la vez
#define VELOCIDAD_INICIO 4    // píxeles que baja cada frame
#define VIDAS_INICIO     3
#define COMIDA_PARA_GANAR 20

// Posiciones X del centro de cada carril
const int X_CARRILES[3] = { 26, 80, 133 };

// Y donde está la mascota (fila de abajo)
#define Y_MASCOTA  105
#define Y_INICIO   -16  // donde aparecen los objetos

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
static uint8_t framesSinSpawn;
static uint8_t spawnsHastaAhora;
bool    minijuegoActivo;
static bool    minijuegoGanado;

#define FRAMES_ENTRE_SPAWNS 25

// Redibuja las líneas divisorias en la zona del objeto para no romper la pantalla
static void redibujarLineasEnArea(int y, int h) {
  if (y < 12) return;
  tft.drawLine(53,  y, 53,  min(y + h, (int)Y_MASCOTA), COLOR_NEGRO);
  tft.drawLine(106, y, 106, min(y + h, (int)Y_MASCOTA), COLOR_NEGRO);
}

// ============================================================
// DIBUJAR OBJETO en pantalla
// ============================================================
static void dibujarObjeto(Objeto& obj) {
  if (!obj.activo) return;

  int x = X_CARRILES[obj.carril] - 8;

  switch (obj.tipo) {
    case OBJ_CHAMPINON: dibujarSpriteMin(Icon_Mushroom, x, obj.y, 16, 16); break;
    case OBJ_PEPERONI:  dibujarSpriteMin(Icon_Onion,    x, obj.y, 16, 16); break;
    case OBJ_BACON:     dibujarSpriteMin(Icon_Bacon,    x, obj.y, 16, 16); break;
    case OBJ_RATA:      dibujarSpriteMin(Icon_Rat,      x, obj.y, 16, 16); break;
  }
}

// ============================================================
// BORRAR OBJETO de pantalla (pinta el color de fondo LCD)
// ============================================================
static void borrarObjeto(Objeto& obj) {
  if (!obj.activo) return;
  int x = X_CARRILES[obj.carril] - 8;
  
  // Limpiamos con el fondo LCD
  tft.fillRect(x, obj.y, 16, 16, COLOR_FONDO_LCD);
  
  // Reparamos la línea divisoria si el borrado la pisó
  redibujarLineasEnArea(obj.y, 16);
}

// ============================================================
// SPAWNEAR un objeto nuevo
// ============================================================
static void spawnearObjeto() {
  for (int i = 0; i < MAX_OBJETOS; i++) {
    if (!objetos[i].activo) {
      objetos[i].activo = true;
      objetos[i].y      = Y_INICIO;
      objetos[i].carril = random(0, 3);

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
  tft.fillRect(0, Y_MASCOTA, 160, 16, COLOR_FONDO_LCD);
  
  // Redibujar divisiones de carril abajo
  tft.drawLine(53,  Y_MASCOTA, 53,  128, COLOR_NEGRO);
  tft.drawLine(106, Y_MASCOTA, 106, 128, COLOR_NEGRO);

  int x = X_CARRILES[carrilMascota] - 8;
  dibujarSpriteMin(Icon_Pizza, x, Y_MASCOTA, 16, 16);
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

  tft.setTextColor(COLOR_AMARILLO);
  tft.setCursor(90, 2);
  tft.print(comidaRecogida);
  tft.print("/");
  tft.print(COMIDA_PARA_GANAR);
}

// ============================================================
// PANTALLA DE FIN DE MINIJUEGO
// ============================================================
static void pantallaFinMinijuego() {
  tft.fillScreen(COLOR_FONDO_LCD);

  if (minijuegoGanado) {
    tft.setTextColor(COLOR_NEGRO);
    tft.setTextSize(2);
    tft.setCursor(20, 30);
    tft.print("GANASTE!");
    tft.setTextSize(1);
    tft.setCursor(10, 65);
    tft.print("Felicidad +1");

    if (juego.felicidad < 5) juego.felicidad++;

  } else {
    tft.setTextColor(COLOR_ROJO);
    tft.setTextSize(2);
    tft.setCursor(20, 30);
    tft.print("PERDISTE");
    tft.setTextSize(1);
    tft.setTextColor(COLOR_NEGRO);
    tft.setCursor(10, 65);

    int bajada = max(1, (int)(juego.felicidad * 0.1));
    juego.felicidad = (uint8_t)max(0, juego.felicidad - bajada);
    tft.print("Felicidad -10%");
  }

  tft.setTextColor(COLOR_NEGRO);
  tft.setCursor(10, 100);
  tft.print("DER: volver al menu");

  minijuegoActivo = false;
}

// ============================================================
// INICIAR MINIJUEGO
// ============================================================
void iniciarMinijuego() {
  for (int i = 0; i < MAX_OBJETOS; i++) {
    objetos[i].activo = false;
    objetos[i].y      = Y_INICIO;
    objetos[i].carril = 0;
    objetos[i].tipo   = OBJ_VACIO;
  }

  carrilMascota    = 1;
  vidas            = VIDAS_INICIO;
  comidaRecogida   = 0;
  velocidad        = VELOCIDAD_INICIO;
  framesSinSpawn   = 0;
  spawnsHastaAhora = 0;
  minijuegoActivo  = true;
  minijuegoGanado  = false;

  tft.fillScreen(COLOR_FONDO_LCD);

  // Líneas divisorias de carriles (en negro)
  tft.drawLine(53,  12, 53,  128, COLOR_NEGRO);
  tft.drawLine(106, 12, 106, 128, COLOR_NEGRO);

  dibujarHUD();
  dibujarMascotaJuego();
}

// ============================================================
// ACTUALIZAR MINIJUEGO
// ============================================================
void actualizarMinijuego() {
  if (!minijuegoActivo) return;

  for (int i = 0; i < MAX_OBJETOS; i++) {
    if (!objetos[i].activo) continue;

    borrarObjeto(objetos[i]);

    objetos[i].y += velocidad;

    if (objetos[i].y >= Y_MASCOTA - 4) {

      if (objetos[i].carril == carrilMascota) {

        if (objetos[i].tipo == OBJ_RATA) {
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
          comidaRecogida++;

          if (comidaRecogida >= COMIDA_PARA_GANAR) {
            minijuegoGanado = true;
            pantallaFinMinijuego();
            return;
          }
        }
      }

      objetos[i].activo = false;

    } else {
      dibujarObjeto(objetos[i]);
    }
  }

  framesSinSpawn++;
  if (framesSinSpawn >= FRAMES_ENTRE_SPAWNS) {
    framesSinSpawn = 0;
    spawnearObjeto();

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
    tft.fillRect(0, Y_MASCOTA, 160, 16, COLOR_FONDO_LCD);
    carrilMascota--;
    dibujarMascotaJuego();
  }
}

void botonMinijuegoCen() {
  if (!minijuegoActivo) return;
}

void botonMinijuegoDer() {
  if (!minijuegoActivo) return;
  if (carrilMascota < 2) {
    tft.fillRect(0, Y_MASCOTA, 160, 16, COLOR_FONDO_LCD);
    carrilMascota++;
    dibujarMascotaJuego();
  }
}