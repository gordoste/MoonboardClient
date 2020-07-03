#include "MBUtils.h"

// Pass a buffer for working
void MBUtils::begin(char *tmpBuf, uint16_t tmpBufLen, FS *fs, Print *stdErr) {
    m_tmpBuf = tmpBuf;
    m_tmpBufLen = tmpBufLen;
    m_stdErr = stdErr;
    m_fs = fs;
    findCustomLists();
    m_list.begin(tmpBuf, tmpBufLen, fs, stdErr);
}

void MBUtils::setStdErr(Print *stdErr) {
    m_stdErr = stdErr;
}

// Add a sort order. Format aaa:bbb where aaa is the string used in .lst filenames, bbb is the display name
SortOrder *MBUtils::addSortOrder(const char *sortOrderStr) {
    if (m_numSortOrders == MAX_SORT_ORDERS) {
        m_stdErr->println(F("MBU::sSO - Hit max #"));
        return NULL;
    }
    strcpy(m_tmpBuf, sortOrderStr);
    strcpy(t_strtok, ":");
    char *_t_ptr_char = StringUtils::strtoke(m_tmpBuf, t_strtok);
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

void MBUtils::beginCatType(char *catTypeName, bool wildcardOpt) {
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

CategoryType *MBUtils::endCatType() {
    m_numCatTypes++;
    return &(m_catTypes[m_numCatTypes - 1]);
}

void MBUtils::addCat(const char *catName) {
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
CategoryType *MBUtils::addCatType(const char *catType, bool wildcardOpt) {
    strcpy(m_tmpBuf, catType);
    strcpy(t_strtok, ":");
    char *_t_ptr_char = StringUtils::strtoke(m_tmpBuf, t_strtok);
    beginCatType(_t_ptr_char, wildcardOpt);
    while ((_t_ptr_char = StringUtils::strtoke(NULL, t_strtok))) {
        addCat(_t_ptr_char);
    }
    return endCatType();
}

// Get category type by number. Return NULL if it doesn't exist. Good for iterating.
CategoryType *MBUtils::getCatType(int8_t z_catType) {
    return (z_catType < m_numCatTypes ? &(m_catTypes[z_catType]) : NULL);
}

// Get sort order by number. Return NULL if it doesn't exist. Good for iterating.
SortOrder *MBUtils::getSortOrder(int8_t z_sortOrder) {
    return (z_sortOrder < m_numSortOrders ? &(m_sortOrders[z_sortOrder]) : NULL);
}

void MBUtils::selectCat_ss(const char *catTypeName, const char *catName) {
    uint8_t _t_int8_t = catTypeToNum(catTypeName);
    if (_t_int8_t == -1) {
        m_stdErr->printf("MBU::sC - Bad catType '%s'\n", catTypeName);
        return;
    }
    selectCat_is(_t_int8_t, catName);
}

void MBUtils::selectCat_is(int8_t z_catType, const char *catName) {
    uint8_t _t_int8_t = catNameToNum(z_catType, catName);
    if (_t_int8_t == -1) {
        m_stdErr->printf("MBU::sC - Bad cat '%s'\n", catName);
        return;
    }
    selectCat_ii(z_catType, _t_int8_t);
}

void MBUtils::selectCat_si(const char *catTypeName, int8_t z_catNum) {
    uint8_t _t_int8_t = catTypeToNum(catTypeName);
    if (_t_int8_t == -1) {
        m_stdErr->printf("MBU::sC - Bad catType '%s'\n", catTypeName);
        return;
    }
    selectCat_ii(_t_int8_t, z_catNum);
}

void MBUtils::selectCat_ii(int8_t z_catType, int8_t z_catNum) {
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

void MBUtils::unselectCat_s(const char *catTypeName) {
    uint8_t _t_int8_t = catTypeToNum(catTypeName);
    if (_t_int8_t == -1) {
        m_stdErr->printf("MBU::uC - Bad catType '%s'\n", catTypeName);
        return;
    }
    unselectCat_i(_t_int8_t);
}

void MBUtils::unselectCat_i(int8_t z_catType) {
    if (z_catType >= m_numCatTypes) {
        m_stdErr->printf("MBU::uC - Bad catType #%d\n", z_catType);
        return;
    }
    m_catTypes[z_catType].selectedCat = -1;
    updateStatus();
}

// Search for a category type with specified name and return the index. -1 if not found
int8_t MBUtils::catTypeToNum(const char *catTypeName) {
    for (uint8_t _t_uint8_t = 0; _t_uint8_t < m_numCatTypes; _t_uint8_t++) {
        if (strcmp(catTypeName, m_catTypes[_t_uint8_t].name) == 0) {
            return _t_uint8_t;
        }
    }
    return -1;
}

// Search for a category name within specified cat type and return the index. -1 if not found
int8_t MBUtils::catNameToNum(int8_t z_catType, const char *catName) {
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
char *MBUtils::catNumToName(int8_t z_catType, int8_t z_catNum) {
    if (z_catType >= m_numCatTypes) {
        return NULL;
    }
    if (z_catNum >= m_catTypes[z_catType].catCount) {
        return NULL;
    }
    return m_catTypes[z_catType].catNames[z_catNum];
}

// Get the name of the selected category for specified cat type. Return NULL if none selected
char *MBUtils::getSelectedCatName(int8_t z_catType) {
    if (z_catType >= m_numCatTypes) {
        return NULL;
    }
    return m_catTypes[z_catType].getSelectedCat();
}

const char *MBUtils::getSelectedFilteredListName() {
    return m_selectedFiltListName;
}

void MBUtils::updateStatus() {
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
    m_tmpBuf[0] = '/';
    strcpy(&(m_tmpBuf[1]), m_selectedFiltListName);
    char *_t_ptr_char = &(m_tmpBuf[strlen(m_tmpBuf)]); // points to the null terminator after /filename
    strcpy(_t_ptr_char, ".dat");                       // Check if the problem data file (/filename.dat) exists
    m_selectedFiltListExists = m_fs->exists(m_tmpBuf);
    // iterate sort orders
    for (uint8_t _t_uint8_t = 0; _t_uint8_t < m_numSortOrders; _t_uint8_t++) {
        // check whether an index file (/filename_sortname.lst) exists for each
        sprintf(_t_ptr_char, "_%s.lst", m_sortOrders[_t_uint8_t].name);
        m_sortOrders[_t_uint8_t].exists = m_fs->exists(m_tmpBuf);
    }
}

// Search for a sort order with specified name and return the index. NULL if not found
SortOrder *MBUtils::getSortOrderByName(const char *sortOrderName) {
    for (uint8_t _t_uint8_t = 0; _t_uint8_t < m_numSortOrders; _t_uint8_t++) {
        if (strcmp(sortOrderName, m_sortOrders[_t_uint8_t].name) == 0) {
            return &(m_sortOrders[_t_uint8_t]);
        }
    }
    return NULL;
}

// Opens the specified list using the specified sort order (NULL for sortOrder means order will be as it is read from the file)
bool MBUtils::openFilteredList(const char *listName, const char *sortOrder) {
    if (m_list.isOpen()) m_list.close();
    if (sortOrder == NULL || listName == NULL) return false;
    if (!m_list.open(ListType::LIST_FILTER, listName, sortOrder)) return false;
    return true;
}

// Opens the list corresponding to the selections made (or not) using the specified sort order
bool MBUtils::openSelectedFilteredList(const char *sortOrder) {
    return openFilteredList(m_selectedFiltListName, sortOrder);
}

void MBUtils::showCatType(Print *outStr, CategoryType *ptrCT) {
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

void MBUtils::showAllCatTypes(Print *outStr) {
    CategoryType *ptrCT;
    uint8_t _t_uint8_t = 0;
    while (ptrCT = getCatType(_t_uint8_t++)) {
        showCatType(outStr, ptrCT);
    }
}

// void MBUtils::showStatus(Print *outStr) {
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

uint8_t MBUtils::getNumCatTypes() { return m_numCatTypes; }
uint8_t MBUtils::getNumSortOrders() { return m_numSortOrders; }
uint8_t MBUtils::getNumCustomLists() { return m_numCustomLists; }

uint8_t MBUtils::findCustomLists() {
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

void MBUtils::checkFileIsCustomList(const char *fileName) {
    uint8_t _t_uint8_t = strlen(fileName);
    if (strncmp(&(fileName[_t_uint8_t - 4]), ".dat", 4) == 0) {
        strncpy(m_customListNames[m_numCustomLists], &(fileName[m_listDirSz]), _t_uint8_t - 4 - m_listDirSz);
        m_customListNames[m_numCustomLists][_t_uint8_t] = '\0';
        m_stdErr->printf("Found custom list %s\n", m_customListNames[m_numCustomLists]);
        m_numCustomLists++;
    }
}

bool MBUtils::openCustomList(uint8_t z_listNum) {
    if (m_list.isOpen()) m_list.close();
    if (z_listNum >= m_numCustomLists) return false;
    if (m_list.open(ListType::LIST_CUSTOM, m_customListNames[z_listNum], "name")) {
        m_selectedCustomList = z_listNum;
        return true;
    }
    return false;
}

const char *MBUtils::customListNumToName(uint8_t z_listNum) {
    return z_listNum < m_numCustomLists ? m_customListNames[z_listNum] : NULL;
}

const char *MBUtils::getSelectedCustomListName() {
    return customListNumToName(m_selectedCustomList);
}

// Add the specified problem to a custom list. Involves rewriting the list files that have problems sorted
bool MBUtils::addProblem(const Problem *p, const char *listName, const std::vector<SortOrder *> *sortOrders) {
    Problem tmpProblem;
    // Add the problem at the bottom of the data file. Note the offset of the problem
    if (!MBData::dataFileNameToBuf(LIST_CUSTOM, listName, m_tmpBuf, m_tmpBufLen)) return false;
    File oldDataFile = m_fs->open(m_tmpBuf);
    if (!oldDataFile) return false;
    File newDataFile = m_fs->open("/__newdata", "w");
    if (!newDataFile) return false;
    long int newProbDataPos = 0;
    for (String s = oldDataFile.readStringUntil('\n'); s.length() > 0; s = oldDataFile.readStringUntil('\n')) {
        newProbDataPos += newDataFile.print(s + '\n');
    }
    writeProblem(p, newDataFile);
    newDataFile.close();
    std::vector<uint32_t> oldPageOffsets = std::vector<uint32_t>();
    // For each sort order ...
    for (auto soIt = sortOrders->begin(); soIt < sortOrders->end(); soIt++) {
        uint32_t listBytesWritten = 0;
        sprintf(m_tmpBuf, "/__newlist_%s", (*soIt)->name);
        File newListFile = m_fs->open(m_tmpBuf, "w");
        if (!newListFile) return false;
        //      - open the list file for that sort order
        if (!MBData::listFileNameToBuf(LIST_CUSTOM, listName, (*soIt)->name, m_tmpBuf, m_tmpBufLen)) return false;
        File oldListFile = m_fs->open(m_tmpBuf);
        if (!oldListFile) return false;
        //      - add one to first line (# problems)
        oldListFile.readStringUntil('\n').toCharArray(m_tmpBuf, m_tmpBufLen);
        long int numProbs = strtol(m_tmpBuf, NULL, 10);
        if (numProbs <= 0) return false;
        listBytesWritten += newListFile.printf("%ld\n", numProbs + 1);
        // Skip through all the problems
        for (long int l = 0; l < numProbs; l++)
            oldListFile.readStringUntil('\n');
        oldPageOffsets.clear();
        //      - read the last line (page offsets)
        oldListFile.readStringUntil(':').toCharArray(m_tmpBuf, m_tmpBufLen);
        while (strlen(m_tmpBuf) > 0) {
            long int offset = strtol(m_tmpBuf, NULL, 10);
            if (offset > 0) oldPageOffsets.push_back(offset);
            oldListFile.readStringUntil(':').toCharArray(m_tmpBuf, m_tmpBufLen);
        }
        uint32_t prevPageOffset = 0;
        bool foundPage = false;
        std::vector<uint32_t> newPageOffsets;
        long int probsRead = 0;
        long int probsWritten = 0;
        // First, find the page where we need to insert it. We do this by checking the 1st entry in each page
        // to see if it comes after the new problem. If it does, the new problem belongs on the page before that.
        //      - for each page offset ...
        for (auto offsetIter = oldPageOffsets.begin(); offsetIter < oldPageOffsets.end() && !foundPage; offsetIter++) {
            oldListFile.seek(*offsetIter, SeekSet);
            //  - ... read the list entry and open data file at that offset
            if (MBData::readListEntryAndSeekInData(oldListFile, oldDataFile, m_tmpBuf, m_tmpBufLen) == -1) return false;
            //  - ... examine problem to see if it comes after the new problem in the list
            oldDataFile.readStringUntil('\n').toCharArray(m_tmpBuf, m_tmpBufLen);
            if (!parseProblem(&tmpProblem, m_tmpBuf)) return false;
            if (comesBefore(*soIt, p, &tmpProblem)) {
                foundPage = true;                                      // Needs to go on the previous page (or possibly as first entry of this one)
                if (prevPageOffset == 0) prevPageOffset = *offsetIter; // Handle insert before first page
            } else {
                //  - ... if not, write the prev page to the list file (unless this is the first page)
                if (prevPageOffset != 0) {
                    newPageOffsets.push_back(listBytesWritten);
                    oldListFile.seek(prevPageOffset, SeekSet);
                    for (uint8_t i = 0; i < CONST_PAGE_SIZE; i++) { // We know it's a complete page
                        listBytesWritten += newListFile.println(oldListFile.readStringUntil('\n'));
                        probsRead++;
                        probsWritten++;
                    }
                }
                prevPageOffset = *offsetIter;
            }
        }
        // If we never find a page, new prob must go on the last page... which is the one we have in prevPageOffset
        if (!foundPage) foundPage = true;
        // Go to start of page which will have the new problem
        oldListFile.seek(prevPageOffset, SeekSet);
        bool wroteProblem = false;
        int dataPos;
        // Now we just go through the remaining problems...
        while (probsRead < numProbs) {
            // Record page offset if it's the start of a new page
            if (probsWritten % CONST_PAGE_SIZE == 0) newPageOffsets.push_back(listBytesWritten);
            if (wroteProblem) {
                // We just need to keep writing the rest of the problems
                listBytesWritten += newListFile.println(oldListFile.readStringUntil('\n'));
                probsRead++;
                probsWritten++;
            } else {
                // If we haven't written the new problem, check to see if it's time to write it now
                dataPos = MBData::readListEntryAndSeekInData(oldListFile, oldDataFile, m_tmpBuf, m_tmpBufLen);
                if (dataPos == -1) return false;
                oldDataFile.readStringUntil('\n').toCharArray(m_tmpBuf, m_tmpBufLen);
                probsRead++;
                if (!parseProblem(&tmpProblem, m_tmpBuf)) return false;
                // If the problem is already in this list then pretend it has been added
                if (strcmp(tmpProblem.name, p->name) == 0) {
                    wroteProblem = true;
                } else {
                    //      - ... check to see if we've gone past the new problem's pos in the sorted list
                    if (comesBefore(*soIt, p, &tmpProblem)) {
                        // Time to write our problem
                        listBytesWritten += newListFile.printf("-:%ld\n", newProbDataPos);
                        probsWritten++;
                        wroteProblem = true;
                    }
                    listBytesWritten += newListFile.printf("-:%d\n", dataPos);
                    probsWritten++;
                }
            }
        }
        // If we still haven't written the new problem then it must come last
        if (!wroteProblem) {
            // Record page offset if it's the start of a new page
            if (probsWritten % CONST_PAGE_SIZE == 0) newPageOffsets.push_back(listBytesWritten);
            listBytesWritten += newListFile.printf("-:%ld\n", newProbDataPos);
            probsWritten++;
            wroteProblem = true;
        }

        listBytesWritten += newListFile.printf("%d", newPageOffsets[0]);
        for (uint8_t i = 1; i < newPageOffsets.size(); i++) {
            listBytesWritten += newListFile.printf(":%d", newPageOffsets[i]);
        }
        listBytesWritten += newListFile.println();
        newListFile.close();
    }
    if (!MBData::dataFileNameToBuf(LIST_CUSTOM, listName, m_tmpBuf, m_tmpBufLen)) return false;
    m_fs->remove(m_tmpBuf);
    if (!m_fs->rename("/__newdata", m_tmpBuf)) return false;
    for (auto soIt = sortOrders->begin(); soIt < sortOrders->end(); soIt++) {
        snprintf(m_tmpBuf, 40, "/__newlist_%s", (*soIt)->name);
        if (!MBData::listFileNameToBuf(LIST_CUSTOM, listName, (*soIt)->name, m_tmpBuf+40, m_tmpBufLen-40)) return false;
        m_fs->remove(m_tmpBuf+40);
        if (!m_fs->rename(m_tmpBuf, m_tmpBuf+40)) return false;
    }
    return true;
}

// Remove the specified problem from a custom list. Involves rewriting the list files that have problems sorted
bool MBUtils::deleteProblem(const Problem *p, const char *listName, const std::vector<SortOrder *> *sortOrders) {
    return true;
}

// Delete a problem from the currently open list, which must be a custom list.
bool MBUtils::deleteProblemFromOpenList(const Problem *p, const std::vector<SortOrder *> *sortOrders) {
    if (!m_list.isOpen()) return false;
    if (m_list.getType() != LIST_CUSTOM) return false;
    deleteProblem(p, m_list.getName(), sortOrders);
}