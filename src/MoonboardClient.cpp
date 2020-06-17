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
    clearBoard();
    int btmCmd = m_btmPnl.lightHolds(p->bottomHolds, false);
    int midCmd = m_midPnl.lightHolds(p->middleHolds, false);
    int topCmd = m_topPnl.lightHolds(p->topHolds, false);
    while (btmCmd != 0 || midCmd != 0 || topCmd != 0) {
        if (btmCmd != 0) {
            m_btmPnl.receive();
            if (!m_btmPnl.hasAckPending(btmCmd)) btmCmd = 0;
        }
        if (midCmd != 0) {
            m_midPnl.receive();
            if (!m_midPnl.hasAckPending(midCmd)) midCmd = 0;
        }
        if (topCmd != 0) {
            m_topPnl.receive();
            if (!m_topPnl.hasAckPending(topCmd)) topCmd = 0;
        }
    }
}

void MoonboardClient::clearBoard() {
    m_btmPnl.clear();
    m_midPnl.clear();
    m_topPnl.clear();
}

void MoonboardClient::stop() {
    m_btmPnl.stop();
    m_midPnl.stop();
    m_topPnl.stop();
    m_log->log("Disconnected from moonboard");
}

bool MoonboardClient::isConnected() {
    return (m_btmPnl.getClient() && m_btmPnl.getClient().connected()) &&
           (m_midPnl.getClient() && m_midPnl.getClient().connected()) &&
           (m_topPnl.getClient() && m_topPnl.getClient().connected());
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

#else // #ifdef MOONBOARD_DISABLED

void MoonboardClient::showProblem(Problem *p) { return; }
void MoonboardClient::clearBoard() { return; }
void MoonboardClient::stop() { return; }
bool MoonboardClient::isConnected() { return true; }
bool MoonboardClient::registerClient(const char *id, WiFiClient _conn) { return true; }

#endif
