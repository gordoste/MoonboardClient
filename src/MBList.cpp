#include "MBList.h"

void MBList::begin(char *probBuf, uint16_t probBufLen, FS *fs, Print *stdErr) {
    m_probBuf = probBuf;
    m_probBufLen = probBufLen;
    m_stdErr = stdErr;
    m_fs = fs;
}

bool MBList::open(ListType type, const char *listName, const char *sortOrder) {
    if (!openListFile(type, listName, sortOrder)) return false;

    // Read the number of problems and store
    listFile.readStringUntil('\n').toCharArray(tmpBuf, sizeof(tmpBuf));
    long int numProbs = strtol(tmpBuf, NULL, 10);
    if (numProbs <= 0) return false;
    listSize = numProbs;
    while (numProbs > 0) { // Skip through all the problems
        listFile.readStringUntil('\n');
        numProbs--;
    }
    // Read and store the list of offsets
    pageOffsets.clear();
    listFile.readStringUntil(':').toCharArray(tmpBuf, sizeof(tmpBuf));
    while (strlen(tmpBuf) > 0) {
        long int offset = strtol(tmpBuf, NULL, 10);
        if (offset > 0) pageOffsets.push_back(offset);
        listFile.readStringUntil(':').toCharArray(tmpBuf, sizeof(tmpBuf));
    }
    // Reopen the list
    listFile.close();

    if (!openListFile(type, listName, sortOrder)) return false;
    listFile.readStringUntil('\n'); // Skip the first line (# of problems)

    if (!openDataFile(type, listName)) {
        listFile.close();
        return false;
    }
    nextProbNum = 0;
    listHasNext = fetchNextProblem();
    return true;
}

bool MBList::isOpen() { return dataFile; }

bool MBList::seekPage(uint16_t pageNum) {
    if (!dataFile) return false;
    if (pageNum >= pageOffsets.size()) return false;
    if (!listFile.seek(pageOffsets[pageNum], SeekSet)) return false;
    nextProbNum = pageNum * CONST_PAGE_SIZE;
    listHasNext = fetchNextProblem();
    return true;
}

// m_nextProbNum needs to already be set to the new value before calling
bool MBList::fetchNextProblem() {
    if (!dataFile) return false;
    if (nextProbNum == listSize) return false;
    if (readListEntryAndSeekInData() == -1) return false;
    dataFile.readStringUntil('\n').toCharArray(m_probBuf, m_probBufLen);
    if (strlen(m_probBuf) == 0) return false;
    return true;
}

void MBList::close() {
    if (listFile) listFile.close();
    if (dataFile) dataFile.close();
}

// Read next problem from the currently open list
// The problem will be pre-fetched into buffer each time
bool MBList::readNextProblem(Problem *prob) {
    if (!listHasNext) return false;                   // List exhausted
    if (!parseProblem(prob, m_probBuf)) return false; // Parse the one in cache
    nextProbNum++;
    listHasNext = fetchNextProblem(); // Fetch next one into cache
    return true;
}

uint16_t MBList::getPageNum() {
    return (nextProbNum - 1) / CONST_PAGE_SIZE;
}

// Read problems (up to max. # specified) from the open list into given array. Returns # problems read.
uint8_t MBList::readNextProblems(Problem pArr[], uint8_t max) {
    uint8_t _t_uint8_t = 0;
    while (_t_uint8_t < max) {
        if (!readNextProblem(&(pArr[_t_uint8_t]))) {
            return _t_uint8_t;
        }
        _t_uint8_t++;
    }
    return _t_uint8_t;
}

uint8_t MBList::readNextPage(Problem pArr[]) {
    return readNextProblems(pArr, CONST_PAGE_SIZE);
}

uint8_t MBList::readPrevPage(Problem pArr[]) {
    if (!seekPage(getPageNum() - 1)) return 0;
    return readNextProblems(pArr, CONST_PAGE_SIZE);
}

uint8_t MBList::readPage(Problem pArr[], uint16_t pageNum) {
    if (!seekPage(pageNum)) return 0;
    return readNextProblems(pArr, CONST_PAGE_SIZE);
}

// Return true if p1 comes before p2 in the specified sort order
bool MBList::comesBefore(const SortOrder *so, const Problem *p1, const Problem *p2) {
    if (strncmp(so->name, "name", 4) == 0) {
        return strcmp(p1->name, p2->name) >= 0;
    }
    if (strncmp(so->name, "rpts", 4) == 0) {
        return p1->repeats > p2->repeats;
    }
    return false;
}

// Add the specified problem to a custom list. Involves rewriting the list files that have problems sorted
bool MBList::addProblem(const Problem *p, const char *listName, const std::vector<SortOrder *> *sortOrders) {
    Problem tmpProblem;
    // Add the problem at the bottom of the data file. Note the offset of the problem
    if (!openDataFile(LIST_CUSTOM, listName)) return false;
    File newDataFile = m_fs->open("/__newdata", "w");
    if (!newDataFile) return false;
    long int newProbDataPos = 0;
    for (String s = dataFile.readStringUntil('\n'); s.length() > 0; s = dataFile.readStringUntil('\n')) {
        newProbDataPos += newDataFile.print(s+'\n');
    }
    writeProblem(p, newDataFile);
    newDataFile.close();
    // For each sort order ...
    for (auto soIt = sortOrders->begin(); soIt < sortOrders->end(); soIt++) {
        uint32_t listBytesWritten = 0;
        sprintf(tmpBuf, "/__newlist_%s", (*soIt)->name);
        File newListFile = m_fs->open(tmpBuf, "w");
        if (!newListFile) return false;
        //      - open the list file for that sort order
        if (!openListFile(LIST_CUSTOM, listName, (*soIt)->name)) return false;
        //      - add one to first line (# problems)
        listFile.readStringUntil('\n').toCharArray(tmpBuf, sizeof(tmpBuf));
        long int numProbs = strtol(tmpBuf, NULL, 10);
        if (numProbs <= 0) return false;
        listBytesWritten += newListFile.printf("%ld\n", numProbs + 1);
        // Skip through all the problems
        for (long int l = 0; l < numProbs; l++)
            listFile.readStringUntil('\n');
        pageOffsets.clear();
        //      - read the last line (page offsets)
        listFile.readStringUntil(':').toCharArray(tmpBuf, sizeof(tmpBuf));
        while (strlen(tmpBuf) > 0) {
            long int offset = strtol(tmpBuf, NULL, 10);
            if (offset > 0) pageOffsets.push_back(offset);
            listFile.readStringUntil(':').toCharArray(tmpBuf, sizeof(tmpBuf));
        }
        uint32_t prevPageOffset = 0;
        bool foundPage = false;
        std::vector<uint32_t> newPageOffsets;
        long int probsRead = 0;
        long int probsWritten = 0;
        // First, find the page where we need to insert it. We do this by checking the 1st entry in each page
        // to see if it comes after the new problem. If it does, the new problem belongs on the page before that.
        //      - for each page offset ...
        for (auto offsetIter = pageOffsets.begin(); offsetIter < pageOffsets.end() && !foundPage; offsetIter++) {
            listFile.seek(*offsetIter, SeekSet);
            //  - ... read the list entry and open data file at that offset
            if (readListEntryAndSeekInData() == -1) return false;
            //  - ... examine problem to see if it comes after the new problem in the list
            dataFile.readStringUntil('\n').toCharArray(tmpBuf, sizeof(tmpBuf));
            if (!parseProblem(&tmpProblem, tmpBuf)) return false;
            if (comesBefore(*soIt, p, &tmpProblem)) {
                foundPage = true;                                      // Needs to go on the previous page (or possibly as first entry of this one)
                if (prevPageOffset == 0) prevPageOffset = *offsetIter; // Handle insert before first page
            } else {
                //  - ... if not, write the prev page to the list file (unless this is the first page)
                if (prevPageOffset != 0) {
                    newPageOffsets.push_back(listBytesWritten);
                    listFile.seek(prevPageOffset, SeekSet);
                    for (uint8_t i = 0; i < PROB_PAGE_SIZE; i++) { // We know it's a complete page
                        listBytesWritten += newListFile.println(listFile.readStringUntil('\n'));
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
        listFile.seek(prevPageOffset, SeekSet);
        bool wroteProblem = false;
        int dataPos;
        // Now we just go through the remaining problems...
        while (probsRead < numProbs) {
            // Record page offset if it's the start of a new page
            if (probsWritten % PROB_PAGE_SIZE == 0) newPageOffsets.push_back(listBytesWritten);
            if (wroteProblem) {
                // We just need to keep writing the rest of the problems
                listBytesWritten += newListFile.println(listFile.readStringUntil('\n'));
                probsRead++;
                probsWritten++;
            } else {
                // If we haven't written the new problem, check to see if it's time to write it now
                dataPos = readListEntryAndSeekInData();
                if (dataPos == -1) return false;
                dataFile.readStringUntil('\n').toCharArray(tmpBuf, sizeof(tmpBuf));
                probsRead++;
                if (!parseProblem(&tmpProblem, tmpBuf)) return false;
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
        // If we still haven't written the new problem then it must come last
        if (!wroteProblem) {
            // Record page offset if it's the start of a new page
            if (probsWritten % PROB_PAGE_SIZE == 0) newPageOffsets.push_back(listBytesWritten);
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
    if (!dataFileNameToBuf(LIST_CUSTOM, listName, tmpBuf, sizeof(tmpBuf))) return false;
    m_fs->remove(tmpBuf);
    if (!m_fs->rename("/__newdata", tmpBuf)) return false;
    for (auto soIt = sortOrders->begin(); soIt < sortOrders->end(); soIt++) {
        snprintf(tmpBuf, 40, "/__newlist_%s", (*soIt)->name);
        if (!listFileNameToBuf(LIST_CUSTOM, listName, (*soIt)->name, tmpBuf+40, sizeof(tmpBuf)-40)) return false;
        m_fs->remove(tmpBuf+40);
        if (!m_fs->rename(tmpBuf, tmpBuf+40)) return false;
    }
    return true;
}

bool MBList::openListFile(ListType type, const char *listName, const char *sortOrder) {
    if (listFile) listFile.close();
    if (!listFileNameToBuf(type, listName, sortOrder, tmpBuf, sizeof(tmpBuf))) return false;
    listFile = m_fs->open(tmpBuf);
    return listFile;
}

bool MBList::listFileNameToBuf(ListType type, const char *listName, const char *sortOrder, char *buf, size_t bufLen) {
    switch (type) {
    case ListType::LIST_FILTER:
        snprintf(buf, bufLen, "/%s_%s.lst", listName, sortOrder);
        break;
    case ListType::LIST_CUSTOM:
        snprintf(buf, bufLen, "%s/%s_%s.lst", MB_PROBLIST_DIR, listName, sortOrder);
        break;
    default:
        return false;
    }
    return true;
}

bool MBList::openDataFile(ListType type, const char *listName) {
    if (dataFile) dataFile.close();
    if (!dataFileNameToBuf(type, listName, tmpBuf, sizeof(tmpBuf))) return false;
    dataFile = m_fs->open(tmpBuf);
    return dataFile;
}

bool MBList::dataFileNameToBuf(ListType type, const char *listName, char *buf, size_t bufLen) {
    switch (type) {
    case ListType::LIST_FILTER:
        snprintf(buf, bufLen, "/%s.dat", listName);
        break;
    case ListType::LIST_CUSTOM:
        snprintf(buf, bufLen, "%s/%s.dat", MB_PROBLIST_DIR, listName);
        break;
    default:
        return false;
    }
    return true;
}

int MBList::readListEntryAndSeekInData() {
    // Check list is open
    char *_t_ptr_char;
    if (!listFile) return false;
    listFile.readStringUntil('\n').toCharArray(tmpBuf, sizeof(tmpBuf));
    _t_ptr_char = StringUtils::strtoke(tmpBuf, CONST_LIST_SEPARATOR);
    if (_t_ptr_char == NULL) {
        return -1;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, CONST_LIST_SEPARATOR);
    if (_t_ptr_char == NULL) {
        return -1;
    }
    int offset = atoi(_t_ptr_char);
    if (!dataFile.seek(offset, SeekSet)) return -1;
    return offset;
}