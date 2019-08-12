#ifndef _MOONBOARD_CLIENT_H
#define _MOONBOARD_CLIENT_H

#include <BasicLog.h>

class MoonboardClient {
protected:
  BasicLog *m_log;
public:
  MoonboardClient() {};
  void begin(BasicLog *_log);
  BasicLog *getLog();
  void setLog(BasicLog *);
};

#endif // #ifndef _MOONBOARD_CLIENT_H