#include "MoonboardMenu.h"

char MoonboardStreamMenu::problemChoice(Problem probs[], uint8_t numProblems,
  const char *otherChoiceStrings[], const char *otherChoices) {
  while (1) {
    if (numProblems > 0) {
      for (_t_ui8 = 1; _t_ui8 <= numProblems; _t_ui8++) {
        problemAsString(&(probs[_t_ui8-1]), problemStr, sizeof(problemStr));
        m_stdout->printf("%d) %s\n", _t_ui8, problemStr);
      }
    }
    else {
      m_stdout->println(F("No problems available for selection."));
    }
    uint8_t numChoices = strlen(otherChoices);
    for (uint8_t i = 0; i < numChoices; i++) {
      m_stdout->printf("%c) %s\n", otherChoices[i], otherChoiceStrings[i]);
    }
    m_stdout->printf("> ");
    char c = m_stdin->timedRead();
    m_stdout->println(c);
    if (c > '0' && c <= '9' && c-'1' <= numProblems) {  // A number that's valid
      return c;        
    }
    if (c == 'x' || c == 'X') return -1;
    if (strchr(otherChoices, c)) return c; // Check if character is a valid selection
    if (c >= 'a' && c <= 'z' && strchr(otherChoices, c - 'a' + 'A')) return c; // if LC, check for UC version
    if (c >= 'A' && c <= 'Z' && strchr(otherChoices, c - 'A' + 'a')) return c; // if UC, check for LC version
    m_stdout->println(F("Invalid selection."));
  }
  return -1;
}

#ifdef TFT_ENABLED

char MoonboardTFTMenu::problemChoice(Problem probs[], uint8_t numProblems,
  const char *otherChoiceStrings[], const char *otherChoices) {
  return 0;
}

#endif //#ifdef TFT_ENABLED