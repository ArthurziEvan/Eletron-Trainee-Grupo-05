#ifndef TESTE_LEDS_H
#define TESTE_LEDS_H

#include <Arduino.h>

// Inicializa os pinos para teste
void initTesteLeds(int vermelho, int amarelo, int verde);

// Executa o teste sequencial
void executarTesteLeds();

#endif