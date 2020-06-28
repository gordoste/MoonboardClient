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

bool MBList::openListFile(ListType type, const char *listName, const char *sortOrder) {
    if (listFile) listFile.close();
    if (!listFileNameToBuf(type, listName, sortOrder, tmpBuf, sizeof(tmpBuf))) return false;
    listFile = m_fs->open(tmpBuf);
    return listFile;
}


bool MBList::openDataFile(ListType type, const char *listName) {
    if (dataFile) dataFile.close();
    if (!dataFileNameToBuf(type, listName, tmpBuf, sizeof(tmpBuf))) return false;
    dataFile = m_fs->open(tmpBuf);
    return dataFile;
}