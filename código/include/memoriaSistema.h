#ifndef MEMORIA_SISTEMA_H
#define MEMORIA_SISTEMA_H

#include <Arduino.h>
#include "dto.h"

class MemoriaSistema {

    private:

        EstadoSistema estadoSistema;

        static const int quantidadeMaximaLeituras = 100;

        LeituraSensor historicoLeituras[quantidadeMaximaLeituras];

        int quantidadeAtualLeituras;

        bool _leituraManualSolicitada = false;

    public:

        MemoriaSistema();

        void salvarConfiguracao(const ConfiguracaoAlerta& configuracao);
        ConfiguracaoAlerta obterConfiguracao();

        void salvarModo(ModoSistema modo);
        ModoSistema obterModo();

        void salvarLeitura(const LeituraSensor& leitura);
        LeituraSensor obterUltimaLeitura();

        void salvarEstadoAlerta(const EstadoAlerta& alerta);
        EstadoAlerta obterEstadoAlerta();

        void adicionarHistorico(const LeituraSensor& leitura);
        LeituraSensor* obterHistorico();
        int obterQuantidadeHistorico();

        EstadoSistema obterEstadoSistema();
        void carregarPreferences();
        void salvarPreferences();

        void solicitarLeituraManual();
        bool consumirLeituraManual();
};
#endif