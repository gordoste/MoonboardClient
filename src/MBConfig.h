#ifndef _MBCONFIG_H
#define _MBCONFIG_H

#include "FS.h"
#include "StringUtils.h"

#include "ConfConf.h"

#define MBCONFIG_FLAG_TEST_MODE 0x1

#define MBCONFIG_NUM_HOLDMAPS 5

typedef struct {
    char src[2];
    char dest[2];
} holdmap_t;

struct MBConfigData {
    holdmap_t holdMaps[MBCONFIG_NUM_HOLDMAPS];
    uint8_t flags = 0;
    MBConfigData() {
        memset(&holdMaps, ' ', sizeof(holdMaps));
    }
};

class MBConfig : public ConfConf<MBConfigData> {
public:
    bool writeHumanReadable(Stream &out, MBConfigData *src = NULL);
    void setFlags(bool testMode, MBConfigData *dest = NULL);

private:
    MBConfigData *readImpl(Stream &in, MBConfigData *dest);
    bool writeImpl(Stream &out, MBConfigData *src);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MBCONFIG)
extern MBConfig MoonboardConf;
#endif

#endif // #ifndef _MBCONFIG_H