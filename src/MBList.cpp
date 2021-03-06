#include "MBList.h"

void MBList::begin(char *tmpBuf, uint16_t tmpBufLen, FS *fs, Print *stdErr) {
    m_tmpBuf = tmpBuf;
    m_tmpBufLen = tmpBufLen;
    m_stdErr = stdErr;
    m_fs = fs;
}

bool MBList::open(ListType type, const char *listName, const SortOrder *sortOrder) {
    if (!openListFile(type, listName, sortOrder)) return false;

    // Read the number of problems and store
    m_listFile.readStringUntil('\n').toCharArray(m_tmpBuf, m_tmpBufLen);
    m_listSize = strtol(m_tmpBuf, NULL, 10);
    if (m_listSize <= 0) return false;
    // Skip through all the problems
    for (long int l = m_listSize; l > 0; l--)
        m_listFile.readStringUntil('\n');
    // Read and store the list of offsets
    pageOffsets.clear();
    m_listFile.readStringUntil(':').toCharArray(m_tmpBuf, m_tmpBufLen);
    while (strlen(m_tmpBuf) > 0) {
        long int offset = strtol(m_tmpBuf, NULL, 10);
        if (offset > 0) pageOffsets.push_back(offset);
        m_listFile.readStringUntil(':').toCharArray(m_tmpBuf, m_tmpBufLen);
    }
    // Reopen the list
    m_listFile.close();

    if (!openListFile(type, listName, sortOrder)) return false;
    m_listFile.readStringUntil('\n'); // Skip the first line (# of problems)

    if (!openDataFile(type, listName)) {
        m_listFile.close();
        return false;
    }
    m_nextProbNum = 0;
    m_listHasNext = fetchNextProblem();
    m_listType = type;
    if (listName != m_listName) {
        strncpy(m_listName, listName, MAX_LISTNAME_SIZE);
        m_listName[MAX_LISTNAME_SIZE] = '\0';
    }
    m_sortOrder = sortOrder;
    return true;
}

bool MBList::isOpen() { return m_dataFile; }

bool MBList::seekPage(uint16_t pageNum) {
    if (!m_dataFile) return false;
    if (pageNum >= pageOffsets.size()) return false;
    if (!m_listFile.seek(pageOffsets[pageNum], SeekSet)) return false;
    m_nextProbNum = pageNum * CONST_PAGE_SIZE;
    m_listHasNext = fetchNextProblem();
    return true;
}

// m_nextProbNum needs to already be set to the new value before calling
bool MBList::fetchNextProblem() {
    if (!m_dataFile) return false;
    if (m_nextProbNum == m_listSize) return false;
    if (MBData::readListEntryAndSeekInData(m_listFile, m_dataFile, m_tmpBuf, m_tmpBufLen) == -1) return false;
    m_dataFile.readStringUntil('\n').toCharArray(m_probBuf, sizeof(m_probBuf));
    if (strlen(m_probBuf) == 0) return false;
    return true;
}

void MBList::close() {
    if (m_listFile) m_listFile.close();
    if (m_dataFile) m_dataFile.close();
    m_listSize = 0;
}

// Read next problem from the currently open list
// The problem will be pre-fetched into buffer each time
bool MBList::readNextProblem(Problem *prob) {
    if (!m_listHasNext) return false;                 // List exhausted
    if (!parseProblem(prob, m_probBuf)) return false; // Parse the one in cache
    m_nextProbNum++;
    m_listHasNext = fetchNextProblem(); // Fetch next one into cache
    return true;
}

uint16_t MBList::getPageNum() {
    return (m_nextProbNum - 1) / CONST_PAGE_SIZE;
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

bool MBList::openListFile(ListType type, const char *listName, const SortOrder *sortOrder) {
    if (m_listFile) m_listFile.close();
    if (!MBData::listFileNameToBuf(type, listName, sortOrder, m_tmpBuf, m_tmpBufLen)) return false;
    m_listFile = m_fs->open(m_tmpBuf);
    return m_listFile;
}

bool MBList::openDataFile(ListType type, const char *listName) {
    if (m_dataFile) m_dataFile.close();
    if (!MBData::dataFileNameToBuf(type, listName, m_tmpBuf, m_tmpBufLen)) return false;
    m_dataFile = m_fs->open(m_tmpBuf);
    return m_dataFile;
}
