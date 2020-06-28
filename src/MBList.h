#ifndef _MBLIST_H
#define _MBLIST_H

#include "MBData.h"
#include "StringUtils.h"
#include <FS.h>

enum ListType {
    LIST_CUSTOM,
    LIST_FILTER
};

class MBList {
public:
    void begin(char *probBuf, uint16_t probBufLen, FS *FS, Print *stdErr);
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
    bool addProblem(const Problem *p, const char *listName, const std::vector<SortOrder *> *sortOrders);

protected:
    virtual bool seekPage(uint16_t pageNum);
    FS *m_fs;
    Print *m_stdErr;
    char *m_probBuf;
    uint16_t m_probBufLen;
    char tmpBuf[512];
    File listFile, dataFile;
    bool listHasNext = false;
    uint16_t nextProbNum = 0;
    uint16_t listSize = 0;
    const uint8_t CONST_PAGE_SIZE = PROB_PAGE_SIZE;
    std::vector<uint32_t> pageOffsets = std::vector<uint32_t>();
    bool openListFile(ListType type, const char *listName, const char *sortOrder);
    bool openDataFile(ListType type, const char *listName);
    bool listFileNameToBuf(ListType type, const char *listName, const char *sortOrder, char *buf, size_t bufLen);
    bool dataFileNameToBuf(ListType type, const char *listName, char *buf, size_t bufLen);
    int readListEntryAndSeekInData();
    bool comesBefore(const SortOrder *so, const Problem *p1, const Problem *p2);

    const char CONST_LIST_SEPARATOR[2] = ":";
};

#endif // #ifndef _MBLIST_H