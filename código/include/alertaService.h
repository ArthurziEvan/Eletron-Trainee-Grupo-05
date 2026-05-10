#ifndef ALERTA_SERVICE_H
#define ALERTA_SERVICE_H

#include <Arduino.h>
#include "dto.h"
#include "MemoriaSistema.h"

class AlertaService {

    private:

        MemoriaSistema& memoria;

        int verificarTemperatura(float temperatura, const ConfiguracaoAlerta& config);

        int verificarUmidade(float umidade, const ConfiguracaoAlerta& config);

    public:

        AlertaService(MemoriaSistema& memoriaSistema);

        void verificar(const LeituraSensor& leitura);
};

#endif