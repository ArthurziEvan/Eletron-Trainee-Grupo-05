#include <Arduino.h>
#include "modo.h"
#include "leds/LedController.h"
#include "sensor/dhtSensor.h"
#include "leds/testeLeds.h"

// Definição centralizada dos pinos
#define PINO_LED_VERMELHO 32
#define PINO_LED_AMARELO  25
#define PINO_LED_VERDE    12
#define PINO_DHT          4

// Instâncias dos componentes
LedController leds(PINO_LED_VERMELHO, PINO_LED_AMARELO);
DHTSensor sensor(PINO_DHT);

bool testeExecutado = false;
// Estado atual do sistema
ModoSistema modoAtual = DESLIGADO;


void executarModo(ModoSistema modo) {
    switch (modo) {
        case TESTE:

        if (!testeExecutado) {
            executarTesteLeds();
            testeExecutado = true;

            // volta automaticamente para desligado
            modoAtual = DESLIGADO;
            Serial.println("Teste finalizado");
        }

        break;

        case DESLIGADO:
            // LED vermelho ligado, nenhum processamento do sensor
            leds.setModo(DESLIGADO);
            break;

        case LIGADO:
            // LED amarelo ligado
            // Sensor não roda automaticamente (modo manual)
            leds.setModo(LIGADO);
            break;

        case AUTOMATICO:
            // Ambos LEDs ligados
            // Sensor roda automaticamente com base no intervalo interno
            leds.setModo(AUTOMATICO);
            sensor.update();
            break;
    }
}

/*
 Função responsável por tratar comandos recebidos via terminal.
 Aqui fazemos o mapeamento entre entrada do usuário e ações do sistema.
*/
void tratarComando(char comando) {

    switch (comando) {
        case '0':
            modoAtual = TESTE;
            testeExecutado = false;
            Serial.println("Modo: Teste de LEDs");
        break;

        case '1':
            modoAtual = DESLIGADO;
            sensor.setMode(SENSOR_OFF); // sensor desativado
            Serial.println("Modo: Desligado");
            break;

        case '2':
            modoAtual = LIGADO;
            sensor.setMode(SENSOR_OFF); // leitura apenas sob demanda
            Serial.println("Modo: Ligado");
            Serial.println("Digite 4 para ler o sensor");
            break;

        case '3':
            modoAtual = AUTOMATICO;
            sensor.setMode(SENSOR_AUTO); // sensor passa a rodar sozinho
            Serial.println("Modo: Automatico");
            break;

        case '4':
            // Leitura manual só faz sentido no modo LIGADO
            if (modoAtual == LIGADO) {
                Serial.println("Leitura manual do sensor:");
                sensor.readNow();
            } else {
                Serial.println("Comando invalido neste modo");
            }
            break;
    }
}

void setup() {
    Serial.begin(115200);

    // Inicializa hardware
    leds.begin();
    sensor.begin();

    Serial.println("Sistema iniciado");
    Serial.println("Escolha o modo:");
    Serial.println("0 - Teste LEDs");
    Serial.println("1 - Desligado");
    Serial.println("2 - Ligado");
    Serial.println("3 - Automatico");
    Serial.println("4 - Ler sensor (modo manual)");
}

void loop() {

    // Verifica se há comando vindo do terminal
    if (Serial.available()) {
        char comando = Serial.read();
        tratarComando(comando);
    }

    // Executa continuamente o comportamento do sistema
    executarModo(modoAtual);
     delay(10);
}