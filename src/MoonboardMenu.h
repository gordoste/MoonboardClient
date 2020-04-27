#ifndef _MOONBOARD_MENU_H
#define _MOONBOARD_MENU_H

#include "Data.h"
#include "Menu.h"

class MoonboardStreamMenu : public StreamMenu {
  public:
    char problemChoice(Problem probs[], uint8_t numProblems,
      const char *otherChoiceStrings[], const char *otherChoices,
      char selected = '\0');
  protected:
    uint8_t _t_ui8;
    char problemStr[80];
};

#endif //#ifndef _MOONBOARD_MENU_H