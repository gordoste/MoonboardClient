#ifndef _MOONBOARD_DATA_H
#define _MOONBOARD_DATA_H

#include <Arduino.h>

#define MAX_CATTYPENAME_SIZE 8

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

struct CategoryType {
  char name[MAX_CATTYPENAME_SIZE];
  uint8_t catStartIdx; // What index its list of category names starts
  uint8_t catCount; // How many categories
  int8_t selectedCat; // Which category is selected. 0-based. -1 = NONE
};

#endif // #ifndef _MOONBOARD_DATA_H