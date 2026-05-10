#ifndef DTO_H
#define DTO_H

enum ModoSistema {
    TESTE = 0,
    DESLIGADO = 1,
    LIGADO = 2,
    AUTOMATICO = 3
};
struct ConfiguracaoAlerta {
    float temperaturaMinima;
    float temperaturaMaxima;

    float umidadeMinima;
    float umidadeMaxima;
};

struct EstadoAlerta {
    bool temperaturaBaixa;
    bool temperaturaAlta;

    bool umidadeBaixa;
    bool umidadeAlta;

    bool alertaAtivo;
};

struct LeituraSensor {
    float temperatura;
    float umidade;

    unsigned long timestamp;
};

struct EstadoSistema {

    ModoSistema modoAtual;

    LeituraSensor ultimaLeitura;

    EstadoAlerta alertaAtual;

    ConfiguracaoAlerta configuracaoAtual;

    bool possuiLeitura;

    bool possuiConfiguracao;
};

#endif