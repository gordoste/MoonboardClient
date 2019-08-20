#include "MoonboardUtils.h"

//void MoonboardUtils::setBuf(char *buf, uint8_t bufLen) {
  //m_buf = buf;
  //m_bufLen = bufLen;
//}

void MoonboardUtils::beginCatType(char *catType) {
  catTypeNames[numCatTypes] = catType;
  catNameStartIdxPerType[numCatTypes] = numCatNames;
  catCountPerType[numCatTypes] = 0;
  selectedCatPerType[numCatTypes] = -1;
}

void MoonboardUtils::endCatType() {
  numCatTypes++;
}

void MoonboardUtils::addCat(const char *catName) {
  catNames[numCatNames++] = strcpy(t_catBufPtr, catName);
  t_catBufPtr += strlen(catName)+1; // +1 for null terminator
  catCountPerType[numCatTypes]++;
}

// Pass colon-delimited string
// First token is cat type, others are taken as category names
void MoonboardUtils::defineCatType(const char *catType) {
  strcpy(m_buf, catType);
  char t[2] = ":";
  char *p = strtok(m_buf, t);
  beginCatType(p);
  while ((p = strtok(NULL, t))) {
    addCat(p);
  }
  endCatType();
}

void MoonboardUtils::selectCat(uint8_t z_catType, uint8_t z_catNum) {
  if (z_catType >= numCatTypes) { Serial.printf("Bad catType #%d\n", z_catType); }
  if (z_catNum >= catCountPerType[z_catType]) {
    Serial.printf("Bad catNum #%d >= %d\n", z_catNum, catCountPerType[z_catType]);
  }
  selectedCatPerType[z_catType] = z_catNum;
}

void MoonboardUtils::unselectCat(uint8_t z_catType) {
  if (z_catType >= numCatTypes) { Serial.printf("Bad catType #%d\n", z_catType); }
  selectedCatPerType[z_catType] = -1;
}

void MoonboardUtils::getListNameFromCats(char *buf, uint8_t bufLen) {
  uint8_t copied = 0;
  for (uint8_t l_catTypeNum = 0; l_catTypeNum < numCatTypes; l_catTypeNum++) {
    char *l_catName;
    if (selectedCatPerType[l_catTypeNum] == -1) {
      l_catName = wildcardStr;
    }
    else {
      l_catName = catNames[catNameStartIdxPerType[l_catTypeNum] + selectedCatPerType[l_catTypeNum]];
    }
    uint8_t len = strlen(l_catName);
    if (len + 2 > bufLen) { // add delimiter and null-terminator
      Serial.printf("gLNFC - %s too long for buffer\n", l_catName);
      return;
    }
    strcpy(&(buf[copied]), l_catName);
    copied += len;
    if (numCatTypes - l_catTypeNum > 1) { // Skip delimiter on last one
      strcpy(&(buf[copied++]), "_");
    }
  }
}
void MoonboardUtils::findLists() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  char *listNameBufPtr = listNameBuf; // Point to unused storage for list names
  uint8_t listNum = 0; // Keep track of how many list names we've added
  while (file) {
    uint8_t l = strlen(file.name());
    if (strncmp(&(file.name()[l-4]),".dat",4) == 0) {
      if (l-5 > MAX_LISTNAME_SIZE) { // 5 = 1 (/ prefix) + 4 (.dat suffix)
        Serial.printf("List name %s too long\n", file.name());
      }
      else {
        // Copy the list name into the space reserved and record where we copied it
        listNames[listNum++] = strncpy(listNameBufPtr, &(file.name()[1]), l-5);
        listNameBufPtr[l-5] = '\0'; // Null terminate the string
        listNameBufPtr = &(listNameBufPtr[l-4]); // Move our pointer along to the next free space
      }
    }
    file = root.openNextFile();
  }
}

void MoonboardUtils::openList(const char *listName, const char *sortOrder) {
  sprintf(m_buf, "/%s_%s.lst", listName, sortOrder);
  m_list = SPIFFS.open(m_buf);
  sprintf(m_buf, "/%s.dat", listName);
  m_data = SPIFFS.open(m_buf);
}

void MoonboardUtils::closeList() {
  m_list.close();
  m_data.close();
}

void MoonboardUtils::readNextProblem(Problem *prob) {
  const char t[2] = ":";
  char *p;
  int offset;
  m_list.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
  p = strtok(m_buf, t);
  p = strtok(NULL, t);
  offset = atoi(p);
  m_data.seek(offset, SeekSet);
  m_data.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
  readProblem(prob, m_buf);
}

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
