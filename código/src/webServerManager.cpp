#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include "WebServerManager.h"
#include "dto.h"
#include <ArduinoJson.h>

const char* ssid  = "Mustafa2-2G";
const char* senha = "W+jls2540B";

static WebServer       server(80);
static MemoriaSistema* _memoria = nullptr;

void WebServerManager::begin(MemoriaSistema& mem) {
    _memoria = &mem;

    WiFi.begin(ssid, senha);

    unsigned long inicio = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - inicio > 15000) {
            Serial.println("\nFalha ao conectar WiFi");
            return;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado!");
    Serial.println(WiFi.localIP());

    if (!LittleFS.begin(true)) {
        Serial.println("Erro ao montar LittleFS");
        return;
    }
    Serial.println("LittleFS montado");

    server.on("/", HTTP_GET, []() {
        File file = LittleFS.open("/index.html", "r");
        if (!file) {
            server.send(404, "text/plain", "index.html nao encontrado");
            return;
        }
        server.streamFile(file, "text/html");
        file.close();
    });

    server.on("/api/dados", HTTP_GET, []() {
        EstadoSistema e = _memoria->obterEstadoSistema();

        String json = "{";
        json += "\"possuiLeitura\":"  + String(e.possuiLeitura ? "true" : "false") + ",";
        json += "\"temperatura\":"    + String(e.ultimaLeitura.temperatura) + ",";
        json += "\"umidade\":"        + String(e.ultimaLeitura.umidade) + ",";
        json += "\"timestamp\":"      + String(e.ultimaLeitura.timestamp) + ",";
        json += "\"modo\":"           + String(e.modoAtual);
        json += "}";

        server.send(200, "application/json", json);
    });

    server.on("/api/modo", HTTP_GET, []() {
        ModoSistema modo = _memoria->obterModo();
        const char* nomes[] = { "desligado", "desligado", "ligado", "automatico" };
        const char* nome = (modo >= TESTE && modo <= AUTOMATICO) ? nomes[modo] : "desligado";
        String json = "{\"modo\":\"" + String(nome) + "\"}";
        server.send(200, "application/json", json);
    });

    server.on("/api/modo", HTTP_POST, []() {
        if (!server.hasArg("plain")) {
            server.send(400, "application/json", "{\"erro\":\"sem body\"}");
            return;
        }

        String body = server.arg("plain");

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, body);
        if (err) {
            server.send(400, "application/json", "{\"erro\":\"json invalido\"}");
            return;
        }

        String valor = doc["modo"].as<String>();

        ModoSistema novoModo;
        if      (valor == "automatico") novoModo = AUTOMATICO;
        else if (valor == "ligado")     novoModo = LIGADO;
        else if (valor == "desligado")  novoModo = DESLIGADO;
        else {
            server.send(400, "application/json", "{\"erro\":\"modo invalido\"}");
            return;
        }

        _memoria->salvarModo(novoModo);
        server.send(200, "application/json", "{\"ok\":true}");
    });

    // Leitura manual — só aceita se o modo atual for LIGADO
    server.on("/api/ler", HTTP_POST, []() {
        if (_memoria->obterModo() != LIGADO) {
            server.send(403, "application/json", "{\"erro\":\"modo invalido\"}");
            return;
        }
        _memoria->solicitarLeituraManual();
        server.send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/api/config", HTTP_GET, []() {
        ConfiguracaoAlerta c = _memoria->obterConfiguracao();

        String json = "{";
        json += "\"tempMin\":"  + String(c.temperaturaMinima) + ",";
        json += "\"tempMax\":"  + String(c.temperaturaMaxima) + ",";
        json += "\"humidMin\":" + String(c.umidadeMinima) + ",";
        json += "\"humidMax\":" + String(c.umidadeMaxima);
        json += "}";

        server.send(200, "application/json", json);
    });

    server.on("/api/config", HTTP_POST, []() {
        if (!server.hasArg("plain")) {
            server.send(400, "application/json", "{\"erro\":\"sem body\"}");
            return;
        }

        String body = server.arg("plain");

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, body);
        if (err) {
            server.send(400, "application/json", "{\"erro\":\"json invalido\"}");
            return;
        }

        if (!doc["tempMin"].is<float>() || !doc["tempMax"].is<float>() ||
            !doc["humidMin"].is<float>() || !doc["humidMax"].is<float>()) {
            server.send(400, "application/json", "{\"erro\":\"campos ausentes\"}");
            return;
        }

        ConfiguracaoAlerta config;
        config.temperaturaMinima = doc["tempMin"].as<float>();
        config.temperaturaMaxima = doc["tempMax"].as<float>();
        config.umidadeMinima     = doc["humidMin"].as<float>();
        config.umidadeMaxima     = doc["humidMax"].as<float>();

        if (config.temperaturaMinima >= config.temperaturaMaxima ||
            config.umidadeMinima     >= config.umidadeMaxima) {
            server.send(400, "application/json", "{\"erro\":\"limites invalidos\"}");
            return;
        }

        _memoria->salvarConfiguracao(config);
        server.send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/api/historico", HTTP_GET, []() {
        int            qtd = _memoria->obterQuantidadeHistorico();
        LeituraSensor* h   = _memoria->obterHistorico();

        String json = "[";
        for (int i = 0; i < qtd; i++) {
            if (i > 0) json += ",";
            json += "{";
            json += "\"temperatura\":" + String(h[i].temperatura) + ",";
            json += "\"umidade\":"     + String(h[i].umidade) + ",";
            json += "\"timestamp\":"   + String(h[i].timestamp);
            json += "}";
        }
        json += "]";

        server.send(200, "application/json", json);
    });

    server.on("/api/alerta", HTTP_GET, []() {
        EstadoAlerta a = _memoria->obterEstadoAlerta();

        String json = "{";
        json += "\"alertaAtivo\":"      + String(a.alertaAtivo      ? "true" : "false") + ",";
        json += "\"temperaturaBaixa\":" + String(a.temperaturaBaixa ? "true" : "false") + ",";
        json += "\"temperaturaAlta\":"  + String(a.temperaturaAlta  ? "true" : "false") + ",";
        json += "\"umidadeBaixa\":"     + String(a.umidadeBaixa     ? "true" : "false") + ",";
        json += "\"umidadeAlta\":"      + String(a.umidadeAlta      ? "true" : "false");
        json += "}";

        server.send(200, "application/json", json);
    });

    server.begin();
    Serial.println("Servidor iniciado");
}

void WebServerManager::handle() {
    server.handleClient();
}