#include "MBConfig.h"

MBConfigData *MBConfig::read(Stream &configFile) {
    if (!m_data.top_panel_ip.fromString(configFile.readStringUntil(':'))) return NULL;
    m_data.top_port = atoi(configFile.readStringUntil('|').c_str());
    if (!m_data.mid_panel_ip.fromString(configFile.readStringUntil(':'))) return NULL;
    m_data.mid_port = atoi(configFile.readStringUntil('|').c_str());
    if (!m_data.btm_panel_ip.fromString(configFile.readStringUntil(':'))) return NULL;
    m_data.btm_port = atoi(configFile.readStringUntil('|').c_str());
    m_data.flags = atoi(configFile.readStringUntil('|').c_str());
    return &m_data;
}

bool MBConfig::write(Stream &cfgStream) {
    return cfgStream.printf("%s:%i|%s:%i|%s:%i|%i|",
                      m_data.top_panel_ip.toString().c_str(),
                      m_data.top_port,
                      m_data.mid_panel_ip.toString().c_str(),
                      m_data.mid_port,
                      m_data.btm_panel_ip.toString().c_str(),
                      m_data.btm_port,
                      m_data.flags) > 0;

bool MBConfig::writeHumanReadable(Stream &cfgStream) {
    return cfgStream.printf("Top: %s:%i\r\nMiddle: %s:%i\r\nBottom: %s:%i\r\nFlags: %i\r\n",
                            m_data.top_panel_ip.toString().c_str(),
                            m_data.top_port,
                            m_data.mid_panel_ip.toString().c_str(),
                            m_data.mid_port,
                            m_data.btm_panel_ip.toString().c_str(),
                            m_data.btm_port,
                            m_data.flags) > 0;
}

MBConfigData *MBConfig::fromString(char *str) {
    char *tokptr = NULL;
    tokptr = strtok(str, ":");
    if (tokptr == NULL || !m_data.top_panel_ip.fromString(tokptr)) return NULL;
    tokptr = strtok(str, "|");
    if (tokptr == NULL) return NULL;
    m_data.top_port = atoi(tokptr);
    tokptr = strtok(str, ":");
    if (tokptr == NULL || !m_data.mid_panel_ip.fromString(tokptr)) return NULL;
    tokptr = strtok(str, "|");
    if (tokptr == NULL) return NULL;
    m_data.mid_port = atoi(tokptr);
    tokptr = strtok(str, ":");
    if (tokptr == NULL || !m_data.btm_panel_ip.fromString(tokptr)) return NULL;
    tokptr = strtok(str, "|");
    if (tokptr == NULL) return NULL;
    m_data.btm_port = atoi(tokptr);
    tokptr = strtok(str, "|");
    if (tokptr == NULL) return NULL;
    m_data.flags = atoi(tokptr);
    return &m_data;
}

void MBConfig::setFlags(bool testMode) {
    m_data.flags = (testMode ? MBCONFIG_TEST_MODE : 0x0); // OR any other flags here
}

bool MBConfig::testMode() { return m_data.flags & MBCONFIG_TEST_MODE; }

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MBCONFIG)
MBConfig MOON_CONF;
#endif
