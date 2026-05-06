#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>
#include <DHT.h>

// Estados do sensor
enum SensorMode
{
    SENSOR_OFF,
    SENSOR_ON,
    SENSOR_AUTO
};

class DHTSensor
{
private:
    DHT dht;
    uint8_t pin;
    SensorMode mode;

    unsigned long lastRead;
    unsigned long interval;

    float temperature;
    float humidity;

    bool lastReadSuccess;

public:
    DHTSensor(uint8_t pin);

    void begin();

    void setMode(SensorMode newMode);
    SensorMode getMode();

    void setInterval(unsigned long newInterval);

    void update(); // usado no loop

    bool readNow(); // leitura manual

    float getTemperature();
    float getHumidity();

    bool isLastReadOk();
};

#endif