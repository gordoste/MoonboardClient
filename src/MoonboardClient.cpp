#include "MoonboardClient.h"

void MoonboardClient::begin(BasicLog *_log) {
    m_log = _log;
    m_btmPnl.begin(_log);
    m_midPnl.begin(_log);
    m_topPnl.begin(_log);
}

#ifndef MOONBOARD_DISABLED

void MoonboardClient::showProblem(Problem *p) {
    if (!isConnected()) return;
    clear();
    m_btmCmdId = m_btmPnl.lightHolds(p->bottomHolds, false);
    m_midCmdId = m_midPnl.lightHolds(p->middleHolds, false);
    m_topCmdId = m_topPnl.lightHolds(p->topHolds, false);
    waitForPendingCmds();
}

void MoonboardClient::clear() {
    m_btmCmdId = m_btmPnl.clear(false);
    m_midCmdId = m_midPnl.clear(false);
    m_topCmdId = m_topPnl.clear(false);
    waitForPendingCmds();
}

void MoonboardClient::stop() {
    m_btmPnl.stop();
    m_midPnl.stop();
    m_topPnl.stop();
    m_log->log("Disconnected from moonboard");
}

bool MoonboardClient::isConnected() {
    return m_btmPnl.connected() && m_midPnl.connected() && m_topPnl.connected();
}

void MoonboardClient::ping() {
    m_btmCmdId = m_btmPnl.ping(false);
    m_midCmdId = m_midPnl.ping(false);
    m_topCmdId = m_topPnl.ping(false);
    waitForPendingCmds(); 
}

bool MoonboardClient::registerClient(const char *id, WiFiClient _conn) {
    if (strcmp(id, "moonboard_top") == 0) {
        m_topPnl.setClient(_conn);
        return true;
    }
    if (strcmp(id, "moonboard_mid") == 0) {
        m_midPnl.setClient(_conn);
        return true;
    }
    if (strcmp(id, "moonboard_btm") == 0) {
        m_btmPnl.setClient(_conn);
        return true;
    }
    return false;
}

void MoonboardClient::waitForPendingCmds() {
    while (m_btmCmdId != 0 || m_midCmdId != 0 || m_topCmdId != 0) {
        if (m_btmCmdId != 0) {
            m_btmPnl.receive();
            if (!m_btmPnl.hasAckPending(m_btmCmdId)) m_btmCmdId = 0;
        }
        if (m_midCmdId != 0) {
            m_midPnl.receive();
            if (!m_midPnl.hasAckPending(m_midCmdId)) m_midCmdId = 0;
        }
        if (m_topCmdId != 0) {
            m_topPnl.receive();
            if (!m_topPnl.hasAckPending(m_topCmdId)) m_topCmdId = 0;
        }
    }
}

#else // #ifdef MOONBOARD_DISABLED

void MoonboardClient::showProblem(Problem *p) { return; }
void MoonboardClient::clear() { return; }
void MoonboardClient::stop() { return; }
bool MoonboardClient::ping() { }
bool MoonboardClient::isConnected() { return true; }
bool MoonboardClient::registerClient(const char *id, WiFiClient _conn) { return true; }
void MoonboardClient::waitForPendingCmds() { }
#endif
