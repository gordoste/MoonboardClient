#include "MBConfig.h"

MBConfigData *MBConfig::readImpl(Stream &in, MBConfigData *dest) {
    for (uint8_t i = 0; i < MBCONFIG_NUM_HOLDMAPS; i++) {
        dest->holdMaps[i].src[0] = (char)in.read();
        dest->holdMaps[i].src[1] = (char)in.read();
        in.read();
        dest->holdMaps[i].dest[0] = (char)in.read();
        dest->holdMaps[i].dest[1] = (char)in.read();
        in.read();
    }
    String s = in.readStringUntil('\n');
    dest->flags = atoi(s.c_str());
    return dest;
}

bool MBConfig::writeImpl(Stream &out, MBConfigData *src) {
    for (uint8_t i = 0; i < MBCONFIG_NUM_HOLDMAPS; i++) {
        out.printf("%c%c:%c%c|",
        src->holdMaps[i].src[0], src->holdMaps[i].src[1],
        src->holdMaps[i].dest[0], src->holdMaps[i].dest[1]);
    }
    out.printf("%i\n", src->flags);
    return true;
}

bool MBConfig::writeHumanReadable(Stream &out, MBConfigData *src) {
    if (src == NULL) src = &m_data;
    out.print("Holdmaps:");
    for (uint8_t i = 0; (i < MBCONFIG_NUM_HOLDMAPS) && (src->holdMaps[i].src[0] != ' '); i++) {
        if (i > 0) out.print(", ");
        out.printf("%c%c -> %c%c",
        src->holdMaps[i].src[0], src->holdMaps[i].src[1],
        src->holdMaps[i].dest[0], src->holdMaps[i].dest[1]);
    }
    out.printf("\r\nFlags: %i\r\n", src->flags);
    return true;
}

void MBConfig::setFlags(bool testMode, MBConfigData *dest) {
    if (dest == NULL) dest = &m_data;
    dest->flags = testMode && MBCONFIG_FLAG_TEST_MODE; // OR additional flags
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MBCONFIG)
MBConfig MoonboardConf;
#endif
