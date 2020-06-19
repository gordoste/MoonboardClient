#ifndef _MBCONFIG_H
#define _MBCONFIG_H

#include "FS.h"
#include "IPAddress.h"
#include "StringUtils.h"

#include "ConfConf.h"

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

class MBConfig : ConfConf<MBConfigData> {
public:
    bool writeHumanReadable(Stream &out, MBConfigData *src = NULL);
    void setFlags(bool testMode, MBConfigData *dest = NULL);
    bool testMode(MBConfigData *dest = NULL);
    bool parseTopIPAndPort(const char *str, MBConfigData *dest = NULL);
    bool parseMidIPAndPort(const char *str, MBConfigData *dest = NULL);
    bool parseBtmIPAndPort(const char *str, MBConfigData *dest = NULL);

private:
    MBConfigData *readImpl(Stream &in, MBConfigData *dest);
    bool writeImpl(Stream &out, MBConfigData *src);
    bool parseIPAndPort(const char *str, IPAddress &dstIP, uint16_t &dstPort);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MBCONFIG)
extern MBConfig MoonboardConf;
#endif

#endif // #ifndef _MBCONFIG_H