#ifndef _PANEL_CLIENT_H
#define _PANEL_CLIENT_H

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <Wifi.h>
#endif

#include <SSD1306Console.h>
#include <BasicLog.h>

class PanelClient {
protected:
  Client *m_client;
  BasicLog *m_log;
public:
  PanelClient() {};
  void begin(BasicLog *_log);
  BasicLog *getLog();
  void setLog(BasicLog *);
  Client *getClient();
  void setClient(Client *client);
};

#endif // #ifndef _PANEL_CLIENT_H