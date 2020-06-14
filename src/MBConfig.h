#ifndef _MBCONFIG_H
#define _MBCONFIG_H

#include "FS.h"
#include "IPAddress.h"
#include "StringUtils.h"

#define MBCONFIG_TEST_MODE 0x1

struct MBConfigData {
    IPAddress top_panel_ip;
    uint16_t top_port;
    IPAddress mid_panel_ip;
    uint16_t mid_port;
    IPAddress btm_panel_ip;
    uint16_t btm_port;
    uint8_t flags;
};

class MBConfig {
public:
    MBConfigData *read(Stream &in, MBConfigData *dest = NULL);
    bool readConfig(FS &fs, String fileName, MBConfigData *dest = NULL);
    bool write(Stream &out, MBConfigData *src = NULL);
    bool writeConfig(FS &fs, const char *fnam, MBConfigData *src = NULL);
    bool writeHumanReadable(Stream &out, MBConfigData *src = NULL);
    bool writeNewConf(Stream &out);
    MBConfigData *fromString(char *str, MBConfigData *dest = NULL);
    void setFlags(bool testMode, MBConfigData *dest = NULL);
    bool testMode(MBConfigData *dest = NULL);
    bool parseTopIPAndPort(const char *str, MBConfigData *dest = NULL);
    bool parseMidIPAndPort(const char *str, MBConfigData *dest = NULL);
    bool parseBtmIPAndPort(const char *str, MBConfigData *dest = NULL);

private:
    bool parseIPAndPort(const char *str, IPAddress &dstIP, uint16_t &dstPort);
    MBConfigData m_data;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MBCONFIG)
extern MBConfig MoonboardConf;
#endif

#endif // #ifndef _MBCONFIG_H