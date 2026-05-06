#include "LedController.h"

LedController::LedController(int vermelho, int amarelo) {
    pinVermelho = vermelho;
    pinAmarelo = amarelo;
}

void LedController::begin() {
    pinMode(pinVermelho, OUTPUT);
    pinMode(pinAmarelo, OUTPUT);
}

void LedController::setModo(ModoSistema modo) {
    switch (modo) {
        case DESLIGADO:
            digitalWrite(pinVermelho, HIGH);
            digitalWrite(pinAmarelo, LOW);
            break;

        case LIGADO:
            digitalWrite(pinVermelho, LOW);
            digitalWrite(pinAmarelo, HIGH);
            break;

        case AUTOMATICO:
            digitalWrite(pinVermelho, HIGH);
            digitalWrite(pinAmarelo, HIGH);
            break;
    }
}