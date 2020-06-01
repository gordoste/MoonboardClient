#include "PanelClient.h"

void PanelClient::begin(BasicLog *_log) {
    m_log = _log;
}

Client *PanelClient::getClient() {
    return &m_client;
}

BasicLog *PanelClient::getLog() {
    return m_log;
}

void PanelClient::setLog(BasicLog *_log) {
    m_log = _log;
}

void PanelClient::setAddress(const char *host, uint16_t port) {
    WiFi.hostByName(host, m_hostIP);
    m_port = port;
}

void PanelClient::setAddress(IPAddress host, uint16_t port) {
    m_hostIP = host;
    m_port = port;
}

int PanelClient::connect() {
    if (m_client.connected()) return 1;
    return m_client.connect(m_hostIP, m_port, PANEL_CONN_TIMEOUT);
}

void PanelClient::stop() {
    if (!m_client.connected()) return;
    m_client.stop();
}

void PanelClient::sendCommand(const char *cmd, const char *data) {
    m_log->debug2("sendCommand(%s %d %s)", cmd, m_cmdId, data ? data : "");
    sprintf(sendBuf, "%s %d", cmd, m_cmdId);
    m_log->debug("write:%s", sendBuf);
    m_client.write(sendBuf);
    if (data != NULL) {
        sprintf(sendBuf, " %s", data);
        m_log->debug("write:%s", sendBuf);
        m_client.write(sendBuf);
    }
    m_client.write('\n');
    if (!waitForAck(m_cmdId)) {
        m_log->debug("sendCommand failed");
    }
    m_cmdId++;
};

void PanelClient::clearBoard() {
    sendCommand("CLR");
};

void PanelClient::lightHolds(const char *holdList) {
    sendCommand("SET", holdList);
};

bool PanelClient::waitForAck(int cmdId) {
    m_log->debug2("waitForAck(%d)", cmdId);
    while (true) {
        if (receiveLine(PANEL_RCV_TIMEOUT * 1000) == 0) {
            return false;
        }
        m_log->debug3("wfa:%s", rcvdBuf);
        if (strncmp("ACK ", rcvdBuf, 4) == 0) {
            if (atoi(&(rcvdBuf[4])) == cmdId) {
                m_rcvLen = 0;
                return true;
            } else {
                m_log->debug("WFA: exp %d got %s", cmdId, &(rcvdBuf[4]));
                m_rcvLen = 0;
                return false;
            }
        } else {
            m_log->debug("WFA: exp ACK got %s", rcvdBuf);
            m_rcvLen = 0;
            return false;
        }
    }
};

int PanelClient::receiveLine(uint16_t timeout) {
    if (m_rcvLen == 0) {
        memset(rcvdBuf, 0, PANEL_RCVBUF_LEN);
    }
    uint16_t expiryTime = millis() + timeout;
    while (m_client.connected() && millis() < expiryTime) {
        char c;
        if (m_client.available()) {
            c = m_client.read();
            switch (c) {
            case '\r':
                break;
            case '\n':
                rcvdBuf[m_rcvLen] = '\0';
                return m_rcvLen;
                break;
            default:
                rcvdBuf[m_rcvLen++] = c;
            }
        } else {
            delay(500);
        }
    }
    rcvdBuf[m_rcvLen] = '\0';
    return m_rcvLen;
};