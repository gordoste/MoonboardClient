#ifndef _PANEL_CLIENT_H
#define _PANEL_CLIENT_H

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <Wifi.h>
#endif

#include <BasicLog.h>
#include <SSD1306Console.h>

#define PANEL_RCVBUF_LEN 256  // Receive buffer length
#define PANEL_SNDBUF_LEN 256  // Send buffer length
#define PANEL_CONN_TIMEOUT 5 // Connect timeout (s)
#define PANEL_RCV_TIMEOUT 10  // Receive timeout (s)

class PanelClient {
protected:
    WiFiClient m_client;
    BasicLog *m_log;
    int m_cmdId = 1;
    int m_rcvLen = 0;
    char rcvdBuf[PANEL_RCVBUF_LEN];
    char sendBuf[PANEL_SNDBUF_LEN];

public:
    PanelClient(){};
    void begin(BasicLog *_log);
    void stop();
    bool sendCommand(const char *cmd, const char *data = NULL);
    void clearBoard();
    void lightHolds(const char *holdList);
    bool waitForAck(int cmdId);
    int receiveLine(uint32_t timeout);
    BasicLog *getLog() { return m_log; };
    void setLog(BasicLog *l) { m_log = l; };
    WiFiClient getClient() { return m_client; };
    void setClient(WiFiClient c) { m_client = c; };
};

#endif // #ifndef _PANEL_CLIENT_H