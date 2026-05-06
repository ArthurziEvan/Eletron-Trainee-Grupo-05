#include "dhtSensor.h"

#define DHTTYPE DHT11

// Intervalo padrão de leitura (ms)
static const unsigned long INTERVALO_PADRAO = 5000;

DHTSensor::DHTSensor(uint8_t pin) : dht(pin, DHTTYPE), pino(pin)
{
    modo = SENSOR_OFF;

    intervaloLeitura = INTERVALO_PADRAO;
    ultimoTempoLeitura = 0;

    temperatura = 0.0;
    umidade = 0.0;

    leituraValida = false;
}

void DHTSensor::begin()
{
    dht.begin();
    Serial.println("[DHT] Sensor inicializado com sucesso");
}

void DHTSensor::setMode(SensorMode newMode)
{
    modo = newMode;
}

SensorMode DHTSensor::getMode()
{
    return modo;
}

void DHTSensor::setInterval(unsigned long newInterval)
{
    intervaloLeitura = newInterval;
}

void DHTSensor::update()
{
    // Só executa no modo automático
    if (modo != SENSOR_AUTO)
        return;

    unsigned long tempoAtual = millis();

    // Verifica se já passou o intervalo configurado
    if (tempoAtual - ultimoTempoLeitura >= intervaloLeitura)
    {
        ultimoTempoLeitura = tempoAtual;
        readNow();
    }
}

bool DHTSensor::readNow()
{
    float umidadeLida = dht.readHumidity();
    float temperaturaLida = dht.readTemperature();

    // Validação da leitura
    if (isnan(umidadeLida) || isnan(temperaturaLida))
    {
        Serial.println("[DHT] Falha na leitura do sensor");
        leituraValida = false;
        return false;
    }

    // Atualiza estado interno
    umidade = umidadeLida;
    temperatura = temperaturaLida;
    leituraValida = true;

    // Log formatado
    Serial.print("[DHT] Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C | Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");

    return true;
}

float DHTSensor::getTemperature()
{
    return temperatura;
}

float DHTSensor::getHumidity()
{
    return umidade;
}

bool DHTSensor::isLastReadOk()
{
    return leituraValida;
}