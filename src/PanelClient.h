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

typedef struct {
    int cmdId;
    uint32_t timeReceived;
} ack_t;

class PanelClient {
protected:
    WiFiClient m_client;
    BasicLog *m_log;
    int m_cmdId = 1;
    int m_rcvLen = 0;
    char rcvdBuf[PANEL_RCVBUF_LEN];
    char sendBuf[PANEL_SNDBUF_LEN];
    uint32_t m_ackTimeout;
    std::vector<ack_t> m_pendingAcks;

public:
    PanelClient(){};
    void begin(BasicLog *_log, uint32_t ackTimeout_ms = 60000);
    void stop();

    int clear(bool blocking = true);
    int lightHolds(const char *holdList, bool blocking = true);
    int reset(bool blocking = true);
    int ping(bool blocking = true);

    int sendCommand(const char *cmd, const char *data = NULL, bool blocking = true);

    // Blocking mode
    bool waitForAck(int cmdId);
    int receiveLine(uint32_t timeout);

    // Non-blocking mode. Call receive() in a loop, checking hasAckPending()
    void receive();
    bool hasAckPending(int cmdId);

    BasicLog *getLog() { return m_log; };
    void setLog(BasicLog *l) { m_log = l; };
    WiFiClient getClient() { return m_client; };
    void setClient(WiFiClient c) { m_client = c; };
    uint32_t getAckTimeout() { return m_ackTimeout; }
    void setAckTimeout(uint32_t _ackTimeout_ms) { m_ackTimeout = _ackTimeout_ms; }
};

#endif // #ifndef _PANEL_CLIENT_H