#ifndef _MOONBOARD_CLIENT_H
#define _MOONBOARD_CLIENT_H

#include <BasicLog.h>
#include "Data.h"

class MoonboardClient {
protected:
  BasicLog *m_log;
public:
  MoonboardClient() {};
  void begin(BasicLog *_log);
  BasicLog *getLog();
  void setLog(BasicLog *);
  bool readProblem(Problem *p, char *in);
};

#endif // #ifndef _MOONBOARD_CLIENT_H