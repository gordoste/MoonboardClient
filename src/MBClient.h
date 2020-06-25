#ifndef _MBCLIENT_H
#define _MBCLIENT_H

#include "MBData.h"
#include "MBPanelClient.h"
#include <BasicLog.h>

class MBClient {
protected:
    BasicLog *m_log;
    MBPanelClient m_btmPnl;
    MBPanelClient m_midPnl;
    MBPanelClient m_topPnl;
    int m_btmCmdId; // IDs of pending commands
    int m_midCmdId;
    int m_topCmdId;

public:
    MBClient(){};
    void begin(BasicLog *_log);
    BasicLog *getLog() { return m_log; };
    void setLog(BasicLog *l) { m_log = l; };
    void showProblem(Problem *p);
    void clear();
    void ping();
    void stop();
    bool isConnected();
    bool registerClient(const char *id, WiFiClient _conn);

private:
    void waitForPendingCmds();
};

#endif // #ifndef _MOONBOARD_CLIENT_H