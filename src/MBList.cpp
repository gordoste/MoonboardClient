#include "MBList.h"

void MBList::begin(char *buf, uint16_t bufLen, FS *fs, Print *stdErr, MBListMem *mem) {
    m_buf = buf;
    m_bufLen = bufLen;
    m_stdErr = stdErr;
    m_fs = fs;
    _M = mem;
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

    _M->listFile = m_fs->open(m_buf);
    if (!_M->listFile) {
        m_stdErr->printf("'%s' can't open\n", m_buf);
        return false;
    }
    m_stdErr->printf("'%s' opened ok\n", m_buf);
    // Read the number of problems and store
    _M->listFile.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
    long int numProbs = strtol(m_buf, NULL, 10);
    if (numProbs <= 0) return false;
    _M->listSize = numProbs;
    while (numProbs > 0) { // Skip through all the problems
        _M->listFile.readStringUntil('\n');
        numProbs--;
    }
    // Read and store the list of offsets
    _M->pageOffsets.clear();
    _M->listFile.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    while (strlen(m_buf) > 0) {
        long int offset = strtol(m_buf, NULL, 10);
        if (offset > 0) _M->pageOffsets.push_back(offset);
        _M->listFile.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    }
    // Reopen the list
    _M->listFile.close();
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
    _M->listFile = m_fs->open(m_buf);
    if (!_M->listFile) return false;
    _M->listFile.readStringUntil('\n'); // Skip the first line (# of problems)

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
    _M->dataFile = m_fs->open(m_buf);
    if (!_M->dataFile) {
        _M->listFile.close();
        m_stdErr->printf("'%s' can't open\n", m_buf);
        return false;
    }
    m_stdErr->printf("'%s' opened ok\n", m_buf);
    _M->nextProbNum = 0;
    _M->listHasNext = fetchNextProblem();
    return true;
}

bool MBList::seekPage(uint16_t pageNum) {
    if (!_M->dataFile) return false;
    if (pageNum >= _M->pageOffsets.size()) return false;
    if (!_M->listFile.seek(_M->pageOffsets[pageNum], SeekSet)) return false;
    _M->nextProbNum = pageNum * _M->CONST_PAGE_SIZE;
    _M->listHasNext = fetchNextProblem();
    return true;
}

// m_nextProbNum needs to already be set to the new value before calling
bool MBList::fetchNextProblem() {
    char *_t_ptr_char;

    if (!_M->dataFile) {
        return false;
    } // Check list is open
    if (_M->listFile) {
        _M->listFile.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
        _t_ptr_char = StringUtils::strtoke(m_buf, CONST_LIST_SEPARATOR);
        if (_t_ptr_char == NULL) {
            return false;
        }
        _t_ptr_char = StringUtils::strtoke(NULL, CONST_LIST_SEPARATOR);
        if (_t_ptr_char == NULL) {
            return false;
        }
        _M->dataFile.seek(atoi(_t_ptr_char), SeekSet);
    }
    if (_M->nextProbNum == _M->listSize) return false;
    _M->dataFile.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
    if (strlen(m_buf) == 0) {
        return false;
    }
    return true;
}

void MBList::close() {
    if (_M->listFile) _M->listFile.close();
    if (_M->dataFile) _M->dataFile.close();
}

// Read next problem from the currently open list
// The problem will be pre-fetched into buffer each time
bool MBList::readNextProblem(Problem *prob) {
    if (!_M->listHasNext) return false;             // List exhausted
    if (!_M->probParser(prob, m_buf)) return false; // Parse the one in cache
    _M->nextProbNum++;
    _M->listHasNext = fetchNextProblem(); // Fetch next one into cache
    return true;
}

uint16_t MBList::getPageNum() {
    return (_M->nextProbNum - 1) / _M->CONST_PAGE_SIZE;
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
    return readNextProblems(pArr, _M->CONST_PAGE_SIZE);
}

uint8_t MBList::readPrevPage(Problem pArr[]) {
    if (!seekPage(getPageNum() - 1)) return 0;
    return readNextProblems(pArr, _M->CONST_PAGE_SIZE);
}

uint8_t MBList::readPage(Problem pArr[], uint16_t pageNum) {
    if (!seekPage(pageNum)) return 0;
    return readNextProblems(pArr, _M->CONST_PAGE_SIZE);
}