#ifndef _MBLIST_H
#define _MBLIST_H

#include "Data.h"
#include "StringUtils.h"
#include <FS.h>

typedef std::function<bool(Problem *prob, char *buf)> ProblemParser;

class MBList {
public:
    void begin(char *buf, uint16_t bufLen, FS *FS, Print *stdErr, ProblemParser parser);
    virtual bool open(const char *listName, const char *sortOrder) = 0;
    void close();
    bool readNextProblem(Problem *p);
    uint8_t readNextProblems(Problem pArr[], uint8_t max);
    uint8_t getPageSize() { return m_pageSize; }
    bool hasNext() { return m_listHasNext; }
    bool hasPrevPage() { return m_nextProbNum > m_pageSize; };
    bool fetchNextProblem();
    uint16_t getPageNum();
    uint8_t readNextPage(Problem pArr[]);
    uint8_t readPrevPage(Problem pArr[]);
    uint8_t readPage(Problem pArr[], uint16_t pageNum);

protected:
    virtual bool seekPage(uint16_t pageNum) = 0;
    FS *m_fs;
    ProblemParser m_parser;
    Print *m_stdErr;
    File m_list, m_data;
    char *m_buf;
    uint16_t m_bufLen;
    bool m_listHasNext = false;
    uint16_t m_nextProbNum = 0;
    uint16_t m_listSize = 0;
    const uint8_t m_pageSize = PROB_PAGE_SIZE;
    std::vector<uint32_t> m_listOffsets = std::vector<uint32_t>();

    char t_strtok[2] = ":";
};

#endif // #ifndef _MBLIST_H