#ifndef _MOONBOARD_CLIENT_H
#define _MOONBOARD_CLIENT_H

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