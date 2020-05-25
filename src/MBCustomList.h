#ifndef _MB_CUSTOM_LIST_H
#define _MB_CUSTOM_LIST_H

#include "MBList.h"

class MBCustomList : public MBList {
public:
    bool open(const char *listName, const char *sortOrder);
    void close();

private:
    bool seekPage(uint16_t pageNum);
};

#endif // #ifndef _MB_CUSTOM_LIST_H
