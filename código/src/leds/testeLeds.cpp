#include "testeLeds.h"

static int pinVermelho;
static int pinAmarelo;
static int pinVerde;

void initTesteLeds(int vermelho, int amarelo, int verde) {
    pinVermelho = vermelho;
    pinAmarelo  = amarelo;
    pinVerde    = verde;

    pinMode(pinVermelho, OUTPUT);
    pinMode(pinAmarelo, OUTPUT);
    pinMode(pinVerde, OUTPUT);
}

void executarTesteLeds() {

    // Vermelho
    digitalWrite(pinVermelho, HIGH);
    delay(800);
    digitalWrite(pinVermelho, LOW);

    // Amarelo
    digitalWrite(pinAmarelo, HIGH);
    delay(800);
    digitalWrite(pinAmarelo, LOW);

    // Verde
    digitalWrite(pinVerde, HIGH);
    delay(800);
    digitalWrite(pinVerde, LOW);

    // Todos juntos
    digitalWrite(pinVermelho, HIGH);
    digitalWrite(pinAmarelo, HIGH);
    digitalWrite(pinVerde, HIGH);
    delay(1000);

    digitalWrite(pinVermelho, LOW);
    digitalWrite(pinAmarelo, LOW);
    digitalWrite(pinVerde, LOW);
}