#include "MoonboardUtils.h"

MoonboardUtils::MoonboardUtils(char *buf, uint16_t bufLen) {
  m_buf = buf;
  m_bufLen = bufLen;
}

void MoonboardUtils::beginCatType(char *catTypeName) {
  strncpy(m_catTypes[numCatTypes].name, catTypeName, sizeof(CategoryType::name));
  // Ensure string is terminated
  m_catTypes[numCatTypes].name[sizeof(CategoryType::name)-1] = '\0';

  m_catTypes[numCatTypes].catStartIdx = numCatNames;
  m_catTypes[numCatTypes].catCount = 0;
  m_catTypes[numCatTypes].selectedCat = -1;
}

void MoonboardUtils::endCatType() {
  numCatTypes++;
}

void MoonboardUtils::addCat(const char *catName) {
  _t_uint8_t = strlen(catName) + 1; // +1 for null terminator
  if (t_catBufPtr - catBuf + _t_uint8_t > sizeof(catBuf)) {
    Serial.println("MBU::aC - Exhausted catBuf");
    return;
  }
  catNames[numCatNames++] = strcpy(t_catBufPtr, catName);
  t_catBufPtr += _t_uint8_t; 
  m_catTypes[numCatTypes].catCount++;
}

// Pass colon-delimited string. First token is cat type name, others are taken as category names
void MoonboardUtils::addCatType(const char *catType) {
  strcpy(m_buf, catType);
  strcpy(t_strtok, ":");
  char *p = strtok(m_buf, t_strtok);
  beginCatType(p);
  while ((p = strtok(NULL, t_strtok))) {
    addCat(p);
  }
  endCatType();
}

void MoonboardUtils::selectCat(uint8_t z_catType, uint8_t z_catNum) {
  if (z_catType >= numCatTypes) { Serial.printf("MBU::sC - Bad catType #%d\n", z_catType); }
  if (z_catNum >= m_catTypes[z_catType].catCount) {
    Serial.printf("MBU::sC - Bad catNum #%d >= %d\n", z_catNum, m_catTypes[z_catType].catCount);
  }
  m_catTypes[z_catType].selectedCat = z_catNum;
}

void MoonboardUtils::selectCat(const char *catTypeName, uint8_t z_catNum) {
  _t_int8_t = catTypeNameToNum(catTypeName);
  if (_t_int8_t == -1) { Serial.printf("MBU::sC - Bad catType '%s'\n", catTypeName); }
  selectCat(_t_int8_t, z_catNum);
}

void MoonboardUtils::unselectCat(uint8_t z_catType) {
  if (z_catType >= numCatTypes) { Serial.printf("MBU::uC - Bad catType #%d\n", z_catType); }
  m_catTypes[z_catType].selectedCat = -1;
}

void MoonboardUtils::unselectCat(const char *catTypeName) {
  _t_int8_t = catTypeNameToNum(catTypeName);
  if (_t_int8_t == -1) { Serial.printf("MBU::uC - Bad catType '%s'\n", catTypeName); }
  unselectCat(_t_int8_t);
}

// Get the name of the selected category for specified cat type. Return NULL if none selected
char *MoonboardUtils::getSelectedCatName(uint8_t z_catType) {
  if (m_catTypes[z_catType].selectedCat == -1) { return NULL; }
  return catNames[m_catTypes[z_catType].catStartIdx + m_catTypes[z_catType].selectedCat];
}

// Search for a category type with specified name and return the index. -1 if not found
int8_t MoonboardUtils::catTypeNameToNum(const char *catTypeName) {
  for (_t_uint8_t = 0; _t_uint8_t < numCatTypes; _t_uint8_t++) {
    if (strcmp(catTypeName, m_catTypes[_t_uint8_t].name) == 0) {
      return _t_uint8_t;
    }
  }
  return -1;
}

// Search for a category name within specified cat type and return the index. -1 if not found
int8_t MoonboardUtils::catNameToNum(uint8_t z_catType, const char *catName) {
  if (z_catType >= numCatTypes) { return -1; }
  for (_t_uint8_t = 0; _t_uint8_t < m_catTypes[z_catType].catCount; _t_uint8_t++) {
    if (strcmp(catName, m_catTypes[_t_uint8_t].name) == 0) {
      return _t_uint8_t;
    }
  }
  return -1;
}

// Return the name of the category specified within specified cat type. NULL if invalid param
char *MoonboardUtils::catNumToName(uint8_t z_catType, uint8_t z_catNum) {
  if (z_catType >= numCatTypes) { return NULL; }
  if (z_catNum >= m_catTypes[z_catType].catCount) { return NULL; }
  return catNames[m_catTypes[z_catType].catStartIdx + z_catNum];
}

// Build the list name from the currently category selections
// For types where no selection is made, the wildcard string is used
void MoonboardUtils::getListNameFromCats(char *buf, uint8_t bufLen) {
  uint8_t copied = 0;
  for (uint8_t l_catTypeNum = 0; l_catTypeNum < numCatTypes; l_catTypeNum++) {
    char *l_catName = (m_catTypes[l_catTypeNum].selectedCat == -1) ? wildcardStr : getSelectedCatName(l_catTypeNum);
    uint8_t len = strlen(l_catName);
    if (len + 2 > bufLen) { // add delimiter and null-terminator
      Serial.printf("MBU::gLNFC - %s too long for buffer\n", l_catName);
      return;
    }
    strcpy(&(buf[copied]), l_catName);
    copied += len;
    if (numCatTypes - l_catTypeNum > 1) { // Skip delimiter on last one
      strcpy(&(buf[copied++]), "_");
    }
  }
}

// Finds all lists in the filesystem and populates the internal storage with them
void MoonboardUtils::findLists() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  char *listNameBufPtr = listNameBuf; // Point to unused storage for list names
  uint8_t listNum = 0; // Keep track of how many list names we've added
  while (file) {
    uint8_t l = strlen(file.name());
    if (strncmp(&(file.name()[l-4]),".dat",4) == 0) {
      if (listNameBufPtr - listNameBuf + l-4 >= sizeof(listNameBuf)) {
        Serial.println("MBU::fL - Exhausted listNameBuf");
        return;
      }
      if (l-5 > MAX_LISTNAME_SIZE) { // 5 = 1 (/ prefix) + 4 (.dat suffix)
        Serial.printf("MBU::fL - List name %s too long\n", file.name());
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

// Opens the specified list using the specified sort order
uint8_t MoonboardUtils::openList(const char *listName, const char *sortOrder) {
  sprintf(m_buf, "/%s_%s.lst", listName, sortOrder);
  m_list = SPIFFS.open(m_buf);
  if (!m_list) return 0;
  sprintf(m_buf, "/%s.dat", listName);
  m_data = SPIFFS.open(m_buf);
  if (!m_data) return 0;
  return 1;
}

void MoonboardUtils::closeList() {
  m_list.close();
  m_data.close();
}

// Read next problem from the currently open list
void MoonboardUtils::readNextProblem(Problem *prob) {
  strcpy(t_strtok, ":");
  int offset;
  m_list.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
  _t_ptr_char = strtok(m_buf, t_strtok);
  _t_ptr_char = strtok(NULL, t_strtok);
  offset = atoi(_t_ptr_char);
  m_data.seek(offset, SeekSet);
  m_data.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
  readProblem(prob, m_buf);
}

// read the string, placing data in the problem struct passed. Return true on success
bool MoonboardUtils::readProblem(Problem *prob, char *in)
{
  strcpy(t_strtok, ":"); // need room for null terminator
  // First grab the command type
  _t_ptr_char = strtok(in, t_strtok);
  if (_t_ptr_char == NULL)
  {
    return false;
  }
  prob->name = _t_ptr_char;
  _t_ptr_char = strtok(NULL, t_strtok);
  if (_t_ptr_char == NULL)
  {
    return false;
  }
  if (_t_ptr_char[0] != 'V') {
    return false;
  }
  prob->grade = atoi(&(_t_ptr_char[1]));
  if (prob->grade == 0) {
    return false;
  }
  _t_ptr_char = strtok(NULL, t_strtok);
  prob->rating = atoi(_t_ptr_char);
  if (prob->rating == 0) {
    return false;
  }
  _t_ptr_char = strtok(NULL, t_strtok);
  prob->repeats = atoi(_t_ptr_char);
  if (prob->repeats == 0) {
    return false;
  }
  _t_ptr_char = strtok(NULL, t_strtok);
  switch (_t_ptr_char[0]) {
    case 'Y': prob->isBenchmark = true; break;
    case 'N': prob->isBenchmark = false; break;
    default:
      return false;
  }
  _t_ptr_char = strtok(NULL, t_strtok);
  prob->bottomHolds = _t_ptr_char;
  _t_ptr_char = strtok(NULL, t_strtok);
  prob->middleHolds = _t_ptr_char;
  _t_ptr_char = strtok(NULL, t_strtok);
  prob->topHolds = _t_ptr_char;
  return true;
}
