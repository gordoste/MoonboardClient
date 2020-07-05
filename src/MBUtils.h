#ifndef _MBUTILS_H
#define _MBUTILS_H

#include "MBData.h"
#include "MBList.h"
#include "StringUtils.h"
#include <FS.h>

class MBUtils {
public:
    void begin(char *tmpBuf, uint16_t tmpBufLen, FS *FS, Print *stdErr);
    void setStdErr(Print *stdErr);
    CategoryType *addCatType(const char *catType, bool wildcardOpt = true);
    SortOrder *addSortOrder(const char *sortOrderStr);

    CategoryType *getCatType(int8_t z_catType);
    SortOrder *getSortOrder(int8_t z_sortOrder);
    SortOrder *getSortOrderByName(const char *sortOrderName);

    uint8_t getNumCatTypes();
    uint8_t getNumSortOrders();
    uint8_t getNumCustomLists();

    uint8_t findCustomLists();
    const char *customListNumToName(uint8_t z_listNum);

    void selectCat_ss(const char *catTypeName, const char *catName);
    void selectCat_is(int8_t z_catType, const char *catName);
    void selectCat_si(const char *catTypeName, int8_t z_catNum);
    void selectCat_ii(int8_t z_catType, int8_t z_catNum);
    void unselectCat_s(const char *catTypeName);
    void unselectCat_i(int8_t z_catType);

    char *getSelectedCatName(int8_t z_catType);

    const char *getSelectedFilteredListName();
    bool selectedFilteredListExists() { return m_selectedFiltListExists; }

    bool openSelectedFilteredList(const SortOrder *sortOrder);
    bool openFilteredList(const char *listName, const SortOrder *sortOrder);
    bool openCustomList(uint8_t z_listNum, const SortOrder *);
    const char *getSelectedCustomListName();
    uint16_t getPageNum();

    int8_t catTypeToNum(const char *catTypeName);
    char *catNumToName(int8_t z_catType, int8_t z_catNum);
    int8_t catNameToNum(int8_t z_catType, const char *catName);

    MBList *getList() { return &m_list; }
    bool listIsOpen() { return m_list.isOpen(); }

    bool addProblem(const Problem *p, const char *listName, const std::vector<SortOrder *> *sortOrders);
    bool deleteProblem(const Problem *p, const char *listName, const std::vector<SortOrder *> *sortOrders);
    bool deleteProblemFromOpenList(const Problem *p, const std::vector<SortOrder *> *sortOrders);

    void showCatType(Print *outStr, CategoryType *ptrCT);
    void showAllCatTypes(Print *outStr);
    // void showStatus(Print *outStr);

    void updateStatus();

private:
    void beginCatType(char *catTypeName, bool wildcardOpt = true);
    CategoryType *endCatType();
    void addCat(const char *catName);
    void checkFileIsCustomList(const char *fileName);

    const char m_wildcardStr[4] = MB_WILDCARD_STRING;

    char m_catBuf[CAT_BUF_SIZE] = ""; // Storage for category names

    CategoryType m_catTypes[MAX_CAT_TYPES];
    uint8_t m_numCatTypes = 0;

    SortOrder m_sortOrders[MAX_SORT_ORDERS]; // Storage for sort order names
    uint8_t m_numSortOrders = 0;

    MBList m_list = MBList();

    FS *m_fs;
    Print *m_stdErr;
    char *m_tmpBuf;
    uint16_t m_tmpBufLen;

    bool m_selectedFiltListExists;
    char m_selectedFiltListName[MAX_LISTNAME_SIZE + 1];

    uint8_t m_numCustomLists = 0;
    char m_customListNames[MAX_CUSTOM_LISTS][MAX_LISTNAME_SIZE + 1];
    uint8_t m_selectedCustomList;

    char *t_catBufPtr = m_catBuf; // While setting up, point to where unused storage starts
    const uint8_t m_listDirSz = sizeof(MB_PROBLIST_DIR);

    char t_strtok[2]; // Token storage for strtok'ing
};

#endif // #ifndef _MBUTILS_H