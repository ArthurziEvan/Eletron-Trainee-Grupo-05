#include "DHTSensor.h"

static const unsigned long INTERVALO_PADRAO = 5000;

DHTSensor::DHTSensor(uint8_t pin, MemoriaSistema& memoriaSistema, AlertaService& servicoAlerta)
    : dht(pin, DHTTYPE),
      pino(pin),
      memoria(memoriaSistema),
      alertaService(servicoAlerta) {

    modo                = SENSOR_OFF;
    intervaloLeitura    = INTERVALO_PADRAO;
    ultimoTempoLeitura  = 0;
    leituraValida       = false;

    ultimaLeitura.temperatura = 0.0;
    ultimaLeitura.umidade     = 0.0;
    ultimaLeitura.timestamp   = 0;
}

void DHTSensor::begin() {
    dht.begin();
    Serial.println("[DHT] Sensor inicializado");
}

void DHTSensor::setMode(SensorMode novoModo) {
    modo = novoModo;
}

SensorMode DHTSensor::getMode() {
    return modo;
}

void DHTSensor::setInterval(unsigned long novoIntervalo) {
    intervaloLeitura = novoIntervalo;
}

void DHTSensor::update() {
    if (modo != SENSOR_AUTO) return;

    unsigned long tempoAtual = millis();

    if (tempoAtual - ultimoTempoLeitura >= intervaloLeitura) {
        ultimoTempoLeitura = tempoAtual;
        readNow();
    }
}

bool DHTSensor::readNow() {
    float umidadeLida     = dht.readHumidity();
    float temperaturaLida = dht.readTemperature();

    if (isnan(umidadeLida) || isnan(temperaturaLida)) {
        Serial.println("[DHT] Falha na leitura");
        leituraValida = false;
        return false;
    }

    ultimaLeitura.temperatura = temperaturaLida;
    ultimaLeitura.umidade     = umidadeLida;
    ultimaLeitura.timestamp   = millis();
    leituraValida             = true;

    // salva na memória
    memoria.salvarLeitura(ultimaLeitura);
    memoria.adicionarHistorico(ultimaLeitura);
    alertaService.verificar(ultimaLeitura);

    Serial.print("[DHT] Temperatura: ");
    Serial.print(ultimaLeitura.temperatura);
    Serial.print(" C | Umidade: ");
    Serial.print(ultimaLeitura.umidade);
    Serial.println(" %");

    return true;
}

LeituraSensor DHTSensor::obterLeituraAtual() {
    return ultimaLeitura;
}

bool DHTSensor::isLastReadOk() {
    return leituraValida;
}