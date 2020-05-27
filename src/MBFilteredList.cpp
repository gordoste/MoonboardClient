#include "MBFilteredList.h"

bool MBFilteredList::open(const char *listName, const char *sortOrder) {
    sprintf(m_buf, "/%s_%s.lst", listName, sortOrder);
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
        if (offset > 0)_M->pageOffsets.push_back(offset);
        _M->listFile.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    }
    // Reopen the list
    _M->listFile.close();
    sprintf(m_buf, "/%s_%s.lst", listName, sortOrder);
    _M->listFile = m_fs->open(m_buf);
    if (!_M->listFile) return false;
    _M->listFile.readStringUntil('\n'); // Skip the first line (# of problems)

    sprintf(m_buf, "/%s.dat", listName);
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

bool MBFilteredList::seekPage(uint16_t pageNum) {
    if (!_M->dataFile) return false;
    if (pageNum >= _M->pageOffsets.size()) return false;
    if (!_M->listFile.seek(_M->pageOffsets[pageNum], SeekSet)) return false;
    _M->nextProbNum = pageNum * _M->CONST_PAGE_SIZE;
    _M->listHasNext = fetchNextProblem();
    return true;
}