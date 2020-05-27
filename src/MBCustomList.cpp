#include "MBCustomList.h"

bool MBCustomList::open(const char *listName, const char *sortOrder) {
    sprintf(m_buf, "/%s/%s.dat", MB_PROBLIST_DIR, listName);
    _M->dataFile = m_fs->open(m_buf);
    if (!_M->dataFile) return false;
    // Read the number of problems and store
    _M->dataFile.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
    long int numProbs = strtol(m_buf, NULL, 10);
    if (numProbs <= 0) return false;
    _M->listSize = numProbs;
    while (numProbs > 0) { // Skip through all the problems
        _M->dataFile.readStringUntil('\n');
        numProbs--;
    }
    // Read and store the list of offsets
    _M->pageOffsets.clear();
    _M->dataFile.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    while (strlen(m_buf) > 0) {
        long int offset = strtol(m_buf, NULL, 10);
        if (offset > 0) _M->pageOffsets.push_back(offset);
        _M->dataFile.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    }
    // Rewind the list
    _M->dataFile.seek(0, SeekSet);
    _M->dataFile.readStringUntil('\n'); // Skip the first line (# of problems)

    _M->nextProbNum = 0;
    _M->listHasNext = fetchNextProblem();
    return true;
}

bool MBCustomList::seekPage(uint16_t pageNum) {
    if (!_M->dataFile) return false;
    if (pageNum >= _M->pageOffsets.size()) return false;
    if (!_M->dataFile.seek(_M->pageOffsets[pageNum], SeekSet)) return false;
    _M->nextProbNum = pageNum * _M->CONST_PAGE_SIZE;
    _M->listHasNext = fetchNextProblem();
    return true;
}
