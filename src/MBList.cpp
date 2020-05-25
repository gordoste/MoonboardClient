#include "MBList.h"

void MBList::begin(char *buf, uint16_t bufLen, FS *fs, Print *stdErr, ProblemParser parser) {
    m_buf = buf;
    m_bufLen = bufLen;
    m_stdErr = stdErr;
    m_fs = fs;
    m_parser = parser;
}

// m_nextProbNum needs to already be set to the new value before calling
bool MBList::fetchNextProblem() {
    char *_t_ptr_char;

    if (!m_data) {
        return false;
    } // Check list is open
    if (m_list) {
        strcpy(t_strtok, ":");
        m_list.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
        _t_ptr_char = StringUtils::strtoke(m_buf, t_strtok);
        if (_t_ptr_char == NULL) {
            return false;
        }
        _t_ptr_char = StringUtils::strtoke(NULL, t_strtok);
        if (_t_ptr_char == NULL) {
            return false;
        }
        m_data.seek(atoi(_t_ptr_char), SeekSet);
    }
    if (m_nextProbNum == m_listSize) return false;
    m_data.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
    if (strlen(m_buf) == 0) {
        return false;
    }
    return true;
}

void MBList::close() {
    if (m_list != NULL) m_list.close();
    if (m_data != NULL) m_data.close();
}

// Read next problem from the currently open list
// The problem will be pre-fetched into buffer each time
bool MBList::readNextProblem(Problem *prob) {
    if (!m_listHasNext) return false;             // List exhausted
    if (!m_parser(prob, m_buf)) return false; // Parse the one in cache
    m_nextProbNum++;
    m_listHasNext = fetchNextProblem(); // Fetch next one into cache
    return true;
}

uint16_t MBList::getPageNum() {
    return (m_nextProbNum - 1) / m_pageSize;
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
    return readNextProblems(pArr, m_pageSize);
}

uint8_t MBList::readPrevPage(Problem pArr[]) {
    if (!seekPage(getPageNum() - 1)) return 0;
    return readNextProblems(pArr, m_pageSize);
}

uint8_t MBList::readPage(Problem pArr[], uint16_t pageNum) {
    if (!seekPage(pageNum)) return 0;
    return readNextProblems(pArr, m_pageSize);
}