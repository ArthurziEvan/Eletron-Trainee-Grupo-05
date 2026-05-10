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
    prefs.putFloat("tempMin", configuracao.temperaturaMinima);
    prefs.putFloat("tempMax", configuracao.temperaturaMaxima);
    prefs.putFloat("humMin",  configuracao.umidadeMinima);
    prefs.putFloat("humMax",  configuracao.umidadeMaxima);
    prefs.end();
}

ConfiguracaoAlerta MemoriaSistema::obterConfiguracao()
{
    return estadoSistema.configuracaoAtual;
}

void MemoriaSistema::salvarModo(ModoSistema modo)
{
    estadoSistema.modoAtual = modo;

    Preferences prefs;
    prefs.begin("config", false);
    prefs.putInt("modo", (int)modo);
    prefs.end();
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
 
void MemoriaSistema::solicitarLeituraManual()
{
    _leituraManualSolicitada = true;
}
 
bool MemoriaSistema::consumirLeituraManual()
{
    if (_leituraManualSolicitada) {
        _leituraManualSolicitada = false;
        return true;
    }
    return false;
}

void MemoriaSistema::carregarPreferences() {
    Preferences prefs;

    // Tenta abrir em leitura. Se o namespace ainda não existe
    // (primeiro boot), begin() retorna false → sai sem erro.
    if (!prefs.begin("config", true)) {
        Serial.println("[MEM] Sem preferencias salvas (primeiro boot)");
        return;
    }

    if (prefs.isKey("tempMin")) {
        estadoSistema.configuracaoAtual.temperaturaMinima = prefs.getFloat("tempMin");
        estadoSistema.configuracaoAtual.temperaturaMaxima = prefs.getFloat("tempMax");
        estadoSistema.configuracaoAtual.umidadeMinima     = prefs.getFloat("humMin");
        estadoSistema.configuracaoAtual.umidadeMaxima     = prefs.getFloat("humMax");
        estadoSistema.possuiConfiguracao = true;
        Serial.println("[MEM] Configuracao de alerta carregada");
    }

    if (prefs.isKey("modo")) {
        estadoSistema.modoAtual = (ModoSistema)prefs.getInt("modo");
        Serial.println("[MEM] Modo carregado");
    }

    prefs.end();
}