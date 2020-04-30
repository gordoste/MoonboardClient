#include "MoonboardClient.h"

#ifndef MOONBOARD_DISABLED

int MoonboardClient::connect() {
    m_btmPnl.connect();
    m_midPnl.connect();
    m_topPnl.connect();
    if (!isConnected()) {
        m_log->error("Failed to connect to moonboard");
        stop();
        return 0;
    }
    m_log->log("Connected to moonboard");
    return 1;
}

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
    return m_btmPnl.getClient()->connected() &&
           m_midPnl.getClient()->connected() &&
           m_topPnl.getClient()->connected();
}

#else // #ifdef MOONBOARD_DISABLED

int MoonboardClient::connect() { return 1; }
void MoonboardClient::showProblem(Problem *p) { return; }
void MoonboardClient::clearBoard() { return; }
void MoonboardClient::stop() { return; }
bool MoonboardClient::isConnected() { return true; }
#endif

void MoonboardClient::begin(BasicLog *_log) {
    m_log = _log;
    m_btmPnl.begin(_log);
    m_midPnl.begin(_log);
    m_topPnl.begin(_log);
    m_btmPnl.setAddress(IPAddress(MB_IP_ADDRESS), MB_PANEL_BTM_PORT);
    m_midPnl.setAddress(IPAddress(MB_IP_ADDRESS), MB_PANEL_MID_PORT);
    m_topPnl.setAddress(IPAddress(MB_IP_ADDRESS), MB_PANEL_TOP_PORT);
}

BasicLog *MoonboardClient::getLog() {
    return m_log;
}

void MoonboardClient::setLog(BasicLog *_log) {
    m_log = _log;
}
