#include "MoonboardUtils.h"

// read the string, placing data in the problem struct passed. Return true on success
bool MoonboardUtils::readProblem(Problem *p, char *in)
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
    return false;
  }
  p->grade = atoi(&(ptr[1]));
  if (p->grade == 0) {
    return false;
  }
  ptr = strtok(NULL, token);
  p->rating = atoi(ptr);
  if (p->rating == 0) {
    return false;
  }
  ptr = strtok(NULL, token);
  p->repeats = atoi(ptr);
  if (p->repeats == 0) {
    return false;
  }
  ptr = strtok(NULL, token);
  switch (ptr[0]) {
    case 'Y': p->isBenchmark = true; break;
    case 'N': p->isBenchmark = false; break;
    default:
      return false;
  }
  ptr = strtok(NULL, token);
  p->bottomHolds = ptr;
  ptr = strtok(NULL, token);
  p->middleHolds = ptr;
  ptr = strtok(NULL, token);
  p->topHolds = ptr;
  return true;
}
