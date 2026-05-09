#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <Arduino.h>
#include <DHT.h>
#include "../model/dto.h"
#include "../storage/MemoriaSistema.h"
#include "../alerta/AlertaService.h"

#define DHTTYPE DHT11

enum SensorMode {
    SENSOR_OFF,
    SENSOR_ON,
    SENSOR_AUTO
};

class DHTSensor {

    private:

        DHT dht;

        uint8_t pino;

        SensorMode modo;

        unsigned long ultimoTempoLeitura;

        unsigned long intervaloLeitura;

        LeituraSensor ultimaLeitura;

        bool leituraValida;

        MemoriaSistema& memoria;

        AlertaService& alertaService;


    public:

        DHTSensor(uint8_t pin, MemoriaSistema& memoriaSistema, AlertaService& servicoAlerta);

        void begin();

        void setMode(SensorMode novoModo);

        SensorMode getMode();

        void setInterval(unsigned long novoIntervalo);

        void update();

        bool readNow();

        LeituraSensor obterLeituraAtual();

        bool isLastReadOk();
};

#endif