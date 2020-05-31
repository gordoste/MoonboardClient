#ifndef _MOONBOARD_CLIENT_H
#define _MOONBOARD_CLIENT_H

#include "Data.h"
#include "MBConfig.h"
#include "PanelClient.h"
#include <BasicLog.h>

class MoonboardClient {
protected:
    BasicLog *m_log;
    MBConfigData *m_config;
    PanelClient m_btmPnl;
    PanelClient m_midPnl;
    PanelClient m_topPnl;
    uint32_t m_lastConnAttemptTime = 0;

public:
    MoonboardClient(){};
    void begin(BasicLog *_log, MBConfigData *_config);
    int tryConnect(uint16_t _retryTime = 10);
    int connect();
    BasicLog *getLog();
    void setLog(BasicLog *);
    bool readProblem(Problem *p, char *in);
    void showProblem(Problem *p);
    void clearBoard();
    void stop();
    bool isConnected();
    uint32_t getLastConnAttemptTime() { return m_lastConnAttemptTime; }
};

#endif // #ifndef _MOONBOARD_CLIENT_H