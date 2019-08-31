#ifndef _MOONBOARD_CLIENT_H
#define _MOONBOARD_CLIENT_H

#ifndef MB_IP_ADDRESS
#define MB_IP_ADDRESS 192,168,20,59
#endif

#ifndef MB_PANEL_BTM_PORT
#define MB_PANEL_BTM_PORT 4011
#endif

#ifndef MB_PANEL_MID_PORT
#define MB_PANEL_MID_PORT 4012
#endif

#ifndef MB_PANEL_TOP_PORT
#define MB_PANEL_TOP_PORT 4013
#endif

#include <BasicLog.h>
#include "PanelClient.h"
#include "Data.h"

class MoonboardClient {
protected:
  BasicLog *m_log;
  PanelClient m_btmPnl;
  PanelClient m_midPnl;
  PanelClient m_topPnl;
public:
  MoonboardClient() {};
  void begin(BasicLog *_log);
  int connect();
  BasicLog *getLog();
  void setLog(BasicLog *);
  bool readProblem(Problem *p, char *in);
  void showProblem(Problem *p);
  void clearBoard();
  void stop();
  bool isConnected();
};

#endif // #ifndef _MOONBOARD_CLIENT_H