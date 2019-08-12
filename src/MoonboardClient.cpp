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