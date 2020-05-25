#include "MBFilteredList.h"

bool MBFilteredList::open(const char *listName, const char *sortOrder) {
    sprintf(m_buf, "/%s_%s.lst", listName, sortOrder);
    m_list = m_fs->open(m_buf);
    if (!m_list) {
        m_stdErr->printf("'%s' can't open\n", m_buf);
        return false;
    }
    m_stdErr->printf("'%s' opened ok\n", m_buf);
    // Read the number of problems and store
    m_list.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
    long int numProbs = strtol(m_buf, NULL, 10);
    if (numProbs <= 0) return false;
    m_listSize = numProbs;
    while (numProbs > 0) { // Skip through all the problems
        m_list.readStringUntil('\n');
        numProbs--;
    }
    // Read and store the list of offsets
    m_listOffsets.clear();
    m_list.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    while (strlen(m_buf) > 0) {
        long int offset = strtol(m_buf, NULL, 10);
        if (offset > 0) m_listOffsets.push_back(offset);
        m_list.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    }
    // Reopen the list
    m_list.close();
    sprintf(m_buf, "/%s_%s.lst", listName, sortOrder);
    m_list = m_fs->open(m_buf);
    if (!m_list) return false;
    m_list.readStringUntil('\n'); // Skip the first line (# of problems)

    sprintf(m_buf, "/%s.dat", listName);
    m_data = m_fs->open(m_buf);
    if (!m_data) {
        m_list.close();
        m_stdErr->printf("'%s' can't open\n", m_buf);
        return false;
    }
    m_stdErr->printf("'%s' opened ok\n", m_buf);
    m_nextProbNum = 0;
    m_listHasNext = fetchNextProblem();
    return true;
}

bool MBFilteredList::seekPage(uint16_t pageNum) {
    if (!m_data) return false;
    if (pageNum >= m_listOffsets.size()) return false;
    if (!m_list.seek(m_listOffsets[pageNum], SeekSet)) return false;
    m_nextProbNum = pageNum * m_pageSize;
    m_listHasNext = fetchNextProblem();
    return true;
}