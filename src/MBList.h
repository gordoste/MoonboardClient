#ifndef _MBLIST_H
#define _MBLIST_H

#include "MBData.h"
#include "StringUtils.h"
#include <FS.h>

class MBList {
public:
    void begin(char *tmpBuf, uint16_t tmpBufLen, FS *FS, Print *stdErr);
    virtual bool open(ListType type, const char *listName, const char *sortOrder);
    void close();
    bool isOpen();
    bool readNextProblem(Problem *p);
    uint8_t readNextProblems(Problem pArr[], uint8_t max);
    uint8_t getPageSize() { return CONST_PAGE_SIZE; }
    bool hasNext() { return listHasNext; }
    bool hasPrevPage() { return nextProbNum > CONST_PAGE_SIZE; };
    bool fetchNextProblem();
    uint16_t getPageNum();
    uint8_t readNextPage(Problem pArr[]);
    uint8_t readPrevPage(Problem pArr[]);
    uint8_t readPage(Problem pArr[], uint16_t pageNum);

protected:
    virtual bool seekPage(uint16_t pageNum);
    FS *m_fs;
    Print *m_stdErr;
    char m_probBuf[256];
    char *m_tmpBuf;
    uint16_t m_tmpBufLen;
    File listFile, dataFile;
    bool listHasNext = false;
    uint16_t nextProbNum = 0;
    uint16_t listSize = 0;
    std::vector<uint32_t> pageOffsets = std::vector<uint32_t>();
    bool openListFile(ListType type, const char *listName, const char *sortOrder);
    bool openDataFile(ListType type, const char *listName);
};

#endif // #ifndef _MBLIST_H