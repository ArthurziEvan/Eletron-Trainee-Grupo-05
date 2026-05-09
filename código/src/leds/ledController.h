#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <Arduino.h>
#include "dto.h"

class LedController {
private:
    int pinVermelho;
    int pinAmarelo;

public:
    LedController(int vermelho, int amarelo);

    void begin();
    void setModo(ModoSistema modo);
};
#endif