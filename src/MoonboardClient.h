#ifndef _MOONBOARD_CLIENT_H
#define _MOONBOARD_CLIENT_H

#include "Data.h"
#include "PanelClient.h"
#include <BasicLog.h>

class MoonboardClient {
protected:
    BasicLog *m_log;
    PanelClient m_btmPnl;
    PanelClient m_midPnl;
    PanelClient m_topPnl;
    int m_btmCmdId; // IDs of pending commands
    int m_midCmdId;
    int m_topCmdId;

public:
    MoonboardClient(){};
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