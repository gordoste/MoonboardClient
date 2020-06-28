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

// read the string, placing data in the problem struct passed. Return true on success
bool parseProblem(Problem *prob, char *in) {
    char *_t_ptr_char = StringUtils::strtoke(in, "|");
    if (_t_ptr_char == NULL) {
        return false;
    }
    if (strlen(_t_ptr_char) > MAX_PROBLEMNAME_LEN) {
        // m_stdErr->printf("MBU::pP - n '%s' too long (%d)\n", _t_ptr_char, strlen(_t_ptr_char));
        return false;
    }
    strcpy(prob->name, _t_ptr_char);
    _t_ptr_char = StringUtils::strtoke(NULL, "|");
    if (_t_ptr_char == NULL) {
        // m_stdErr->printf("MBU::pP - '%s' no grade\n", prob->name);
        return false;
    }
    if (_t_ptr_char[0] != 'V') {
        // m_stdErr->printf("MBU::pP - '%s' no V\n", prob->name);
        return false;
    }
    prob->grade = atoi(&(_t_ptr_char[1]));
    if (prob->grade == 0) {
        // m_stdErr->printf("MBU::pP - '%s' bad grade\n", prob->name);
        return false;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, "|");
    if (_t_ptr_char == NULL) {
        // m_stdErr->printf("MBU::pP - '%s' no rating\n", prob->name);
        return false;
    }
    prob->rating = atoi(_t_ptr_char);
    if (prob->rating == 0) {
        // m_stdErr->printf("MBU::pP - '%s' bad rating\n", prob->name);
        return false;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, "|");
    if (_t_ptr_char == NULL) {
        // m_stdErr->printf("MBU::pP - '%s' no repeats\n", prob->name);
        return false;
    }
    prob->repeats = atoi(_t_ptr_char);
    if (prob->repeats == 0 && (_t_ptr_char[0] != '0' || _t_ptr_char[1] != '\0')) {
        // m_stdErr->printf("MBU::pP - '%s' NaN repeats\n", prob->name);
        return false;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, "|");
    if (_t_ptr_char == NULL) {
        // m_stdErr->printf("MBU::pP - '%s' no BM\n", prob->name);
        return false;
    }
    switch (_t_ptr_char[0]) {
    case 'Y':
        prob->isBenchmark = true;
        break;
    case 'N':
        prob->isBenchmark = false;
        break;
    default:
        return false;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, "|");
    if (_t_ptr_char == NULL) {
        // m_stdErr->printf("MBU::pP - '%s' no BH\n", prob->name);
        return false;
    }
    if (strlen(_t_ptr_char) > MAX_HOLDS_PER_PANEL * 3) {
        // m_stdErr->printf("MBU::pP - bH '%s' too long\n", _t_ptr_char);
        return false;
    }
    strcpy(prob->bottomHolds, _t_ptr_char);
    _t_ptr_char = StringUtils::strtoke(NULL, "|");
    if (_t_ptr_char == NULL) {
        // m_stdErr->printf("MBU::pP - '%s' no MH\n", prob->name);
        return false;
    }
    if (strlen(_t_ptr_char) > MAX_HOLDS_PER_PANEL * 3) {
        // m_stdErr->printf("MBU::pP - mH '%s' too long\n", _t_ptr_char);
        return false;
    }
    strcpy(prob->middleHolds, _t_ptr_char);
    _t_ptr_char = StringUtils::strtoke(NULL, "|");
    if (_t_ptr_char == NULL) {
        // m_stdErr->printf("MBU::pP - '%s' no TH\n", prob->name);
        return false;
    }
    if (strlen(_t_ptr_char) > MAX_HOLDS_PER_PANEL * 3) {
        // m_stdErr->printf("MBU::pP - tH '%s' too long\n", _t_ptr_char);
        return false;
    }
    strcpy(prob->topHolds, _t_ptr_char);
    return true;
}

size_t writeProblem(const Problem *prob, Stream &out) {
    return out.printf("%s|V%i|%i|%i|%c|%s|%s|%s\n",
               prob->name, prob->grade, prob->rating, prob->repeats, prob->isBenchmark ? 'Y' : 'N',
               prob->bottomHolds, prob->middleHolds, prob->topHolds);
}

bool MBData::listFileNameToBuf(ListType type, const char *listName, const char *sortOrder, char *buf, size_t bufLen) {
    switch (type) {
    case ListType::LIST_FILTER:
        snprintf(buf, bufLen, "/%s_%s.lst", listName, sortOrder);
        break;
    case ListType::LIST_CUSTOM:
        snprintf(buf, bufLen, "%s/%s_%s.lst", MB_PROBLIST_DIR, listName, sortOrder);
        break;
    default:
        return false;
    }
    return true;
}
bool MBData::dataFileNameToBuf(ListType type, const char *listName, char *buf, size_t bufLen) {
    switch (type) {
    case ListType::LIST_FILTER:
        snprintf(buf, bufLen, "/%s.dat", listName);
        break;
    case ListType::LIST_CUSTOM:
        snprintf(buf, bufLen, "%s/%s.dat", MB_PROBLIST_DIR, listName);
        break;
    default:
        return false;
    }
    return true;
}

// Return true if p1 comes before p2 in the specified sort order
bool comesBefore(const SortOrder *so, const Problem *p1, const Problem *p2) {
    if (strncmp(so->name, "name", 4) == 0) {
        return strcmp(p1->name, p2->name) >= 0;
    }
    if (strncmp(so->name, "rpts", 4) == 0) {
        return p1->repeats > p2->repeats;
    }
    return false;
}

int MBData::readListEntryAndSeekInData(File &_list, File &_data, char *buf, size_t bufLen) {
    // Check list is open
    char *_t_ptr_char;
    if (!_list) return false;
    _list.readStringUntil('\n').toCharArray(buf, bufLen);
    _t_ptr_char = StringUtils::strtoke(buf, ":");
    if (_t_ptr_char == NULL) {
        return -1;
    }
    _t_ptr_char = StringUtils::strtoke(NULL, ":");
    if (_t_ptr_char == NULL) {
        return -1;
    }
    int offset = atoi(_t_ptr_char);
    if (!_data.seek(offset, SeekSet)) return -1;
    return offset;
}