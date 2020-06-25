#include "MBList.h"

void MBList::begin(char *buf, uint16_t bufLen, FS *fs, Print *stdErr) {
    m_buf = buf;
    m_bufLen = bufLen;
    m_stdErr = stdErr;
    m_fs = fs;
}

bool MBList::open(ListType type, const char *listName, const char *sortOrder) {
    switch (type) {
    case ListType::LIST_FILTER:
        sprintf(m_buf, "/%s_%s.lst", listName, sortOrder);
        break;
    case ListType::LIST_CUSTOM:
        sprintf(m_buf, "%s/%s_%s.lst", MB_PROBLIST_DIR, listName, sortOrder);
        break;
    default:
        return false;
    }

    listFile = m_fs->open(m_buf);
    if (!listFile) {
        m_stdErr->printf("'%s' can't open\n", m_buf);
        return false;
    }
    m_stdErr->printf("'%s' opened ok\n", m_buf);
    // Read the number of problems and store
    listFile.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
    long int numProbs = strtol(m_buf, NULL, 10);
    if (numProbs <= 0) return false;
    listSize = numProbs;
    while (numProbs > 0) { // Skip through all the problems
        listFile.readStringUntil('\n');
        numProbs--;
    }
    // Read and store the list of offsets
    pageOffsets.clear();
    listFile.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    while (strlen(m_buf) > 0) {
        long int offset = strtol(m_buf, NULL, 10);
        if (offset > 0) pageOffsets.push_back(offset);
        listFile.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    }
    // Reopen the list
    listFile.close();
    switch (type) {
    case ListType::LIST_FILTER:
        sprintf(m_buf, "/%s_%s.lst", listName, sortOrder);
        break;
    case ListType::LIST_CUSTOM:
        sprintf(m_buf, "%s/%s_%s.lst", MB_PROBLIST_DIR, listName, sortOrder);
        break;
    default:
        return false;
    }
    listFile = m_fs->open(m_buf);
    if (!listFile) return false;
    listFile.readStringUntil('\n'); // Skip the first line (# of problems)

    switch (type) {
    case ListType::LIST_FILTER:
        sprintf(m_buf, "/%s.dat", listName);
        break;
    case ListType::LIST_CUSTOM:
        sprintf(m_buf, "%s/%s.dat", MB_PROBLIST_DIR, listName);
        break;
    default:
        return false;
    }
    dataFile = m_fs->open(m_buf);
    if (!dataFile) {
        listFile.close();
        m_stdErr->printf("'%s' can't open\n", m_buf);
        return false;
    }
    m_stdErr->printf("'%s' opened ok\n", m_buf);
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
    char *_t_ptr_char;

    if (!dataFile) {
        return false;
    } // Check list is open
    if (listFile) {
        listFile.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
        _t_ptr_char = StringUtils::strtoke(m_buf, CONST_LIST_SEPARATOR);
        if (_t_ptr_char == NULL) {
            return false;
        }
        _t_ptr_char = StringUtils::strtoke(NULL, CONST_LIST_SEPARATOR);
        if (_t_ptr_char == NULL) {
            return false;
        }
        dataFile.seek(atoi(_t_ptr_char), SeekSet);
    }
    if (nextProbNum == listSize) return false;
    dataFile.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
    if (strlen(m_buf) == 0) {
        return false;
    }
    return true;
}

void MBList::close() {
    if (listFile) listFile.close();
    if (dataFile) dataFile.close();
}

// Read next problem from the currently open list
// The problem will be pre-fetched into buffer each time
bool MBList::readNextProblem(Problem *prob) {
    if (!listHasNext) return false;             // List exhausted
    if (!probParser(prob, m_buf)) return false; // Parse the one in cache
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