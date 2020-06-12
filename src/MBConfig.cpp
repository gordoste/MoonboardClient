#include "MBConfig.h"

MBConfigData *MBConfig::read(Stream &in, MBConfigData *dest) {
    if (dest == NULL) dest = &m_data;
    if (!dest->top_panel_ip.fromString(in.readStringUntil(':'))) return NULL;
    dest->top_port = atoi(in.readStringUntil('|').c_str());
    if (!dest->mid_panel_ip.fromString(in.readStringUntil(':'))) return NULL;
    dest->mid_port = atoi(in.readStringUntil('|').c_str());
    if (!dest->btm_panel_ip.fromString(in.readStringUntil(':'))) return NULL;
    dest->btm_port = atoi(in.readStringUntil('|').c_str());
    dest->flags = atoi(in.readStringUntil('|').c_str());
    return dest;
}

bool MBConfig::write(Stream &out, MBConfigData *src) {
    if (src == NULL) src = &m_data;
    return out.printf("%s:%i|%s:%i|%s:%i|%i|",
                      src->top_panel_ip.toString().c_str(),
                      src->top_port,
                      src->mid_panel_ip.toString().c_str(),
                      src->mid_port,
                      src->btm_panel_ip.toString().c_str(),
                      src->btm_port,
                      src->flags) > 0;
}

bool MBConfig::writeHumanReadable(Stream &out, MBConfigData *src) {
    if (src == NULL) src = &m_data;
    return out.printf("Top: %s:%i\r\nMiddle: %s:%i\r\nBottom: %s:%i\r\nFlags: %i\r\n",
                      src->top_panel_ip.toString().c_str(),
                      src->top_port,
                      src->mid_panel_ip.toString().c_str(),
                      src->mid_port,
                      src->btm_panel_ip.toString().c_str(),
                      src->btm_port,
                      src->flags) > 0;
}

bool MBConfig::writeNewConf(Stream &out) {
    return out.println("0.0.0.0:1|0.0.0.0:1|0.0.0.0:1|1|");
}

MBConfigData *MBConfig::fromString(char *str, MBConfigData *dest) {
    if (dest == NULL) dest = &m_data;
    char *tokptr = NULL;
    tokptr = strtok(str, ":");
    if (tokptr == NULL || !dest->top_panel_ip.fromString(tokptr)) return NULL;
    tokptr = strtok(str, "|");
    if (tokptr == NULL) return NULL;
    dest->top_port = atoi(tokptr);
    tokptr = strtok(str, ":");
    if (tokptr == NULL || !dest->mid_panel_ip.fromString(tokptr)) return NULL;
    tokptr = strtok(str, "|");
    if (tokptr == NULL) return NULL;
    dest->mid_port = atoi(tokptr);
    tokptr = strtok(str, ":");
    if (tokptr == NULL || !dest->btm_panel_ip.fromString(tokptr)) return NULL;
    tokptr = strtok(str, "|");
    if (tokptr == NULL) return NULL;
    dest->btm_port = atoi(tokptr);
    tokptr = strtok(str, "|");
    if (tokptr == NULL) return NULL;
    dest->flags = atoi(tokptr);
    return dest;
}

void MBConfig::setFlags(bool testMode, MBConfigData *dest) {
    if (dest == NULL) dest = &m_data;
    dest->flags = (testMode ? MBCONFIG_TEST_MODE : 0x0); // OR any other flags here
}

bool MBConfig::testMode(MBConfigData *dest) {
    if (dest == NULL) dest = &m_data;
    return dest->flags & MBCONFIG_TEST_MODE;
}

bool MBConfig::parseTopIPAndPort(const char *str, MBConfigData *dest) {
    return parseIPAndPort(str, dest->top_panel_ip, dest->top_port);
}

bool MBConfig::parseMidIPAndPort(const char *str, MBConfigData *dest) {
    return parseIPAndPort(str, dest->mid_panel_ip, dest->mid_port);
}

bool MBConfig::parseBtmIPAndPort(const char *str, MBConfigData *dest) {
    return parseIPAndPort(str, dest->btm_panel_ip, dest->btm_port);
}

// Parse a string that should contain IP:port into given variables
bool MBConfig::parseIPAndPort(const char *str, IPAddress &dstIP, uint16_t &dstPort) {
#define IP_PORT_MAXLEN 24 // 3+1+3+1+3+1+3 + 1 + 5 + 1 (plus couple extra)
    char tmpBuf[IP_PORT_MAXLEN];
    IPAddress newAddr;
    int newport = -1;
    char *ptr;
    uint8_t len;
    ptr = strchr(str, '|');
    len = (ptr == NULL ? strlen(str) : ptr - str);
    strncpy(tmpBuf, str, len);
    tmpBuf[len] = '\0';
    ptr = StringUtils::strtoke(tmpBuf, ":");
    if (ptr == NULL) return false;
    if (!newAddr.fromString(ptr)) return false;
    ptr = StringUtils::strtoke(NULL, "|");
    newport = atoi(ptr);
    if (newport == -1) return false;
    dstIP = newAddr;
    dstPort = newport;
    return true;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MBCONFIG)
MBConfig MoonboardConf;
#endif
