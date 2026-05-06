#include "dhtSensor.h"

#define DHTTYPE DHT11

DHTSensor::DHTSensor(uint8_t pin)
    : dht(pin, DHTTYPE), pin(pin)
{
    mode = SENSOR_OFF;
    interval = 5000; // padrão 5s
    lastRead = 0;
    temperature = 0;
    humidity = 0;
    lastReadSuccess = false;
}

void DHTSensor::begin()
{
    dht.begin();
    Serial.println("[DHT] Sensor inicializado!");
}

void DHTSensor::setMode(SensorMode newMode)
{
    mode = newMode;
}

SensorMode DHTSensor::getMode()
{
    return mode;
}

void DHTSensor::setInterval(unsigned long newInterval)
{
    interval = newInterval;
}

void DHTSensor::update()
{
    if (mode != SENSOR_AUTO)
        return;

    unsigned long now = millis();

    if (now - lastRead >= interval)
    {
        lastRead = now;
        readNow();
    }
}

bool DHTSensor::readNow()
{
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t))
    {
        Serial.println("[DHT] Erro ao ler sensor!");
        lastReadSuccess = false;
        return false;
    }

    humidity = h;
    temperature = t;
    lastReadSuccess = true;

    Serial.print("[DHT] Temp: ");
    Serial.print(temperature);
    Serial.print(" °C | Umidade: ");
    Serial.print(humidity);
    Serial.println(" %");

    return true;
}

float DHTSensor::getTemperature()
{
    return temperature;
}

float DHTSensor::getHumidity()
{
    return humidity;
}

bool DHTSensor::isLastReadOk()
{
    return lastReadSuccess;
}