#ifndef _MBLIST_H
#define _MBLIST_H

#include "Data.h"
#include "StringUtils.h"
#include <FS.h>

typedef std::function<bool(Problem *prob, char *buf)> ProblemParser;

struct MBListMem {
    ProblemParser probParser;
    File listFile, dataFile;
    bool listHasNext = false;
    uint16_t nextProbNum = 0;
    uint16_t listSize = 0;
    const uint8_t CONST_PAGE_SIZE = PROB_PAGE_SIZE;
    std::vector<uint32_t> pageOffsets = std::vector<uint32_t>();
};

class MBList {
public:
    void begin(char *buf, uint16_t bufLen, FS *FS, Print *stdErr, MBListMem *mem);
    virtual bool open(const char *listName, const char *sortOrder) = 0;
    void close();
    bool readNextProblem(Problem *p);
    uint8_t readNextProblems(Problem pArr[], uint8_t max);
    uint8_t getPageSize() { return _M->CONST_PAGE_SIZE; }
    bool hasNext() { return _M->listHasNext; }
    bool hasPrevPage() { return _M->nextProbNum > _M->CONST_PAGE_SIZE; };
    bool fetchNextProblem();
    uint16_t getPageNum();
    uint8_t readNextPage(Problem pArr[]);
    uint8_t readPrevPage(Problem pArr[]);
    uint8_t readPage(Problem pArr[], uint16_t pageNum);

protected:
    virtual bool seekPage(uint16_t pageNum) = 0;
    FS *m_fs;
    Print *m_stdErr;
    char *m_buf;
    uint16_t m_bufLen;
    MBListMem *_M;

    const char CONST_LIST_SEPARATOR[2] = ":";
};

#endif // #ifndef _MBLIST_H