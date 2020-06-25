#include "MoonboardUtils.h"

// Pass a buffer for working
void MoonboardUtils::begin(char *buf, uint16_t bufLen, FS *fs, Print *stdErr) {
    m_buf = buf;
    m_bufLen = bufLen;
    m_stdErr = stdErr;
    m_fs = fs;
    findCustomLists();
    m_probListMem.probParser = ([this](Problem *p, char *str) -> bool { return parseProblem(p, str); });
    m_list.begin(buf, bufLen, fs, stdErr, &m_probListMem);
}

void MoonboardUtils::setStdErr(Print *stdErr) {
    m_stdErr = stdErr;
}

// Add a sort order. Format aaa:bbb where aaa is the string used in .lst filenames, bbb is the display name
SortOrder *MoonboardUtils::addSortOrder(const char *sortOrderStr) {
    if (m_numSortOrders == MAX_SORT_ORDERS) {
        m_stdErr->println(F("MBU::sSO - Hit max #"));
        return NULL;
    }
    strcpy(m_buf, sortOrderStr);
    strcpy(t_strtok, ":");
    char *_t_ptr_char = StringUtils::strtoke(m_buf, t_strtok);
    uint8_t _t_uint8_t = strlen(_t_ptr_char); // +1 for null terminator
    if (_t_uint8_t > MAX_SORTORDER_NAME_LEN) {
        m_stdErr->printf("MBU:sSO - '%s' too long\n", _t_ptr_char);
        return NULL;
    }
    strcpy(m_sortOrders[m_numSortOrders].name, _t_ptr_char);
    _t_ptr_char = StringUtils::strtoke(NULL, t_strtok);
    _t_uint8_t = strlen(_t_ptr_char); // +1 for null terminator
    if (_t_uint8_t > MAX_SORTORDER_DSPNAME_LEN) {
        m_stdErr->printf("MBU:sSO - dN '%s' too long\n", _t_ptr_char);
        return NULL;
    }
    strcpy(m_sortOrders[m_numSortOrders].displayName, _t_ptr_char);
    m_numSortOrders++;
    return &(m_sortOrders[m_numSortOrders - 1]);
}

void MoonboardUtils::beginCatType(char *catTypeName, bool wildcardOpt) {
    if (strlen(catTypeName) > MAX_CATTYPENAME_LEN) {
        m_stdErr->printf("MBU::bCT - '%s' too long\n", catTypeName);
        return;
    }
    strncpy(m_catTypes[m_numCatTypes].name, catTypeName, sizeof(CategoryType::name));
    // Ensure string is terminated
    m_catTypes[m_numCatTypes].name[sizeof(CategoryType::name) - 1] = '\0';
    m_catTypes[m_numCatTypes].catCount = 0;
    m_catTypes[m_numCatTypes].selectedCat = -1;
    m_catTypes[m_numCatTypes].wildcardOpt = wildcardOpt;
}

CategoryType *MoonboardUtils::endCatType() {
    m_numCatTypes++;
    return &(m_catTypes[m_numCatTypes - 1]);
}

void MoonboardUtils::addCat(const char *catName) {
    uint8_t _t_uint8_t = strlen(catName) + 1; // +1 for null terminator
    if (t_catBufPtr - m_catBuf + _t_uint8_t > sizeof(m_catBuf)) {
        m_stdErr->println(F("MBU::aC - Exhausted catBuf"));
        return;
    }
    strcpy(t_catBufPtr, catName);
    m_catTypes[m_numCatTypes].addCat(t_catBufPtr);
    t_catBufPtr += _t_uint8_t;
}

// Pass colon-delimited string. First token is cat type name, others are taken as category names
CategoryType *MoonboardUtils::addCatType(const char *catType, bool wildcardOpt) {
    strcpy(m_buf, catType);
    strcpy(t_strtok, ":");
    char *_t_ptr_char = StringUtils::strtoke(m_buf, t_strtok);
    beginCatType(_t_ptr_char, wildcardOpt);
    while ((_t_ptr_char = StringUtils::strtoke(NULL, t_strtok))) {
        addCat(_t_ptr_char);
    }
    return endCatType();
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
    uint8_t _t_int8_t = catTypeToNum(catTypeName);
    if (_t_int8_t == -1) {
        m_stdErr->printf("MBU::sC - Bad catType '%s'\n", catTypeName);
        return;
    }
    selectCat_is(_t_int8_t, catName);
}

void MoonboardUtils::selectCat_is(int8_t z_catType, const char *catName) {
    uint8_t _t_int8_t = catNameToNum(z_catType, catName);
    if (_t_int8_t == -1) {
        m_stdErr->printf("MBU::sC - Bad cat '%s'\n", catName);
        return;
    }
    selectCat_ii(z_catType, _t_int8_t);
}

void MoonboardUtils::selectCat_si(const char *catTypeName, int8_t z_catNum) {
    uint8_t _t_int8_t = catTypeToNum(catTypeName);
    if (_t_int8_t == -1) {
        m_stdErr->printf("MBU::sC - Bad catType '%s'\n", catTypeName);
        return;
    }
    selectCat_ii(_t_int8_t, z_catNum);
}

void MoonboardUtils::selectCat_ii(int8_t z_catType, int8_t z_catNum) {
    if (z_catType >= m_numCatTypes) {
        m_stdErr->printf("MBU::sC - Bad catType #%d\n", z_catType);
        return;
    }
    if (z_catNum >= m_catTypes[z_catType].catCount) {
        m_stdErr->printf("MBU::sC - Bad catNum #%d >= %d\n", z_catNum, m_catTypes[z_catType].catCount);
        return;
    }
    m_catTypes[z_catType].selectedCat = z_catNum;
    updateStatus();
}

void MoonboardUtils::unselectCat_s(const char *catTypeName) {
    uint8_t _t_int8_t = catTypeToNum(catTypeName);
    if (_t_int8_t == -1) {
        m_stdErr->printf("MBU::uC - Bad catType '%s'\n", catTypeName);
        return;
    }
    unselectCat_i(_t_int8_t);
}

void MoonboardUtils::unselectCat_i(int8_t z_catType) {
    if (z_catType >= m_numCatTypes) {
        m_stdErr->printf("MBU::uC - Bad catType #%d\n", z_catType);
        return;
    }
    m_catTypes[z_catType].selectedCat = -1;
    updateStatus();
}

// Search for a category type with specified name and return the index. -1 if not found
int8_t MoonboardUtils::catTypeToNum(const char *catTypeName) {
    for (uint8_t _t_uint8_t = 0; _t_uint8_t < m_numCatTypes; _t_uint8_t++) {
        if (strcmp(catTypeName, m_catTypes[_t_uint8_t].name) == 0) {
            return _t_uint8_t;
        }
    }
    return -1;
}

// Search for a category name within specified cat type and return the index. -1 if not found
int8_t MoonboardUtils::catNameToNum(int8_t z_catType, const char *catName) {
    if (z_catType >= m_numCatTypes) {
        return -1;
    }
    for (uint8_t _t_uint8_t = 0; _t_uint8_t < m_catTypes[z_catType].catCount; _t_uint8_t++) {
        if (strcmp(catName, m_catTypes[_t_uint8_t].name) == 0) {
            return _t_uint8_t;
        }
    }
    return -1;
}

// Return the name of the category specified within specified cat type. NULL if invalid param
char *MoonboardUtils::catNumToName(int8_t z_catType, int8_t z_catNum) {
    if (z_catType >= m_numCatTypes) {
        return NULL;
    }
    if (z_catNum >= m_catTypes[z_catType].catCount) {
        return NULL;
    }
    return m_catTypes[z_catType].catNames[z_catNum];
}

// Get the name of the selected category for specified cat type. Return NULL if none selected
char *MoonboardUtils::getSelectedCatName(int8_t z_catType) {
    if (z_catType >= m_numCatTypes) {
        return NULL;
    }
    return m_catTypes[z_catType].getSelectedCat();
}

const char *MoonboardUtils::getSelectedFilteredListName() {
    return m_selectedFiltListName;
}

void MoonboardUtils::updateStatus() {
    // Build the list name from the currently category selections
    // For types where no selection is made, the wildcard string is used
    uint8_t copied = 0;
    for (uint8_t l_catTypeNum = 0; l_catTypeNum < m_numCatTypes; l_catTypeNum++) {
        const char *l_catName = m_catTypes[l_catTypeNum].getSelectedCat();
        if (l_catName == NULL) {
            l_catName = m_wildcardStr;
        }
        uint8_t len = strlen(l_catName);
        if (copied + len + 1 > MAX_LISTNAME_SIZE) { // add delimiter
            m_stdErr->println(F("MBU::uS - Too long"));
            return;
        }
        strcpy(&(m_selectedFiltListName[copied]), l_catName);
        copied += len;
        if (m_numCatTypes - l_catTypeNum > 1) { // Skip delimiter on last one
            strcpy(&(m_selectedFiltListName[copied++]), "_");
        }
    }

    // Check which category types have an ordered index for the selected list
    m_buf[0] = '/';
    strcpy(&(m_buf[1]), m_selectedFiltListName);
    char *_t_ptr_char = &(m_buf[strlen(m_buf)]); // points to the null terminator after /filename
    strcpy(_t_ptr_char, ".dat");                 // Check if the problem data file (/filename.dat) exists
    m_selectedFiltListExists = m_fs->exists(m_buf);
    // iterate sort orders
    for (uint8_t _t_uint8_t = 0; _t_uint8_t < m_numSortOrders; _t_uint8_t++) {
        // check whether an index file (/filename_sortname.lst) exists for each
        sprintf(_t_ptr_char, "_%s.lst", m_sortOrders[_t_uint8_t].name);
        m_sortOrders[_t_uint8_t].exists = m_fs->exists(m_buf);
    }
}

// Search for a sort order with specified name and return the index. NULL if not found
SortOrder *MoonboardUtils::getSortOrderByName(const char *sortOrderName) {
    for (uint8_t _t_uint8_t = 0; _t_uint8_t < m_numSortOrders; _t_uint8_t++) {
        if (strcmp(sortOrderName, m_sortOrders[_t_uint8_t].name) == 0) {
            return &(m_sortOrders[_t_uint8_t]);
        }
    }
    return NULL;
}

// Opens the specified list using the specified sort order (NULL for sortOrder means order will be as it is read from the file)
bool MoonboardUtils::openFilteredList(const char *listName, const char *sortOrder) {
    if (m_probList != NULL) {
        m_probList->close();
        m_probList = NULL;
    }
    if (sortOrder == NULL || listName == NULL) return false;
    if (!m_list.open(ListType::LIST_FILTER, listName, sortOrder)) return false;
    m_probList = &m_list;
    return true;
}

// Opens the list corresponding to the selections made (or not) using the specified sort order
bool MoonboardUtils::openSelectedFilteredList(const char *sortOrder) {
    return openFilteredList(m_selectedFiltListName, sortOrder);
}

// read the string, placing data in the problem struct passed. Return true on success
bool MoonboardUtils::parseProblem(Problem *prob, char *in) {
    strcpy(t_strtok, "|"); // need room for null terminator
    char *_t_ptr_char = StringUtils::strtoke(in, t_strtok);
    if (_t_ptr_char == NULL) {
        return false;
    }
    if (strlen(_t_ptr_char) > MAX_PROBLEMNAME_LEN) {
        m_stdErr->printf("MBU::pP - n '%s' too long (%d)\n", _t_ptr_char, strlen(_t_ptr_char));
        return false;
    }
    strcpy(prob->name, _t_ptr_char);
    _t_ptr_char = StringUtils::strtoke(NULL, t_strtok);
    if (_t_ptr_char == NULL) {
        m_stdErr->printf("MBU::pP - '%s' no grade\n", prob->name);
        return false;
    }
    if (_t_ptr_char[0] != 'V') {
        m_stdErr->printf("MBU::pP - '%s' no V\n", prob->name);
        return false;
    }
    prob->grade = atoi(&(_t_ptr_char[1]));
    if (prob->grade == 0) {
        m_stdErr->printf("MBU::pP - '%s' bad grade\n", prob->name);
        return false;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, t_strtok);
    if (_t_ptr_char == NULL) {
        m_stdErr->printf("MBU::pP - '%s' no rating\n", prob->name);
        return false;
    }
    prob->rating = atoi(_t_ptr_char);
    if (prob->rating == 0) {
        m_stdErr->printf("MBU::pP - '%s' bad rating\n", prob->name);
        return false;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, t_strtok);
    if (_t_ptr_char == NULL) {
        m_stdErr->printf("MBU::pP - '%s' no repeats\n", prob->name);
        return false;
    }
    prob->repeats = atoi(_t_ptr_char);
    if (prob->repeats == 0 && (_t_ptr_char[0] != '0' || _t_ptr_char[1] != '\0')) {
        m_stdErr->printf("MBU::pP - '%s' NaN repeats\n", prob->name);
        return false;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, t_strtok);
    if (_t_ptr_char == NULL) {
        m_stdErr->printf("MBU::pP - '%s' no BM\n", prob->name);
        return false;
    }
    switch (_t_ptr_char[0]) {
    case 'Y':
        prob->isBenchmark = true;
        break;
    case 'N':
        prob->isBenchmark = false;
        break;
    default:
        return false;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, t_strtok);
    if (_t_ptr_char == NULL) {
        m_stdErr->printf("MBU::pP - '%s' no BH\n", prob->name);
        return false;
    }
    if (strlen(_t_ptr_char) > MAX_HOLDS_PER_PANEL * 3) {
        m_stdErr->printf("MBU::pP - bH '%s' too long\n", _t_ptr_char);
        return false;
    }
    strcpy(prob->bottomHolds, _t_ptr_char);
    _t_ptr_char = StringUtils::strtoke(NULL, t_strtok);
    if (_t_ptr_char == NULL) {
        m_stdErr->printf("MBU::pP - '%s' no MH\n", prob->name);
        return false;
    }
    if (strlen(_t_ptr_char) > MAX_HOLDS_PER_PANEL * 3) {
        m_stdErr->printf("MBU::pP - mH '%s' too long\n", _t_ptr_char);
        return false;
    }
    strcpy(prob->middleHolds, _t_ptr_char);
    _t_ptr_char = StringUtils::strtoke(NULL, t_strtok);
    if (_t_ptr_char == NULL) {
        m_stdErr->printf("MBU::pP - '%s' no TH\n", prob->name);
        return false;
    }
    if (strlen(_t_ptr_char) > MAX_HOLDS_PER_PANEL * 3) {
        m_stdErr->printf("MBU::pP - tH '%s' too long\n", _t_ptr_char);
        return false;
    }
    strcpy(prob->topHolds, _t_ptr_char);
    return true;
}

void MoonboardUtils::printProblem(Problem *p, Print *out) {
    strcpy(m_buf, "%XXs %s V%d %5d");
    char t_num[3];
    snprintf(t_num, 3, "%d", MAX_PROBLEMNAME_LEN);
    strncpy(&(m_buf[1]), t_num, 2);
    out->printf(m_buf, p->name, p->isBenchmark ? "(B)" : "   ", p->grade, p->repeats);
    switch (p->rating) {
    case 0:
        out->println("   ");
        break;
    case 1:
        out->println("*  ");
        break;
    case 2:
        out->println("** ");
        break;
    case 3:
        out->println("***");
        break;
    default:
        m_stdErr->printf("MBU::pTS - %s Weird rating %d ", p->name, p->rating);
        out->println("???");
    }
}

void MoonboardUtils::showCatType(Print *outStr, CategoryType *ptrCT) {
    outStr->printf("%s: ", ptrCT->name);
    for (uint8_t cat_i = 0; cat_i < ptrCT->catCount; cat_i++) {
        if (cat_i == ptrCT->selectedCat) {
            outStr->print('[');
        }
        outStr->print(ptrCT->catNames[cat_i]);
        if (cat_i == ptrCT->selectedCat) {
            outStr->print(']');
        }
        outStr->print('/');
    }
    outStr->println(ptrCT->selectedCat == -1 ? "[*]" : "*");
}

void MoonboardUtils::showAllCatTypes(Print *outStr) {
    CategoryType *ptrCT;
    uint8_t _t_uint8_t = 0;
    while (ptrCT = getCatType(_t_uint8_t++)) {
        showCatType(outStr, ptrCT);
    }
}

// void MoonboardUtils::showStatus(Print *outStr) {
//     switch (m_listType) {
//     default:
//         break;
//     case NONE:
//         outStr->println("No problem list selected currently.");
//         break;
//     case CUSTOM:
//         outStr->printf("Custom list '%s' selected.\n", m_customListNames[m_selectedCustomList]);
//         break;
//     case FILTERED:
//         outStr->printf("Filter name '%s' selected. Available sort orders: ", m_selectedFiltListName);
//         SortOrder *ptrSO;
//         _t_uint8_t = 0;
//         bool t_started = false;
//         while (ptrSO = getSortOrder(_t_uint8_t++)) {
//             if (ptrSO->exists) {
//                 outStr->printf((t_started ? ", %s" : "%s"), ptrSO->displayName);
//                 t_started = true;
//             }
//         }
//         outStr->println();
//         break;
//     }
// }

uint8_t MoonboardUtils::getNumCatTypes() { return m_numCatTypes; }
uint8_t MoonboardUtils::getNumSortOrders() { return m_numSortOrders; }
uint8_t MoonboardUtils::getNumCustomLists() { return m_numCustomLists; }

uint8_t MoonboardUtils::findCustomLists() {
    m_numCustomLists = 0;
    const char *fnam;
    uint8_t listDirSz = strlen(MB_PROBLIST_DIR);
    if (File listDir = m_fs->open(MB_PROBLIST_DIR)) {
        // If we can open the list directory, must be using SD card
        if (listDir.isDirectory()) {
            for (File lFile = listDir.openNextFile(); lFile; lFile = listDir.openNextFile()) {
                fnam = lFile.name();
                checkFileIsCustomList(fnam);
                lFile.close();
            }
            m_stdErr->printf("Found %i custom lists\n", m_numCustomLists);
            return m_numCustomLists;
        }
    }
    // Otherwise fall back to searching the whole filesystem
    File root = m_fs->open("/");
    if (!root) {
        m_stdErr->println(F("Can't open FS. Is it initialised?"));
        return 0;
    }
    File f;
    m_stdErr->println("Looking for custom lists");
    while ((f = root.openNextFile()) && (m_numCustomLists < MAX_CUSTOM_LISTS)) {
        fnam = f.name();
        m_stdErr->print('.');
        if (strncmp(MB_PROBLIST_DIR, fnam, listDirSz) == 0) {
            // SPIFFS has no directories, it treats dirname as part of filename. Look for files with /PROBDIR/ at the start
            if (fnam[listDirSz] == '/') {
                checkFileIsCustomList(fnam);
            }
        }
        f.close();
    }
    root.close();
    m_stdErr->printf("Found %i custom lists\n", m_numCustomLists);
    return m_numCustomLists;
}

void MoonboardUtils::checkFileIsCustomList(const char *fileName) {
    uint8_t _t_uint8_t = strlen(fileName);
    if (strncmp(&(fileName[_t_uint8_t - 4]), ".dat", 4) == 0) {
        strncpy(m_customListNames[m_numCustomLists], &(fileName[m_listDirSz]), _t_uint8_t - 4 - m_listDirSz);
        m_customListNames[m_numCustomLists][_t_uint8_t] = '\0';
        m_stdErr->printf("Found custom list %s\n", m_customListNames[m_numCustomLists]);
        m_numCustomLists++;
    }
}

bool MoonboardUtils::openCustomList(uint8_t z_listNum) {
    if (m_probList != NULL) {
        m_probList->close();
        m_probList = NULL;
    }
    if (z_listNum >= m_numCustomLists) return false;
    if (m_list.open(ListType::LIST_CUSTOM, m_customListNames[z_listNum], "name")) {
        m_selectedCustomList = z_listNum;
        m_probList = &m_list;
        return true;
    }
    return false;
}

const char *MoonboardUtils::customListNumToName(uint8_t z_listNum) {
    return z_listNum < m_numCustomLists ? m_customListNames[z_listNum] : NULL;
}

const char *MoonboardUtils::getSelectedCustomListName() {
    return customListNumToName(m_selectedCustomList);
}