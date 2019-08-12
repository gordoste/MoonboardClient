#ifndef _MOONBOARDCLIENT_DATA_H
#define _MOONBOARDCLIENT_DATA_H

#include <Arduino.h>

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

#endif // #ifndef _MOONBOARDCLIENT_DATA_H