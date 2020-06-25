#ifndef _MBLIST_H
#define _MBLIST_H

#include "Data.h"
#include "StringUtils.h"
#include <FS.h>

typedef std::function<bool(Problem *prob, char *buf)> ProblemParser;

enum ListType {
    LIST_CUSTOM,
    LIST_FILTER
};

class MBList {
public:
    void begin(char *buf, uint16_t bufLen, FS *FS, Print *stdErr);
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
    void setProblemParser(ProblemParser _pp) { probParser = _pp; }

protected:
    virtual bool seekPage(uint16_t pageNum);
    FS *m_fs;
    Print *m_stdErr;
    char *m_buf;
    uint16_t m_bufLen;
    ProblemParser probParser;
    File listFile, dataFile;
    bool listHasNext = false;
    uint16_t nextProbNum = 0;
    uint16_t listSize = 0;
    const uint8_t CONST_PAGE_SIZE = PROB_PAGE_SIZE;
    std::vector<uint32_t> pageOffsets = std::vector<uint32_t>();

    const char CONST_LIST_SEPARATOR[2] = ":";
};

#endif // #ifndef _MBLIST_H