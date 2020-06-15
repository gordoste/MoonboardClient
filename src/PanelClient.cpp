#include "PanelClient.h"

void PanelClient::begin(BasicLog *_log) {
    m_log = _log;
}

void PanelClient::stop() {
    if (!m_client.connected()) return;
    m_client.stop();
}

bool PanelClient::sendCommand(const char *cmd, const char *data) {
    m_log->debug2("sendCommand #%d (%s %s)", m_cmdId, cmd, data ? data : "");
    sprintf(sendBuf, "%s %d", cmd, m_cmdId);
    m_log->debug3("write:%s", sendBuf);
    m_client.write(sendBuf);
    if (data != NULL) {
        sprintf(sendBuf, " %s", data);
        m_log->debug3("write:%s", sendBuf);
        m_client.write(sendBuf);
    }
    m_client.write('\n');
    if (!waitForAck(m_cmdId)) {
        m_log->debug("ack #%d FAIL", m_cmdId);
        return false;
    }
    m_log->debug2("ack #%d OK", m_cmdId);
    m_cmdId++;
    return true;
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

int PanelClient::receiveLine(uint32_t timeout) {
    if (m_rcvLen == 0) {
        memset(rcvdBuf, 0, PANEL_RCVBUF_LEN);
    }
    uint32_t expiryTime = millis() + timeout;
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
        m_log->debug3("%li %li", millis(), expiryTime);
    }
    rcvdBuf[m_rcvLen] = '\0';
    return m_rcvLen;
};