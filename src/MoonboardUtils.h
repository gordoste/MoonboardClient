#ifndef _MOONBOARD_UTILS_H
#define _MOONBOARD_UTILS_H

#include "Data.h"
#include <SPIFFS.h>

#define MAX_LISTS 20
#define MAX_LISTNAME_SIZE 15
#define CAT_BUF_SIZE 256
#define MAX_CATS 20
#define MAX_CAT_TYPES 5

#ifndef MB_WILDCARD_STRING
#define MB_WILDCARD_STRING "Any"
#endif

#define MB_BUFFER_SIZE 255

class MoonboardUtils
{
public:
  MoonboardUtils() {};
  bool readProblem(Problem *p, char *in);
  void beginCatType(char *catType);
  void endCatType();
  void addCat(const char *catName);
  void defineCatType(const char *catType);
  void selectCat(uint8_t z_catType, uint8_t z_catNum);
  void unselectCat(uint8_t z_catType);
  void getListNameFromCats(char *buf, uint8_t bufLen);
  void findLists();
  void openList(const char *listName, const char *sortOrder);
  void closeList();
  void readNextProblem(Problem *p);
protected:
  // Internal variables
  char *listNames[MAX_LISTS]; // Pointers to the list names
  char wildcardStr[4] = MB_WILDCARD_STRING;
  char catBuf[CAT_BUF_SIZE] = ""; // Storage for category names
  char *catNames[MAX_CATS]; // Pointers to the category names
  uint8_t numCatNames = 0;
  uint8_t numCatTypes = 0;
  char *catTypeNames[MAX_CAT_TYPES];
  uint8_t catNameStartIdxPerType[MAX_CAT_TYPES]; // For each type, what index its list of names starts
  uint8_t catCountPerType[MAX_CAT_TYPES]; // For each type, how many categories
  int8_t selectedCatPerType[MAX_CAT_TYPES]; // For each type, which category is selected. 0-based. -1 = NONE
private:
  // TEMPORARY VARIABLES (USED FOR A WHILE BUT THEN WORTHLESS)
  char *t_catBufPtr = catBuf; // Point to unused storage
  File list;
  File data;
  char m_buf[MB_BUFFER_SIZE];
  uint8_t m_bufLen = MB_BUFFER_SIZE;
  char listNameBuf[MAX_LISTS * MAX_LISTNAME_SIZE] = ""; // Storage for the list names
};

#endif // #ifndef _MOONBOARD_UTILS_H