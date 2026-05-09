#include <Arduino.h>
#include "model/dto.h"
#include "leds/LedController.h"
#include "sensor/DHTSensor.h"
#include "leds/testeLeds.h"
#include "storage/MemoriaSistema.h"

#define PINO_LED_VERMELHO 32
#define PINO_LED_AMARELO  25
#define PINO_LED_VERDE    12
#define PINO_DHT          4

MemoriaSistema memoria;
LedController  leds(PINO_LED_VERMELHO, PINO_LED_AMARELO);
DHTSensor      sensor(PINO_DHT, memoria);

bool        testeExecutado = false;
ModoSistema modoAtual      = DESLIGADO;


void executarModo(ModoSistema modo) {
    switch (modo) {

        case TESTE:
            if (!testeExecutado) {
                executarTesteLeds();
                testeExecutado = true;
                modoAtual = DESLIGADO;
                Serial.println("Teste finalizado");
            }
            break;

        case DESLIGADO:
        // LED vermelho ligado, nenhum processamento do sensor
            leds.setModo(DESLIGADO);
            break;

        case LIGADO:
        // LED amarelo ligado, leitura do sensor apenas quando solicitado
            leds.setModo(LIGADO);
            break;

        case AUTOMATICO:
        //led amarelo e vermelho ligados 
            leds.setModo(AUTOMATICO);
            sensor.update();
            break;
    }
}

void exibirUltimaLeitura() {
    EstadoSistema estado = memoria.obterEstadoSistema();

    if (!estado.possuiLeitura) {
        Serial.println("Nenhuma leitura disponivel ainda");
        return;
    }

    LeituraSensor leitura = estado.ultimaLeitura;
    Serial.print("[MEM] Temperatura: ");
    Serial.print(leitura.temperatura);
    Serial.print(" C | Umidade: ");
    Serial.print(leitura.umidade);
    Serial.print(" % | Timestamp: ");
    Serial.println(leitura.timestamp);
}

void exibirHistorico() {
    int quantidade = memoria.obterQuantidadeHistorico();

    if (quantidade == 0) {
        Serial.println("Historico vazio");
        return;
    }

    LeituraSensor* historico = memoria.obterHistorico();

    Serial.print("Historico (");
    Serial.print(quantidade);
    Serial.println(" leituras):");

    for (int i = 0; i < quantidade; i++) {
        Serial.print("[");
        Serial.print(i + 1);
        Serial.print("] T: ");
        Serial.print(historico[i].temperatura);
        Serial.print(" C | U: ");
        Serial.print(historico[i].umidade);
        Serial.print(" % | ts: ");
        Serial.println(historico[i].timestamp);
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
            sensor.setMode(SENSOR_OFF);
            memoria.salvarModo(DESLIGADO);
            Serial.println("Modo: Desligado");
            break;

        case '2':
            modoAtual = LIGADO;
            sensor.setMode(SENSOR_OFF);
            memoria.salvarModo(LIGADO);
            Serial.println("Modo: Ligado");
            Serial.println("Digite 4 para ler o sensor");
            break;

        case '3':
            modoAtual = AUTOMATICO;
            sensor.setMode(SENSOR_AUTO);
            memoria.salvarModo(AUTOMATICO);
            Serial.println("Modo: Automatico");
            break;

        case '4':
            if (modoAtual == LIGADO) {
                Serial.println("Leitura manual do sensor:");
                sensor.readNow();
            } else {
                Serial.println("Comando invalido neste modo");
            }
            break;

        case '5':
            exibirUltimaLeitura();
            break;

        case '6':
            exibirHistorico();
            break;
    }
}

void setup() {
    Serial.begin(115200);

    leds.begin();
    sensor.begin();
    initTesteLeds(PINO_LED_VERMELHO, PINO_LED_AMARELO, PINO_LED_VERDE);

    Serial.println("=== Sistema iniciado ===");
    Serial.println("0 - Teste LEDs");
    Serial.println("1 - Desligado");
    Serial.println("2 - Ligado (manual)");
    Serial.println("3 - Automatico");
    Serial.println("4 - Ler sensor (modo manual)");
    Serial.println("5 - Ultima leitura");
    Serial.println("6 - Historico de leituras");
}

void loop() {
    if (Serial.available()) {
        char comando = Serial.read();
        tratarComando(comando);
    }

    executarModo(modoAtual);
    delay(10);
}