#include "sonidos.h"

void iniciarSonido() {
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);
}

// Pitido corto al mover el icono seleccionado
void sonarNavegacion() {
  if (juego.silenciado) return;
  tone(PIN_BUZZER, 800, 50);
}

// Pitido de confirmación al entrar en función
void sonarAccion() {
  if (juego.silenciado) return;
  tone(PIN_BUZZER, 1000, 80);
}

// Microondas: tono ascendente corto
void sonarComer() {
  if (juego.silenciado) return;
  tone(PIN_BUZZER, 600, 100);
  delay(120);
  tone(PIN_BUZZER, 900, 100);
  delay(120);
  tone(PIN_BUZZER, 1200, 150);
}

// Limpieza: tono de spray, tres pitidos rápidos descendentes
void sonarLimpiar() {
  if (juego.silenciado) return;
  tone(PIN_BUZZER, 1200, 60);
  delay(80);
  tone(PIN_BUZZER, 1000, 60);
  delay(80);
  tone(PIN_BUZZER, 800, 60);
}

// Curar: tono médico ascendente suave
void sonarCurar() {
  if (juego.silenciado) return;
  tone(PIN_BUZZER, 523, 100); // Do
  delay(120);
  tone(PIN_BUZZER, 659, 100); // Mi
  delay(120);
  tone(PIN_BUZZER, 784, 200); // Sol
}

// Evolución: melodía de 5 notas festiva
// Inspirada en el Tamagotchi original
void sonarEvolucion() {
  if (juego.silenciado) return;
  int notas[]    = { 523, 659, 784, 1047, 784  };
  int duracion[] = { 150, 150, 150, 300,  150  };
  for (int i = 0; i < 5; i++) {
    tone(PIN_BUZZER, notas[i], duracion[i]);
    delay(duracion[i] + 30);
  }
}

// Muerte: melodía descendente triste
void sonarMuerte() {
  if (juego.silenciado) return;
  int notas[]    = { 494, 440, 392, 349, 294  };
  int duracion[] = { 200, 200, 200, 200, 400  };
  for (int i = 0; i < 5; i++) {
    tone(PIN_BUZZER, notas[i], duracion[i]);
    delay(duracion[i] + 30);
  }
}

// Alarma: pitido intermitente de aviso
void sonarAlarma() {
  if (juego.silenciado) return;
  tone(PIN_BUZZER, 1000, 100);
  delay(200);
  tone(PIN_BUZZER, 1000, 100);
}

// Minijuego ganado: fanfarria corta
void sonarMinijuegoGana() {
  if (juego.silenciado) return;
  int notas[]    = { 784, 784, 784, 1047 };
  int duracion[] = { 100, 100, 100, 400  };
  for (int i = 0; i < 4; i++) {
    tone(PIN_BUZZER, notas[i], duracion[i]);
    delay(duracion[i] + 30);
  }
}

// Minijuego perdido: tono de fallo descendente
void sonarMinijuegoPierde() {
  if (juego.silenciado) return;
  tone(PIN_BUZZER, 400, 200);
  delay(230);
  tone(PIN_BUZZER, 300, 400);
}