#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "WebServerManager.h"
#include "model/dto.h"

const char* ssid  = "SUA_REDE";
const char* senha = "SUA_SENHA";

static WebServer       server(80);
static MemoriaSistema* _memoria = nullptr;

void WebServerManager::begin(MemoriaSistema& mem) {
    _memoria = &mem;

    WiFi.begin(ssid, senha);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi conectado!");
    Serial.println(WiFi.localIP());

    if (!LittleFS.begin()) {
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
        EstadoSistema estado = _memoria->obterEstadoSistema();

        String json = "{";
        json += "\"temperatura\":" + String(estado.ultimaLeitura.temperatura) + ",";
        json += "\"umidade\":"     + String(estado.ultimaLeitura.umidade);
        json += "}";

        server.send(200, "application/json", json);
    });

    server.on("/api/modo", HTTP_POST, []() {
        if (!server.hasArg("plain")) {
            server.send(400, "application/json", "{\"erro\":\"sem body\"}");
            return;
        }

        String body = server.arg("plain");

        ModoSistema novoModo = DESLIGADO;
        if      (body.indexOf("automatico") >= 0) novoModo = AUTOMATICO;
        else if (body.indexOf("ligado")     >= 0) novoModo = LIGADO;
        else if (body.indexOf("desligado")  >= 0) novoModo = DESLIGADO;

        _memoria->salvarModo(novoModo);
        server.send(200, "application/json", "{\"ok\":true}");
    });

    server.on("/api/config", HTTP_POST, []() {
        if (!server.hasArg("plain")) {
            server.send(400, "application/json", "{\"erro\":\"sem body\"}");
            return;
        }

        String body = server.arg("plain");

        int iTempMin  = body.indexOf("\"tempMin\":");
        int iTempMax  = body.indexOf("\"tempMax\":");
        int iHumidMin = body.indexOf("\"humidMin\":");
        int iHumidMax = body.indexOf("\"humidMax\":");

        if (iTempMin < 0 || iTempMax < 0 || iHumidMin < 0 || iHumidMax < 0) {
            server.send(400, "application/json", "{\"erro\":\"campos ausentes\"}");
            return;
        }

        ConfiguracaoAlerta config;
        config.temperaturaMinima = body.substring(iTempMin  + 10, body.indexOf(',', iTempMin)).toFloat();
        config.temperaturaMaxima = body.substring(iTempMax  + 10, body.indexOf(',', iTempMax)).toFloat();
        config.umidadeMinima     = body.substring(iHumidMin + 11, body.indexOf(',', iHumidMin)).toFloat();
        config.umidadeMaxima     = body.substring(iHumidMax + 11, body.indexOf('}', iHumidMax)).toFloat();

        _memoria->salvarConfiguracao(config);
        server.send(200, "application/json", "{\"ok\":true}");
    });

    server.begin();
    Serial.println("Servidor iniciado");
}

void WebServerManager::handle() {
    server.handleClient();
}
