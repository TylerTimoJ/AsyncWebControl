#include "AsyncWebControl.h"
#include <Update.h>
#include <EEPROM.h>
#include <WiFi.h>

std::string AsyncWebControl::GetFullHTML()
{
  std::ostringstream stream;

  GetNavBarHTML(stream);
  //stream << GetNavBarHTML();

  for (int i = 0; i < PageControls.size(); i++)
  {

    PageControls[i]->GetHTML(stream);
  }
  stream << "</div>\n"
         << GetFullJavaScript()
         << "</body>\n</html>";

  return stream.str();
}

void AsyncWebControl::GetNavBarHTML(std::ostringstream &stream)
{
  //std::ostringstream stream;

  stream << "<!doctype html>\n<html lang=en dir=ltr>\n<head>\n<meta charset=utf-8>\n<meta name=viewport content=\"width=device-width, initial-scale=1\">\n<title>"
         << m_title
         << "</title>\n<link href=style.css rel=stylesheet type=text/css>\n</head>\n<body>\n<nav>\n<input class=menuCheck type=checkbox id=menuCheck>\n<label for=menuCheck class=checkbtn>&#9776;</label>\n<label class=logo>"
         << m_title
         << "</label>\n<ul>\n<li><a class=active href=/>Home</a></li>\n<li><a href=/update>Update</a></li>\n<li><a href=/about>About</a></li>\n</ul>\n</nav>\n<div class=main>\n";
  //return stream.str();
}

std::string AsyncWebControl::GetAboutHTML()
{
  std::ostringstream stream;
  GetNavBarHTML(stream);

  // stream << GetNavBarHTML()
  stream << "<h1>Open Source Licenses</h1> <p>FastLED<br/><br/> The MIT License (MIT)<br/><br/> Copyright (c) 2013 FastLED<br/><br/> Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the \"Software\"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:<br/> The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.<br/> THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</p> <h1>Acknowledgements</h1> <p>WiFi Controlled Parallel LED Driver v3.1<br/><br/> Software Revision 1.2 September 2020<br/><br/> Copyright (c) 2020 Tyler Timo Jones</p>"
         << "</div>\n</body>\n</html>";
  return stream.str();
}

std::string AsyncWebControl::GetUpdateHTML()
{
  std::ostringstream stream;
  GetNavBarHTML(stream);

  // stream << GetNavBarHTML()
  stream << "<h1>Firmware Update</h1> <form method=POST action=/doUpdate enctype=multipart/form-data> <input type=file name=update> <input type=submit value=Update> </form>"
         << "</div>\n</body>\n</html>";
  return stream.str();
}

std::string AsyncWebControl::GetFullJavaScript()
{
  IPAddress apIP = WiFi.softAPIP();

  std::ostringstream stream;
  stream << "\n<script>\nwindow.addEventListener(\"load\", init, false);\nvar url = \"ws://"
         << apIP.toString().c_str()
         << "/ws\";\n";

  //global variables
  for (int i = 0; i < PageControls.size(); i++)
  {
    stream << "var ";
    stream << "VAR_" << PageControls[i]->m_ID.c_str() << ";\n";
  }

  //function init
  stream << "function init() {\n";
  for (int i = 0; i < PageControls.size(); i++)
  {
    stream << "VAR_"
           << PageControls[i]->m_ID.c_str()
           << " = document.getElementById(\""
           << PageControls[i]->m_ID
           << "\");\nVAR_"
           << PageControls[i]->m_ID.c_str()
           << ".disabled = true;\n"; //disables controls at startup
  }
  stream << "wsConnect(url);\n}\n";

  //function wsConnect
  stream << "function wsConnect(url) {\nws = new WebSocket(url);\n"
         << "ws.onopen = function (evt) { onOpen(evt); };\n"
         << "ws.onclose = function (evt) { onClose(evt); };\n"
         << "ws.onmessage = function (evt) { onMessage(evt); };\n"
         << "ws.onerror = function (evt) { onError(evt); };\n}\n";

  //function onError
  stream << "function onError(evt) { }\n";
  //function doSend
  stream << "function doSend(message) {\nif(ws.readyState == WebSocket.OPEN) {\nws.send(message);\n}\n}\n";

  //function onOpen
  stream << "function onOpen(evt) {\n";
  for (int i = 0; i < PageControls.size(); i++)
  {
    stream << "VAR_" << PageControls[i]->m_ID.c_str() << ".disabled = false;\n"; //disables controls at startup
  }
  stream << "doSend(\"GET_ALL\");\n}\n";

  //function onClose

  stream << "function onClose(evt) {\n";
  for (int i = 0; i < PageControls.size(); i++)
  {
    stream << "VAR_" << PageControls[i]->m_ID.c_str() << ".disabled = true;\n"; //disables controls at startup
  }
  stream << "setTimeout(function () { wsConnect(url); }, 2000);\n}\n";

  //function onMessage

  stream << "function onMessage(evt) {\nif (evt.data == \"NULL\") {  }\n";
  //else ifs
  for (int i = 0; i < PageControls.size(); i++)
  {
    stream << PageControls[i]->GetJavaOnMessageStatment() << "\n";
  }

  stream << "}\n";
  // change and click functions
  for (int i = 0; i < PageControls.size(); i++)
  {
    stream << PageControls[i]->GetJavaCommandFunction() << "\n";
  }

  //close tag
  stream << "</script>\n";
  return stream.str();
}


void AsyncWebControl::onIndexRequest(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", GetFullHTML().c_str());
}

void AsyncWebControl::onCaptivePortalRedirect(AsyncWebServerRequest *request)
{
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("Captive Request: [" + remote_ip.toString() + "] HTTP GET request of " + request->url());
  request->redirect("http://192.168.4.1/");
}

void AsyncWebControl::onPageNotFound(AsyncWebServerRequest *request)
{
  if (request->url() != "/")
  {
    request->redirect("http://192.168.4.1/");
  }
}

void AsyncWebControl::onCSSRequest(AsyncWebServerRequest *request)
{
  request->send(200, "text/css", ":root{--c-gry:#ccc;--c-bkd:#ebebeb;--c-dbl:#1a5276;--c-blu:#2471a3;--c-lbl:#7fb3d5}.box select{position:relative;background-color:var(--c-blu);color:white;padding:12px;width:80%;height:48px;max-width:600px;border:0;border-radius:8px;font-size:20px;text-align-last:center;-webkit-appearance:button;appearance:button;outline:0}.box::before{position:absolute;top:0;right:0;width:80%;height:100%;text-align-last:center;font-size:28px;line-height:45px;color:rgba(255,255,255,0.5);background-color:rgba(255,255,255,0.1);pointer-events:none}.box:hover::before{color:rgba(255,255,255,0.6);background-color:rgba(255,255,255,0.2)}.box select option{padding:30px;text-align:center}.switch{position:relative;left:50%;transform:translateX(-50%);display:block;width:96px;height:48px}.switch input{opacity:0;width:0;height:0}.slider{position:absolute;cursor:pointer;top:0;left:0;right:0;bottom:0;background-color:var(--c-gry);-webkit-transition:.1s;transition:.1s}.slider:before{position:absolute;content:\"\";height:40px;width:40px;left:4px;bottom:4px;background-color:white;-webkit-transition:.1s;transition:.1s}input:checked+.slider{background-color:var(--c-lbl)}input:checked+.slider:before{-webkit-transform:translateX(48px);-ms-transform:translateX(48px);transform:translateX(48px)}.slider.round{border-radius:24px}.slider.round:before{border-radius:50%}h1{font-size:32px;margin:4px;margin-top:48px}h1:first-child{font-size:32px;margin:4px}.rangeSlider{-webkit-appearance:none;margin:16px;max-width:600px;width:85%;height:20px;border-radius:10px;background:var(--c-gry);outline:0;opacity:.7;-webkit-transition:.2s;transition:opacity .2s}.rangeSlider::-webkit-slider-thumb{-webkit-appearance:none;appearance:none;width:40px;height:40px;border-radius:50%;background:white;cursor:pointer}.rangeSlider::-moz-range-thumb{width:40px;height:40px;border-radius:50%;background:white;cursor:pointer}.main{padding:16px;top:80px;min-width:300px;width:80vw;word-wrap:break-word;text-align:center;left:50%;transform:translateX(-50%);position:relative}p{text-align:justify;margin-top:32px}*{padding:0;margin:0;text-decoration:none;list-style:none;box-sizing:border-box}body{font-family:sans-serif;background-color:var(--c-bkd)}nav{z-index:1;position:fixed;background:var(--c-blu);height:80px;width:100%}label.logo{color:white;font-size:32px;line-height:80px;padding-left:16px;font-weight:bold}nav ul{float:right;margin-right:20px}nav ul li{display:inline-block;line-height:80px;margin:0 5px}nav ul li a{color:white;font-size:17px;padding:7px 13px;border-radius:3px}a.active,a:hover{background:var(--c-lbl);transition:.5s}.checkbtn{font-size:30px;color:white;float:right;line-height:80px;margin-right:32px;cursor:pointer;display:none}.menuCheck{display:none}@media(max-width :700px){label.logo{font-size:28px}.checkbtn{display:block}ul{position:fixed;width:100%;height:100vh;background:var(--c-dbl);top:80px;left:-100%;text-align:center;transition:all .5s}nav ul li{display:block;margin:50px 0;line-height:30px}nav ul li a{font-size:20px}a:hover,a.active{background:0;color:white}.menuCheck:checked ~ ul{left:0}}");
}

void AsyncWebControl::onAboutRequest(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", GetAboutHTML().c_str());
}

void AsyncWebControl::handleUpdate(AsyncWebServerRequest *request)
{
  request->send(200, "text/html", GetUpdateHTML().c_str());
}

void AsyncWebControl::handleDoUpdate(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index)
  {
    Serial.println("Update");
    content_len = request->contentLength();
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH))
    {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len)
  {
    Update.printError(Serial);
  }

  if (final)
  {
    AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "Please wait while the device reboots");
    response->addHeader("Refresh", "20");
    response->addHeader("Location", "/");
    request->send(response);
    if (!Update.end(true))
    {
      Update.printError(Serial);
    }
    else
    {
      Serial.println("Update complete");
      Serial.flush();
      ESP.restart();
    }
  }
}

void AsyncWebControl::printProgress(size_t prg, size_t sz)
{
  Serial.printf("Progress: %d%%\n", (prg * 100) / content_len);
}


void AsyncWebControl::UpdateClients()
{
  SendAllWSData();
}

void AsyncWebControl::SendAllWSData()
{
  for (int i = 0; i < PageControls.size(); i++)
  {
    SendMessageToAll(PageControls[i]->GetSocketMessage());
  }
}

void AsyncWebControl::SendMessageToAll(std::string message)
{
  socket->textAll(message.c_str());
  Serial.print("Sending to all : ");
  Serial.println(message.c_str());
}

void AsyncWebControl::onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_DISCONNECT:
  {
    Serial.printf("[%u] Disconnected!\n", client->id());
    break;
  }

  case WS_EVT_CONNECT:
  {
    IPAddress ip = client->remoteIP();
    Serial.printf("[%u] Connection from ", client->id());
    Serial.println(ip.toString());
    //webSocket.cleanupClients();
    break;
  }

  case WS_EVT_DATA:
  {
    std::string message((char *)data);

    message.resize(len);

    Serial.printf("Message From [%u] %s\n", client->id(), message.c_str());

    if (message == "GET_ALL")
    {
      SendAllWSData();
      break;
    }

    for (int i = 0; i < PageControls.size(); i++)
    {
      if (PageControls[i]->ProcessSocketMessage(message))
      {
        //send to everyone
        SendMessageToAll(PageControls[i]->GetSocketMessage());

        //write value(s) to eeprom
        EEPROM.writeBytes((i * 4) + 1, PageControls[i]->GetEEPROMData().data, 4);

        //Serial.printf("Writing EEPROM: PageControl:[%d] %d,%d,%d,%d\n", i, PageControls[i]->GetEEPROMData().data[0], PageControls[i]->GetEEPROMData().data[1], PageControls[i]->GetEEPROMData().data[2], PageControls[i]->GetEEPROMData().data[3]);
        EEPROMChanged = true;

        break;
      }
    }
    break;
  }

  default:
    break;
  }
}

AsyncWebControl::AsyncWebControl(std::string title, uint8_t versionNumber)
    : m_title(title), m_version(versionNumber)
{
  server = new AsyncWebServer(80);
  socket = new AsyncWebSocket("/ws");
}

void AsyncWebControl::BeginServer()
{
  // templates for calling non static member functions
  std::function<void(AsyncWebServerRequest * request)> OnIndex_Helper = std::bind(&AsyncWebControl::onIndexRequest, this, std::placeholders::_1);
  std::function<void(AsyncWebServerRequest * request)> OnCSS_Helper = std::bind(&AsyncWebControl::onCSSRequest, this, std::placeholders::_1);
  std::function<void(AsyncWebServerRequest * request)> OnAbout_Helper = std::bind(&AsyncWebControl::onAboutRequest, this, std::placeholders::_1);
  std::function<void(AsyncWebServerRequest * request)> OnHotspot_Helper = std::bind(&AsyncWebControl::onCaptivePortalRedirect, this, std::placeholders::_1);
  std::function<void(AsyncWebServerRequest * request)> OnPageNotFound_Helper = std::bind(&AsyncWebControl::onPageNotFound, this, std::placeholders::_1);
  std::function<void(AsyncWebServerRequest * request)> HandleUpdate_Helper = std::bind(&AsyncWebControl::handleUpdate, this, std::placeholders::_1);
  std::function<void(AsyncWebServerRequest * request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)>
      handleDoUpdate_Helper = std::bind(&AsyncWebControl::handleDoUpdate, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

  std::function<void(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void *arg, uint8_t *data, size_t len)>
      OnSocket_Helper = std::bind(&AsyncWebControl::onWebSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);

  // register handlers
  server->on("/", HTTP_GET, OnIndex_Helper);
  server->on("/style.css", HTTP_GET, OnCSS_Helper);
  server->on("/about", HTTP_GET, OnAbout_Helper);
  server->on("/hotspot-detect.html", HTTP_GET, OnHotspot_Helper);
  server->onNotFound(OnPageNotFound_Helper);
  server->on("/update", HTTP_GET, HandleUpdate_Helper);
  server->on(
      "/doUpdate", HTTP_POST, [](AsyncWebServerRequest *request) {}, handleDoUpdate_Helper);
  socket->onEvent(OnSocket_Helper);
  server->addHandler(socket);

  dnsServer = new DNSServer;
  dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer->start(53, "*", WiFi.softAPIP());

  //load values from EEPROM
  EEPROM.begin((PageControls.size() * 4) + 1);

  Serial.println();
  Serial.printf("Stored Version = %d\tCurrent Version = %d\n", EEPROM.read(0), m_version);

  if (EEPROM.read(0) != m_version)
  {
    Serial.printf("New EEPROM Version\nErasing Data\n");

    EEPROM.write(0, m_version);

    for (int i = 1; i < (PageControls.size() * 4) + 1; i++)
    {
      EEPROM.write(i, 0);
    }
    EEPROM.commit();
  }

  for (int i = 0; i < PageControls.size(); i++)
  {
    PageElement::EEPROM_Data initData;

    EEPROM.readBytes((i * 4) + 1, (void *)initData.data, 4);

    PageControls[i]->InitializeData(initData);

    for (int e = 0; e < 4; e++)
    {
      Serial.printf("PageControl:[%d] Byte:[%d] = %d\n", i, e, initData.data[e]);
    }
  }

  server->begin();
  Serial.println(GetFullHTML().c_str());
}

void AsyncWebControl::Add(PageElement &element)
{
  std::stringstream stream;
  stream << (int)(PageControls.size());

  element.m_ID = "CTRL_" + std::string(3 - stream.str().length(), '0') + stream.str();
  PageControls.push_back(&element);
}

void AsyncWebControl::Loop()
{
  dnsServer->processNextRequest();
  if (EEPROMChanged)
  {
    EEPROM.commit();
    EEPROMChanged = false;
  }
}