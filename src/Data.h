#ifndef _MOONBOARD_DATA_H
#define _MOONBOARD_DATA_H

#include <Arduino.h>

#define MAX_CATTYPENAME_LEN 9
#define MAX_CATS_PER_CATTYPE 5
#define MAX_SORTORDER_NAME_LEN 4
#define MAX_SORTORDER_DSPNAME_LEN 8

struct Problem
{
    char *name;
    uint8_t grade;
    uint8_t rating;
    uint8_t repeats;
    bool isBenchmark;
    char *bottomHolds;
    char *middleHolds;
    char *topHolds;
};

class CategoryType {
  public:
    char name[MAX_CATTYPENAME_LEN+1];
    uint8_t catCount; // How many categories
    int8_t selectedCat; // Which category is selected. 0-based. -1 = NONE
    char *catNames[MAX_CATS_PER_CATTYPE];
    void addCat(char *catName) {
      if (catCount == MAX_CATS_PER_CATTYPE) { Serial.println("CT::aC - too many"); return; }
      catNames[catCount++] = catName;
    }
    char *getSelectedCat() { return (selectedCat == -1) ? NULL : catNames[selectedCat]; }
};

struct SortOrder {
  char name[MAX_SORTORDER_NAME_LEN+1];
  char displayName[MAX_SORTORDER_DSPNAME_LEN+1];
};

#endif // #ifndef _MOONBOARD_DATA_H