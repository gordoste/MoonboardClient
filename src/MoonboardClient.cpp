#include "MoonboardClient.h"

void MoonboardClient::begin(BasicLog *_log)
{
    m_log = _log;
}

BasicLog *MoonboardClient::getLog()
{
  return m_log;
}

void MoonboardClient::setLog(BasicLog *_log)
{
  m_log = _log;
}
