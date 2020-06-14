#include "MoonboardClient.h"

void MoonboardClient::begin(BasicLog *_log, MBConfigData *_config) {
    m_log = _log;
    m_config = _config;
    m_btmPnl.begin(_log);
    m_midPnl.begin(_log);
    m_topPnl.begin(_log);
}

#ifndef MOONBOARD_DISABLED

void MoonboardClient::showProblem(Problem *p) {
    if (!isConnected()) return;
    clearBoard();
    m_btmPnl.lightHolds(p->bottomHolds);
    m_midPnl.lightHolds(p->middleHolds);
    m_topPnl.lightHolds(p->topHolds);
}

void MoonboardClient::clearBoard() {
    m_btmPnl.clearBoard();
    m_midPnl.clearBoard();
    m_topPnl.clearBoard();
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
