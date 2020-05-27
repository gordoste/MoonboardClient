#include "MBConfig.h"

MBConfigData *MBConfig::read(File &configFile) {
    if (!m_data.top_panel_ip.fromString(configFile.readStringUntil(':'))) return NULL;
    if (!m_data.mid_panel_ip.fromString(configFile.readStringUntil(':'))) return NULL;
    if (!m_data.btm_panel_ip.fromString(configFile.readStringUntil(':'))) return NULL;
    m_data.flags = atoi(configFile.readStringUntil(':').c_str());
    return &m_data;
}

bool MBConfig::write(File &configFile) {
    return configFile.printf("%s:%s:%s:%i:",
                      m_data.top_panel_ip.toString().c_str(),
                      m_data.mid_panel_ip.toString().c_str(),
                      m_data.btm_panel_ip.toString().c_str(),
                      m_data.flags) > 0;
}

void MBConfig::setFlags(bool testMode) {
    m_data.flags = (testMode ? MBCONFIG_TEST_MODE : 0x0); // OR any other flags here
}

bool MBConfig::testMode() { return m_data.flags & MBCONFIG_TEST_MODE; }

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MBCONFIG)
MBConfig MOON_CONF;
#endif
