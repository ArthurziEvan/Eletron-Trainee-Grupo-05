#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>
#include <DHT.h>

// Modos de operação do sensor
enum SensorMode
{
    SENSOR_OFF,   // sensor desligado
    SENSOR_ON,    // leitura sob demanda
    SENSOR_AUTO   // leitura automática por intervalo
};

class DHTSensor
{
private:
    DHT dht;
    uint8_t pino;

    SensorMode modo;

    unsigned long ultimoTempoLeitura;
    unsigned long intervaloLeitura;

    float temperatura;
    float umidade;

    bool leituraValida;

public:
    // Construtor
    DHTSensor(uint8_t pin);

    // Inicialização do sensor
    void begin();

    // Controle de modo
    void setMode(SensorMode newMode);
    SensorMode getMode();

    // Configuração de intervalo (modo automático)
    void setInterval(unsigned long newInterval);

    // Atualização automática (chamar no loop)
    void update();

    // Leitura manual imediata
    bool readNow();

    // Getters de dados
    float getTemperature();
    float getHumidity();

    // Status da última leitura
    bool isLastReadOk();
};

#endif