#include "PanelClient.h"

void PanelClient::begin(BasicLog *_log)
{
    m_log = _log;
}

Client *PanelClient::getClient()
{
  return m_client;
}

void PanelClient::setClient(Client *client)
{
  m_client = client;
}

BasicLog *PanelClient::getLog()
{
  return m_log;
}

void PanelClient::setLog(BasicLog *_log)
{
  m_log = _log;
}
