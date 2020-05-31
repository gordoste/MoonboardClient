#ifndef _MBCONFIG_H
#define _MBCONFIG_H

#include "FS.h"
#include "IPAddress.h"

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
    MBConfigData *read(File &configFile);
    bool write(File &configFile);
    void setFlags(bool testMode);
    bool testMode();

private:
    MBConfigData m_data;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MBCONFIG)
extern MBConfig MOON_CONF;
#endif

#endif // #ifndef _MBCONFIG_H