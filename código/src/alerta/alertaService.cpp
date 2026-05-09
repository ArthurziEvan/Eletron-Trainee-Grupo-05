#include "AlertaService.h"

AlertaService::AlertaService(MemoriaSistema& memoriaSistema)
    : memoria(memoriaSistema) {
}

int AlertaService::verificarTemperatura(float temperatura, const ConfiguracaoAlerta& config) {
    if (temperatura <= config.temperaturaMinima) {
        return 1; //para o alerta de temp baixas
    }
    else if (temperatura >= config.temperaturaMaxima) {
        return 2; //para o alerta de temp altas
    }
    return 0; //para se ta de boa a temp
}

int AlertaService::verificarUmidade(float umidade, const ConfiguracaoAlerta& config) {
    if (umidade <= config.umidadeMinima) {
        return 3; //para o alerta de umidade baixa
    }
    else if (umidade >= config.umidadeMaxima) {
        return 4; //para o alerta de umidade alta
    }
    return 0; //umidade normal
    //obs: eu não lembrava se de fato tinha que ter o alerta de umidade e por via das duvidas eu fiz
}

void AlertaService::verificar(const LeituraSensor& leitura) {
    if (!memoria.obterEstadoSistema().possuiConfiguracao) {
        return;
    }

    ConfiguracaoAlerta config = memoria.obterConfiguracao();

    int resultadoTemp     = verificarTemperatura(leitura.temperatura, config);
    int resultadoUmidade  = verificarUmidade(leitura.umidade, config);

    EstadoAlerta alerta;
    alerta.temperaturaBaixa = (resultadoTemp == 1);
    alerta.temperaturaAlta  = (resultadoTemp == 2);
    alerta.umidadeBaixa     = (resultadoUmidade == 3);
    alerta.umidadeAlta      = (resultadoUmidade == 4);
    alerta.alertaAtivo      = (resultadoTemp != 0 || resultadoUmidade != 0);

    memoria.salvarEstadoAlerta(alerta);

    if (alerta.alertaAtivo) {
        Serial.println("[ALERTA] situacao fora do limite:");
        if (alerta.temperaturaBaixa) Serial.println("  - temperatura baixa");
        if (alerta.temperaturaAlta)  Serial.println("  - temperatura alta");
        if (alerta.umidadeBaixa)     Serial.println("  - umidade baixa");
        if (alerta.umidadeAlta)      Serial.println("  - umidade alta");
    }
}