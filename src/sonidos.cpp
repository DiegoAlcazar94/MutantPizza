#include "sonidos.h"

void iniciarSonido() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
}

// Emite un pitido de la duración indicada
static void emitirTono(unsigned long duracion) {
  if (juego.silenciado) return;
  digitalWrite(PIN_BUZZER, HIGH);
  delay(duracion);
  digitalWrite(PIN_BUZZER, LOW);
}

// Pausa entre notas
static void pausa(unsigned long duracion) {
  delay(duracion);
}

void sonarNavegacion() {
  emitirTono(50);
}

void sonarAccion() {
  emitirTono(80);
}

void sonarComer() {
  if (juego.silenciado) return;
  emitirTono(100); pausa(50);
  emitirTono(100); pausa(50);
  emitirTono(200);
}

void sonarLimpiar() {
  if (juego.silenciado) return;
  emitirTono(60); pausa(40);
  emitirTono(60); pausa(40);
  emitirTono(60);
}

void sonarCurar() {
  if (juego.silenciado) return;
  emitirTono(100); pausa(50);
  emitirTono(100); pausa(50);
  emitirTono(300);
}

void sonarEvolucion() {
  if (juego.silenciado) return;
  for (int i = 0; i < 5; i++) {
    emitirTono(150);
    pausa(80);
  }
  emitirTono(400);
}

void sonarMuerte() {
  if (juego.silenciado) return;
  emitirTono(300); pausa(100);
  emitirTono(300); pausa(100);
  emitirTono(600);
}

void sonarAlarma() {
  if (juego.silenciado) return;
  emitirTono(100); pausa(100);
  emitirTono(100); pausa(100);
  emitirTono(100);
}

void sonarMinijuegoGana() {
  if (juego.silenciado) return;
  emitirTono(100); pausa(50);
  emitirTono(100); pausa(50);
  emitirTono(100); pausa(50);
  emitirTono(400);
}

void sonarMinijuegoPierde() {
  if (juego.silenciado) return;
  emitirTono(400); pausa(100);
  emitirTono(600);
}