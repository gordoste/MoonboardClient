#ifndef _MBDATA_H
#define _MBDATA_H

#include "StringUtils.h"
#include <Arduino.h>
#include "FS.h"

#define MAX_LISTNAME_SIZE 15
#define MAX_SORT_ORDERS 4
#define CAT_BUF_SIZE 256
#define MAX_CAT_TYPES 5
#define MAX_CUSTOM_LISTS 10

#ifndef MB_WILDCARD_STRING
#define MB_WILDCARD_STRING "Any"
#endif

#define MAX_CATTYPENAME_LEN 9
#define MAX_CATS_PER_CATTYPE 5
#define MAX_SORTORDER_NAME_LEN 4
#define MAX_SORTORDER_DSPNAME_LEN 8
#define MAX_PROBLEMNAME_LEN 42
#define MAX_HOLDS_PER_PANEL 10

#define CONST_PAGE_SIZE 8

// Directory where problem lists are stored
#ifndef MB_PROBLIST_DIR
#define MB_PROBLIST_DIR "/l"
#endif

const char catWildcardStr[2] = "*";

struct Problem {
    char name[MAX_PROBLEMNAME_LEN + 1];
    uint8_t grade;
    uint8_t rating;
    uint16_t repeats;
    bool isBenchmark;
    char bottomHolds[MAX_HOLDS_PER_PANEL * 3]; // 3 = 2 for hold, 1 for delim or null
    char middleHolds[MAX_HOLDS_PER_PANEL * 3];
    char topHolds[MAX_HOLDS_PER_PANEL * 3];
};

class CategoryType {
public:
    char name[MAX_CATTYPENAME_LEN + 1];
    uint8_t catCount = 0;    // How many categories
    int8_t selectedCat = -1; // Which category is selected. 0-based. -1 = NONE
    bool wildcardOpt = true;
    char *catNames[MAX_CATS_PER_CATTYPE];
    void addCat(char *catName) {
        if (catCount == MAX_CATS_PER_CATTYPE) return;
        catNames[catCount++] = catName;
    }
    char *getSelectedCat() { return (selectedCat == -1) ? NULL : catNames[selectedCat]; }
};

struct SortOrder {
    char name[MAX_SORTORDER_NAME_LEN + 1];
    char displayName[MAX_SORTORDER_DSPNAME_LEN + 1];
    bool exists; // Does it exist for the current selection?
};

int8_t problemAsString(Problem *p, char *buf, size_t bufLen);
bool parseProblem(Problem *p, char *in);
size_t writeProblem(const Problem *prob, Stream &out);
bool comesBefore(const SortOrder *so, const Problem *p1, const Problem *p2);

enum ListType {
    LIST_CUSTOM,
    LIST_FILTER
};

namespace MBData {
bool listFileNameToBuf(ListType type, const char *listName, const SortOrder *sortOrder, char *buf, size_t bufLen);
bool dataFileNameToBuf(ListType type, const char *listName, char *buf, size_t bufLen);
int readListEntryAndSeekInData(File &_list, File &_data, char *buf, size_t bufLen, bool seekInData = true);
int readListEntryDataOffset(File &_list, char *buf, size_t bufLen);
} // namespace MBData

#endif // #ifndef _MBDATA_H