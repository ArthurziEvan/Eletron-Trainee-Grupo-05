#include <WiFi.h>
#include <WebServer.h>
#include "WebServerManager.h"
#include "storage/MemoriaSistema.h"

extern MemoriaSistema memoria;

WebServer server(80);

const char *ssid = "SUA_REDE";
const char *senha = "SUA_SENHA";

void iniciarWiFi()
{
    WiFi.begin(ssid, senha);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi conectado!");
    Serial.println(WiFi.localIP());
}

void iniciarServidor()
{

    server.on("/api/dados", HTTP_GET, []()
              {

        EstadoSistema estado = memoria.obterEstadoSistema();

        String json = "{";
        json += "\"temperatura\":" + String(estado.ultimaLeitura.temperatura) + ",";
        json += "\"umidade\":" + String(estado.ultimaLeitura.umidade);
        json += "}";

        server.send(200, "application/json", json); });

    server.begin();
}

void atualizarServidor()
{
    server.handleClient();
}