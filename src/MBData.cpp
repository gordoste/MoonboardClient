#include "MBData.h"

int8_t problemAsString(Problem *p, char *buf, size_t bufLen) {
    if (bufLen < MAX_PROBLEMNAME_LEN + 20) {
        return -1;
    }
    char fmtStr[16] = "%XXs %s V%d %5d";
    char t_num[3];
    snprintf(t_num, 3, "%d", MAX_PROBLEMNAME_LEN);
    strncpy(&(fmtStr[1]), t_num, 2);
    sprintf(buf, fmtStr, p->name, p->isBenchmark ? "(B)" : "   ", p->grade, p->repeats);
    switch (p->rating) {
    case 0:
        strcat(buf, "    ");
        break;
    case 1:
        strcat(buf, " *  ");
        break;
    case 2:
        strcat(buf, " ** ");
        break;
    case 3:
        strcat(buf, " ***");
        break;
    }
    return strlen(buf);
}