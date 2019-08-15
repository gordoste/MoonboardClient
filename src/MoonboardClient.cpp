#include "MoonboardClient.h"

int MoonboardClient::begin(BasicLog *_log)
{
    m_log = _log;
    m_btmPnl.begin(_log);
    m_midPnl.begin(_log);
    m_topPnl.begin(_log);
    m_btmPnl.setAddress(IPAddress(192,168,20,59),4011);
    m_midPnl.setAddress(IPAddress(192,168,20,59),4012);
    m_topPnl.setAddress(IPAddress(192,168,20,59),4013);
    m_btmPnl.connect();
    m_midPnl.connect();
    m_topPnl.connect();
    if (!isConnected()) {
      m_log->error("Failed to connect to moonboard");
      stop();
      return 0;
    }
    m_log->log("Connected to moonboard");
    return 1;
}

BasicLog *MoonboardClient::getLog()
{
  return m_log;
}

void MoonboardClient::setLog(BasicLog *_log)
{
  m_log = _log;
}

void MoonboardClient::showProblem(Problem *p)
{
  m_btmPnl.lightHolds(p->bottomHolds);
  m_midPnl.lightHolds(p->middleHolds);
  m_topPnl.lightHolds(p->topHolds);
}

// read the string, placing data in the problem struct passed. Return true on success
bool MoonboardClient::readProblem(Problem *p, char *in)
{
  char *ptr = NULL;
  char token[2] = ":"; // need room for null terminator
  // First grab the command type
  ptr = strtok(in, token);
  if (ptr == NULL)
  {
    return false;
  }
  p->name = ptr;
  ptr = strtok(NULL, token);
  if (ptr == NULL)
  {
    return false;
  }
  if (ptr[0] != 'V') {
    m_log->debug("V expected");
    return false;
  }
  p->grade = atoi(&(ptr[1]));
  if (p->grade == 0) {
    m_log->debug("Invalid grade");
    return false;
  }
  ptr = strtok(NULL, token);
  p->rating = atoi(ptr);
  if (p->rating == 0) {
    m_log->debug("Invalid rating");
    return false;
  }
  ptr = strtok(NULL, token);
  p->repeats = atoi(ptr);
  if (p->repeats == 0) {
    m_log->debug("Invalid repeats");
      return false;
  }
  ptr = strtok(NULL, token);
  switch (ptr[0]) {
    case 'Y': p->isBenchmark = true; break;
    case 'N': p->isBenchmark = false; break;
    default:
      m_log->debug("Invalid isBmark");
      return false;
  }
  ptr = strtok(NULL, token);
  if (ptr == NULL) {
    m_log->debug("Bottom holds not found");
  }
  p->bottomHolds = ptr;
  ptr = strtok(NULL, token);
  if (ptr == NULL) {
    m_log->debug("Middle holds not found");
  }
  p->middleHolds = ptr;
  ptr = strtok(NULL, token);
  if (ptr == NULL) {
    m_log->debug("Top holds not found");
  }
  p->topHolds = ptr;
  return true;
}

void MoonboardClient::clearBoard()
{
  m_btmPnl.clearBoard();
  m_midPnl.clearBoard();
  m_topPnl.clearBoard();
}

void MoonboardClient::stop()
{
  m_btmPnl.stop();
  m_midPnl.stop();
  m_topPnl.stop();
  m_log->log("Disconnected from moonboard");
}

bool MoonboardClient::isConnected()
{
  return m_btmPnl.getClient()->connected() &&
    m_midPnl.getClient()->connected() &&
    m_topPnl.getClient()->connected();
}