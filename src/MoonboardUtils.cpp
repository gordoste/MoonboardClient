#include "MoonboardUtils.h"

// Pass a buffer for working
MoonboardUtils::MoonboardUtils(char *buf, uint16_t bufLen) {
  m_buf = buf;
  m_bufLen = bufLen;
}

void MoonboardUtils::addSortOrder(const char *sortOrderStr) {
  if (m_numSortOrders == MAX_SORT_ORDERS) { Serial.println("MBU::sSO - Hit max #"); return; }
  strcpy(m_buf, sortOrderStr);
  strcpy(t_strtok, ":");
  _t_ptr_char = strtok(m_buf, t_strtok);
  _t_uint8_t = strlen(_t_ptr_char); // +1 for null terminator
  if (_t_uint8_t > MAX_SORTORDER_NAME_LEN) { Serial.printf("MBU:sSO - '%s' too long\n", _t_ptr_char); return; }
  strcpy(m_sortOrders[m_numSortOrders].name, _t_ptr_char);
  _t_ptr_char = strtok(NULL, t_strtok);
  _t_uint8_t = strlen(_t_ptr_char); // +1 for null terminator
  if (_t_uint8_t > MAX_SORTORDER_DSPNAME_LEN) { Serial.printf("MBU:sSO - dN '%s' too long\n", _t_ptr_char); return; }
  strcpy(m_sortOrders[m_numSortOrders].displayName, _t_ptr_char);
  m_numSortOrders++;
}

void MoonboardUtils::beginCatType(char *catTypeName) {
  if (strlen(catTypeName) > MAX_CATTYPENAME_LEN) { Serial.printf("MBU::bCT - '%s' too long\n", catTypeName); return; }
  strncpy(m_catTypes[m_numCatTypes].name, catTypeName, sizeof(CategoryType::name));
  // Ensure string is terminated
  m_catTypes[m_numCatTypes].name[sizeof(CategoryType::name)-1] = '\0';
  m_catTypes[m_numCatTypes].catCount = 0;
  m_catTypes[m_numCatTypes].selectedCat = -1;
}

void MoonboardUtils::endCatType() {
  m_numCatTypes++;
}

void MoonboardUtils::addCat(const char *catName) {
  _t_uint8_t = strlen(catName) + 1; // +1 for null terminator
  if (t_catBufPtr - m_catBuf + _t_uint8_t > sizeof(m_catBuf)) {
    Serial.println("MBU::aC - Exhausted catBuf");
    return;
  }
  strcpy(t_catBufPtr, catName);
  m_catTypes[m_numCatTypes].addCat(t_catBufPtr);
  t_catBufPtr += _t_uint8_t; 
}

// Pass colon-delimited string. First token is cat type name, others are taken as category names
void MoonboardUtils::addCatType(const char *catType) {
  strcpy(m_buf, catType);
  strcpy(t_strtok, ":");
  _t_ptr_char = strtok(m_buf, t_strtok);
  beginCatType(_t_ptr_char);
  while ((_t_ptr_char = strtok(NULL, t_strtok))) {
    addCat(_t_ptr_char);
  }
  endCatType();
}

// Get category type by number. Return NULL if it doesn't exist. Good for iterating.
CategoryType *MoonboardUtils::getCatType(int8_t z_catType) {
  return (z_catType < m_numCatTypes ? &(m_catTypes[z_catType]) : NULL);
}

// Get sort order by number. Return NULL if it doesn't exist. Good for iterating.
SortOrder *MoonboardUtils::getSortOrder(int8_t z_sortOrder) {
  return (z_sortOrder < m_numSortOrders ? &(m_sortOrders[z_sortOrder]) : NULL);
}

void MoonboardUtils::selectCat_ss(const char *catTypeName, const char *catName) {
  _t_int8_t = catTypeToNum(catTypeName);
  if (_t_int8_t == -1) { Serial.printf("MBU::sC - Bad catType '%s'\n", catTypeName); }
  selectCat_is(_t_int8_t, catName);
}

void MoonboardUtils::selectCat_is(int8_t z_catType, const char *catName) {
  _t_int8_t = catNameToNum(z_catType, catName);
  if (_t_int8_t == -1) { Serial.printf("MBU::sC - Bad cat '%s'\n", catName); }
  selectCat_ii(z_catType, _t_int8_t);
}

void MoonboardUtils::selectCat_si(const char *catTypeName, int8_t z_catNum) {
  _t_int8_t = catTypeToNum(catTypeName);
  if (_t_int8_t == -1) { Serial.printf("MBU::sC - Bad catType '%s'\n", catTypeName); return; }
  selectCat_ii(_t_int8_t, z_catNum);
}

void MoonboardUtils::selectCat_ii(int8_t z_catType, int8_t z_catNum) {
  if (z_catType >= m_numCatTypes) { Serial.printf("MBU::sC - Bad catType #%d\n", z_catType); return; }
  if (z_catNum >= m_catTypes[z_catType].catCount) {
    Serial.printf("MBU::sC - Bad catNum #%d >= %d\n", z_catNum, m_catTypes[z_catType].catCount);
    return;
  }
  m_catTypes[z_catType].selectedCat = z_catNum;
  updateSortOrders();
}

void MoonboardUtils::unselectCat_s(const char *catTypeName) {
  _t_int8_t = catTypeToNum(catTypeName);
  if (_t_int8_t == -1) { Serial.printf("MBU::uC - Bad catType '%s'\n", catTypeName); return; }
  unselectCat_i(_t_int8_t);
}

void MoonboardUtils::unselectCat_i(int8_t z_catType) {
  if (z_catType >= m_numCatTypes) { Serial.printf("MBU::uC - Bad catType #%d\n", z_catType); return; }
  m_catTypes[z_catType].selectedCat = -1;
  updateSortOrders();
}

// Search for a category type with specified name and return the index. -1 if not found
int8_t MoonboardUtils::catTypeToNum(const char *catTypeName) {
  for (_t_uint8_t = 0; _t_uint8_t < m_numCatTypes; _t_uint8_t++) {
    if (strcmp(catTypeName, m_catTypes[_t_uint8_t].name) == 0) {
      return _t_uint8_t;
    }
  }
  return -1;
}

// Search for a category name within specified cat type and return the index. -1 if not found
int8_t MoonboardUtils::catNameToNum(int8_t z_catType, const char *catName) {
  if (z_catType >= m_numCatTypes) { return -1; }
  for (_t_uint8_t = 0; _t_uint8_t < m_catTypes[z_catType].catCount; _t_uint8_t++) {
    if (strcmp(catName, m_catTypes[_t_uint8_t].name) == 0) {
      return _t_uint8_t;
    }
  }
  return -1;
}

// Return the name of the category specified within specified cat type. NULL if invalid param
char *MoonboardUtils::catNumToName(int8_t z_catType, int8_t z_catNum) {
  if (z_catType >= m_numCatTypes) { return NULL; }
  if (z_catNum >= m_catTypes[z_catType].catCount) { return NULL; }
  return m_catTypes[z_catType].catNames[z_catNum];
}

// Get the name of the selected category for specified cat type. Return NULL if none selected
char *MoonboardUtils::getSelectedCatName(int8_t z_catType) {
  if (z_catType >= m_numCatTypes) { return NULL; }
  return m_catTypes[z_catType].getSelectedCat();
}

// Build the list name from the currently category selections
// For types where no selection is made, the wildcard string is used
void MoonboardUtils::getSelectedListName(char *buf, uint16_t bufLen) {
  uint8_t copied = 0;
  for (uint8_t l_catTypeNum = 0; l_catTypeNum < m_numCatTypes; l_catTypeNum++) {
    char *l_catName = m_catTypes[l_catTypeNum].getSelectedCat();
    if (l_catName == NULL) { l_catName = m_wildcardStr; }
    uint8_t len = strlen(l_catName);
    if (copied + len + 2 > bufLen) { // add delimiter and null-terminator
      Serial.printf("MBU::gSLN - %s too long\n", l_catName);
      return;
    }
    strcpy(&(buf[copied]), l_catName);
    copied += len;
    if (m_numCatTypes - l_catTypeNum > 1) { // Skip delimiter on last one
      strcpy(&(buf[copied++]), "_");
    }
  }
}

// Check which category types have an ordered index for the selected list
void MoonboardUtils::updateSortOrders() {
  m_buf[0] = '/';
  getSelectedListName(&(m_buf[1]), m_bufLen-1);
  _t_ptr_char = &(m_buf[strlen(m_buf)]);
  for (_t_uint8_t = 0; _t_uint8_t < m_numSortOrders; _t_uint8_t++) {
    sprintf(_t_ptr_char, "_%s.lst", m_sortOrders[_t_uint8_t].name);
    m_sortOrders[_t_uint8_t].exists = SPIFFS.exists(m_buf);
  }
}

// Search for a sort order with specified name and return the index. NULL if not found
SortOrder *MoonboardUtils::getSortOrderByName(const char *sortOrderName) {
  for (_t_uint8_t = 0; _t_uint8_t < m_numSortOrders; _t_uint8_t++) {
    if (strcmp(sortOrderName, m_sortOrders[_t_uint8_t].name) == 0) {
      return &(m_sortOrders[_t_uint8_t]);
    }
  }
  return NULL;
}

// Search SPIFFS for lists. Call after SPIFFS is started.
void MoonboardUtils::findLists() {
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  char *listNameBufPtr = m_listNameBuf; // Point to unused storage for list names
  uint8_t listNum = 0; // Keep track of how many list names we've added
  while (file) {
    uint8_t l = strlen(file.name());
    if (strncmp(&(file.name()[l-4]),".dat",4) == 0) {
      if (listNameBufPtr - m_listNameBuf + l-4 >= sizeof(m_listNameBuf)) {
        Serial.println("MBU::fL - Exhausted listNameBuf");
        return;
      }
      if (l-5 > MAX_LISTNAME_SIZE) { // 5 = 1 (/ prefix) + 4 (.dat suffix)
        Serial.printf("MBU::fL - List name %s too long\n", file.name());
      }
      else {
        // Copy the list name into the space reserved and record where we copied it
        m_listNames[listNum++] = strncpy(listNameBufPtr, &(file.name()[1]), l-5);
        listNameBufPtr[l-5] = '\0'; // Null terminate the string
        listNameBufPtr = &(listNameBufPtr[l-4]); // Move our pointer along to the next free space
      }
    }
    file = root.openNextFile();
  }
}

// Opens the specified list using the specified sort order
uint8_t MoonboardUtils::openList(const char *listName, const char *sortOrder) {
  if (strlen(listName) > MAX_LISTNAME_SIZE) { Serial.printf("MBU::oL - name '%s' too long\n", listName); return 0; }
  sprintf(m_buf, "/%s_%s.lst", listName, sortOrder);
  m_list = SPIFFS.open(m_buf);
  if (!m_list) return 0;
  sprintf(m_buf, "/%s.dat", listName);
  m_data = SPIFFS.open(m_buf);
  if (!m_data) return 0;
  return 1;
}

uint8_t MoonboardUtils::openSelectedList(const char *sortOrder) {
  getSelectedListName(&(m_buf[(m_bufLen-1)-(MAX_LISTNAME_SIZE+1)]), MAX_LISTNAME_SIZE+1);
  return openList(&(m_buf[(m_bufLen-1)-(MAX_LISTNAME_SIZE+1)]), sortOrder);
}

void MoonboardUtils::closeList() {
  m_list.close();
  m_data.close();
}

// Read next problem from the currently open list
bool MoonboardUtils::readNextProblem(Problem *prob) {
  strcpy(t_strtok, ":");
  int offset;
  m_list.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
  _t_ptr_char = strtok(m_buf, t_strtok);
  if (_t_ptr_char == NULL) { return false; }
  _t_ptr_char = strtok(NULL, t_strtok);
  offset = atoi(_t_ptr_char);
  m_data.seek(offset, SeekSet);
  m_data.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
  return readProblem(prob, m_buf);
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
