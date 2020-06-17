#include "PanelClient.h"

void PanelClient::begin(BasicLog *_log, uint32_t ackTimeout_ms) {
    m_log = _log;
    m_ackTimeout = ackTimeout_ms;
}

void PanelClient::stop() {
    if (!m_client.connected()) return;
    m_client.stop();
}

// Returns ID of command sent, 0 on failure
int PanelClient::sendCommand(const char *cmd, const char *data, bool blocking) {
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
    if (blocking) {
        if (!waitForAck(m_cmdId)) return 0;
        m_log->debug2("ack #%d OK", m_cmdId);
    }
    return m_cmdId++;
};

// Returns ID of command sent, 0 on failure
int PanelClient::clear(bool blocking) {
    return sendCommand("CLR", NULL, blocking);
};

// Returns ID of command sent, 0 on failure
int PanelClient::lightHolds(const char *holdList, bool blocking) {
    return sendCommand("SET", holdList, blocking);
};

// Returns ID of command sent, 0 on failure
int PanelClient::reset(bool blocking) {
    return sendCommand("RST", NULL, blocking);
}

// Returns ID of command sent, 0 on failure
int PanelClient::ping(bool blocking) {
    return sendCommand("PING", NULL, blocking);
}

// Blocking call which waits for ACK with specified ID to be received
bool PanelClient::waitForAck(int cmdId) {
    m_log->debug2("waitForAck(%d)", cmdId);
    while (true) {
        if (receiveLine(PANEL_RCV_TIMEOUT * 1000) == 0) {
            m_log->debug("WFA timeout");
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

// Blocking call which waits for a line to be received
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

// Use with non-blocking sendCommand() - call in a loop to check for ACKs
void PanelClient::receive() {
    if (!m_client.connected()) return;
    if (m_rcvLen == 0) memset(rcvdBuf, 0, PANEL_RCVBUF_LEN);
    char c;
    while (m_client.available()) {
        c = m_client.read();
        switch (c) {
        case '\r':
            break;
        case '\n':
            rcvdBuf[m_rcvLen] = '\0';
            m_rcvLen = 0;
            if (strncmp("ACK ", rcvdBuf, 4) == 0) {
                int id = atoi(&(rcvdBuf[4]));
                if (id > 0) {
                    for (auto it = m_pendingAcks.begin(); it != m_pendingAcks.end(); it++) {
                        if ((*it).cmdId == id) m_pendingAcks.erase(it);
                    }
                    return;
                }
            }
            m_log->error("receive() bad fmt: '%s'", rcvdBuf);
            return;
            break;
        default:
            rcvdBuf[m_rcvLen++] = c;
        }
    }
}

bool PanelClient::hasAckPending(int _cmdId) {
    uint32_t now = millis();
    // Time out ACKs - they will be in order of receipt
    for (auto it = m_pendingAcks.begin(); it != m_pendingAcks.end() && now > ((*it).timeReceived + m_ackTimeout); it++) {
        m_pendingAcks.erase(it);
    }
    for (auto it = m_pendingAcks.begin(); it != m_pendingAcks.end(); it++) {
        if ((*it).cmdId == _cmdId) return true;
    }
    return false;
}
