#include <Arduino.h>
#include "model/dto.h"
#include "leds/LedController.h" 
#include "sensor/DHTSensor.h"
#include "leds/testeLeds.h"
#include "storage/MemoriaSistema.h"
#include "alerta/AlertaService.h"
#include "web/WebServerManager.h"

#define PINO_LED_VERMELHO 32
#define PINO_LED_AMARELO  25
#define PINO_LED_VERDE    12
#define PINO_DHT          4

MemoriaSistema memoria;
AlertaService  alertaService(memoria);
LedController  leds(PINO_LED_VERMELHO, PINO_LED_AMARELO, PINO_LED_VERDE);
DHTSensor      sensor(PINO_DHT, memoria, alertaService);
WebServerManager webManager;

bool           testeExecutado = false;
ModoSistema modoAtual      = DESLIGADO;

void executarModo(ModoSistema modo) {
    switch (modo) {
        case TESTE:
            if (!testeExecutado) {
                executarTesteLeds();
                testeExecutado = true;
                modoAtual = DESLIGADO;
                memoria.salvarModo(DESLIGADO);
                Serial.println("Teste feito");
            }
            break;

        case DESLIGADO:
            leds.setModo(DESLIGADO);
            break;

        case LIGADO:
            leds.setModo(LIGADO);
            break;

        case AUTOMATICO:
            leds.setModo(AUTOMATICO);
            sensor.update();
            break;
    }
}

void exibirUltimaLeitura() {
    EstadoSistema estado = memoria.obterEstadoSistema();

    if (!estado.possuiLeitura) {
        Serial.println("Nenhuma leitura disponivel");
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

void exibirEstadoAlerta() {
    EstadoAlerta estado = memoria.obterEstadoAlerta();

    if (!estado.alertaAtivo) {
        Serial.println("Sem alertas ativos");
        return;
    }

    Serial.println("[ALERTA] situacao atual:");
    if (estado.temperaturaBaixa) Serial.println("  - temperatura baixa");
    if (estado.temperaturaAlta)  Serial.println("  - temperatura alta");
    if (estado.umidadeBaixa)     Serial.println("  - umidade baixa");
    if (estado.umidadeAlta)      Serial.println("  - umidade alta");
}

void aplicarConfiguracao(const ConfiguracaoAlerta& config) {
    memoria.salvarConfiguracao(config);
    Serial.println("Configuracao salva:");
    Serial.print("  Temperatura: ");
    Serial.print(config.temperaturaMinima);
    Serial.print(" a ");
    Serial.println(config.temperaturaMaxima);
    Serial.print("  Umidade: ");
    Serial.print(config.umidadeMinima);
    Serial.print(" a ");
    Serial.println(config.umidadeMaxima);
}

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

        case '7':
            exibirEstadoAlerta();
            break;

        case '8': {
            Serial.println("Digite: tempMin,tempMax,umidMin,umidMax");
            Serial.println("Exemplo: 18.0,35.0,40.0,80.0");

            while (Serial.available() == 0) {}

            String entrada = Serial.readStringUntil('\n');
            entrada.trim();

            int i0 = entrada.indexOf(',');
            int i1 = entrada.indexOf(',', i0 + 1);
            int i2 = entrada.indexOf(',', i1 + 1);

            if (i0 == -1 || i1 == -1 || i2 == -1) {
                Serial.println("Formato invalido");
                break;
            }

            ConfiguracaoAlerta config;
            config.temperaturaMinima = entrada.substring(0, i0).toFloat();
            config.temperaturaMaxima = entrada.substring(i0 + 1, i1).toFloat();
            config.umidadeMinima     = entrada.substring(i1 + 1, i2).toFloat();
            config.umidadeMaxima      = entrada.substring(i2 + 1).toFloat();

            aplicarConfiguracao(config);
            break;
        }
    }
}

void setup() {
    Serial.begin(115200);

    leds.begin();
    sensor.begin();
    initTesteLeds(PINO_LED_VERMELHO, PINO_LED_AMARELO, PINO_LED_VERDE);
    webManager.begin(memoria);

    Serial.println("=== Sistema iniciado ===");
    Serial.println("0 - Teste LEDs");
    Serial.println("1 - Desligado");
    Serial.println("2 - Ligado (manual)");
    Serial.println("3 - Automatico");
    Serial.println("4 - Ler sensor (modo manual)");
    Serial.println("5 - Ultima leitura");
    Serial.println("6 - Historico de leituras");
    Serial.println("7 - Estado dos alertas");
    Serial.println("8 - Configurar limites de alerta");
}

void loop() {
    modoAtual = memoria.obterEstadoSistema().modo;

    if (Serial.available()) {
        char comando = Serial.read();
        tratarComando(comando);
    }

    executarModo(modoAtual);
    delay(10);
}