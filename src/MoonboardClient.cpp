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

void MoonboardClient::begin(BasicLog *_log, MBConfigData *_config) {
    m_log = _log;
    m_config = _config;
    m_btmPnl.begin(_log);
    m_midPnl.begin(_log);
    m_topPnl.begin(_log);
    m_btmPnl.setAddress(_config->btm_panel_ip, _config->btm_port);
    m_midPnl.setAddress(_config->mid_panel_ip, _config->mid_port);
    m_topPnl.setAddress(_config->top_panel_ip, _config->top_port);
}

BasicLog *MoonboardClient::getLog() {
    return m_log;
}

void MoonboardClient::setLog(BasicLog *_log) {
    m_log = _log;
}
