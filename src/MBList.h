#ifndef _MBLIST_H
#define _MBLIST_H

#include "MBData.h"
#include "StringUtils.h"
#include <FS.h>

class MBList {
public:
    // tmpBuf, tmpBufLen - working memory, which can be re-used between calls to this class
    // FS - filesystem where data is stored
    void begin(char *tmpBuf, uint16_t tmpBufLen, FS *FS, Print *stdErr);
    bool open(ListType type, const char *listName, const SortOrder *sortOrder);
    void close();
    bool isOpen();
    bool readNextProblem(Problem *p);
    uint8_t readNextProblems(Problem pArr[], uint8_t max);
    uint8_t getPageSize() { return CONST_PAGE_SIZE; }
    bool hasNext() { return m_listHasNext; }
    bool hasPrevPage() { return m_nextProbNum > CONST_PAGE_SIZE; };
    bool fetchNextProblem();
    ListType getType() { return m_listType; }
    const char *getName() { return (const char *)m_listName; }
    const SortOrder *getSortOrder() { return m_sortOrder; }
    uint16_t getPageNum();
    uint8_t readNextPage(Problem pArr[]);
    uint8_t readPrevPage(Problem pArr[]);
    uint8_t readPage(Problem pArr[], uint16_t pageNum);
    uint16_t size() { return m_listSize; }

protected:
    bool seekPage(uint16_t pageNum);
    FS *m_fs;
    Print *m_stdErr;
    char m_probBuf[256];
    char *m_tmpBuf;
    uint16_t m_tmpBufLen;
    File m_listFile, m_dataFile;
    bool m_listHasNext = false;
    uint16_t m_nextProbNum = 0;
    uint16_t m_listSize = 0;
    ListType m_listType;
    char m_listName[MAX_LISTNAME_SIZE+1];
    const SortOrder *m_sortOrder;
    std::vector<uint32_t> pageOffsets = std::vector<uint32_t>();
    bool openListFile(ListType type, const char *listName, const SortOrder *sortOrder);
    bool openDataFile(ListType type, const char *listName);
};

#endif // #ifndef _MBLIST_H