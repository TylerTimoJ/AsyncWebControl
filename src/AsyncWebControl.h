#ifndef ASYNCWEBCONTROL_H
#define ASYNCWEBCONTROL_H

#define ASYNC_TCP_RUNNING_CORE 0
#include "ESPAsyncWebServer.h"
#include "PageElement.h"
#include <DNSServer.h>
#include <string>
#include <vector>
#include <iostream>

class AsyncWebControl
{
private:
    AsyncWebServer *server;
    AsyncWebSocket *socket;
    DNSServer *dnsServer;
    std::string m_title;
    std::string indexHTML;

    uint8_t m_version;
    bool EEPROMChanged = false;

    std::vector<PageElement *> PageControls;
    
    size_t content_len; //for ota update

    void GetNavBarHTML(std::ostringstream& stream);
    std::string GetFullHTML();
    std::string GetFullJavaScript();
    std::string GetAboutHTML();
    std::string GetUpdateHTML();

    void onIndexRequest(AsyncWebServerRequest *request);
    void onCaptivePortalRedirect(AsyncWebServerRequest *request);
    void onPageNotFound(AsyncWebServerRequest *request);
    void onCSSRequest(AsyncWebServerRequest *request);
    void onAboutRequest(AsyncWebServerRequest *request);
    void handleUpdate(AsyncWebServerRequest *request);

    void handleDoUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final);
    void printProgress(size_t prg, size_t sz);

    void SendAllWSData();
    void SendMessageToAll(std::string message);
    void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

public:
    std::string color_grey = "#cccccc";
    std::string color_background = "#ebebeb";
    std::string color_dark = "#1a5276";
    std::string color_neutral = "#2471a3";
    std::string color_light = "#7fb3d5";
    

    AsyncWebControl(std::string title, uint8_t versionNumber = 0);

    void BeginServer();

    void UpdateElement(PageElement &element);

    void Add(PageElement &element);

    void Loop();
};

#endif