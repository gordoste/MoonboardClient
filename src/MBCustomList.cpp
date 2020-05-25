#include "MBCustomList.h"

bool MBCustomList::open(const char *listName, const char *sortOrder) {
    sprintf(m_buf, "/%s/%s.dat", MB_PROBLIST_DIR, listName);
    m_data = m_fs->open(m_buf);
    if (!m_data) return false;
    // Read the number of problems and store
    m_data.readStringUntil('\n').toCharArray(m_buf, m_bufLen);
    long int numProbs = strtol(m_buf, NULL, 10);
    if (numProbs <= 0) return false;
    m_listSize = numProbs;
    while (numProbs > 0) { // Skip through all the problems
        m_data.readStringUntil('\n');
        numProbs--;
    }
    // Read and store the list of offsets
    m_listOffsets.clear();
    m_data.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    while (strlen(m_buf) > 0) {
        long int offset = strtol(m_buf, NULL, 10);
        if (offset > 0) m_listOffsets.push_back(offset);
        m_data.readStringUntil(':').toCharArray(m_buf, m_bufLen);
    }
    // Rewind the list
    m_data.seek(0, SeekSet);
    m_data.readStringUntil('\n'); // Skip the first line (# of problems)

    m_nextProbNum = 0;
    m_listHasNext = fetchNextProblem();
    return true;
}

bool MBCustomList::seekPage(uint16_t pageNum) {
    if (!m_data) return false;
    if (pageNum >= m_listOffsets.size()) return false;
    if (!m_data.seek(m_listOffsets[pageNum], SeekSet)) return false;
    m_nextProbNum = pageNum * m_pageSize;
    m_listHasNext = fetchNextProblem();
    return true;
}
