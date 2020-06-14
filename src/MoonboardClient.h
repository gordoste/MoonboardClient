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

public:
    MoonboardClient(){};
    void begin(BasicLog *_log);
    BasicLog *getLog() { return m_log; };
    void setLog(BasicLog *l) { m_log = l; };
    bool readProblem(Problem *p, char *in);
    void showProblem(Problem *p);
    void clearBoard();
    void stop();
    bool isConnected();
    bool registerClient(const char *id, WiFiClient _conn);
};

#endif // #ifndef _MOONBOARD_CLIENT_H