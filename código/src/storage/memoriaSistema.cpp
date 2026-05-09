#include "MemoriaSistema.h"
#include <Preferences.h>

MemoriaSistema::MemoriaSistema()
{
    quantidadeAtualLeituras = 0;
    estadoSistema.possuiConfiguracao = false;
    estadoSistema.possuiLeitura = false;
    estadoSistema.modoAtual = DESLIGADO;
}

void MemoriaSistema::salvarConfiguracao(const ConfiguracaoAlerta &configuracao)
{
    estadoSistema.configuracaoAtual = configuracao;
    estadoSistema.possuiConfiguracao = true;

    Preferences prefs;
    prefs.begin("config", false);
    prefs.putFloat("tempMin", config.temperaturaMinima);
    prefs.putFloat("tempMax", config.temperaturaMaxima);
    prefs.putFloat("humMin",  config.umidadeMinima);
    prefs.putFloat("humMax",  config.umidadeMaxima);
    prefs.end();
}

ConfiguracaoAlerta MemoriaSistema::obterConfiguracao()
{
    return estadoSistema.configuracaoAtual;
}

void MemoriaSistema::salvarModo(ModoSistema modo)
{
    estadoSistema.modoAtual = modo;
}

ModoSistema MemoriaSistema::obterModo()
{
    return estadoSistema.modoAtual;
}

void MemoriaSistema::salvarLeitura(const LeituraSensor &leitura)
{
    estadoSistema.ultimaLeitura = leitura;
    estadoSistema.possuiLeitura = true;
}

LeituraSensor MemoriaSistema::obterUltimaLeitura()
{
    return estadoSistema.ultimaLeitura;
}

void MemoriaSistema::salvarEstadoAlerta(const EstadoAlerta &alerta)
{
    estadoSistema.alertaAtual = alerta;
}

EstadoAlerta MemoriaSistema::obterEstadoAlerta()
{
    return estadoSistema.alertaAtual;
}

void MemoriaSistema::adicionarHistorico(const LeituraSensor &leitura)
{
    // circular buffer: descarta a leitura mais antiga quando cheio
    if (quantidadeAtualLeituras >= quantidadeMaximaLeituras)
    {
        for (int i = 1; i < quantidadeMaximaLeituras; i++)
        {
            historicoLeituras[i - 1] = historicoLeituras[i];
        }
        quantidadeAtualLeituras = quantidadeMaximaLeituras - 1;
    }

    historicoLeituras[quantidadeAtualLeituras] = leitura;
    quantidadeAtualLeituras++;
}

LeituraSensor *MemoriaSistema::obterHistorico()
{
    return historicoLeituras;
}

int MemoriaSistema::obterQuantidadeHistorico()
{
    return quantidadeAtualLeituras;
}

EstadoSistema MemoriaSistema::obterEstadoSistema()
{
    return estadoSistema;
}