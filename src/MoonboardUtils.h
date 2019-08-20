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

class MoonboardUtils
{
public:
  MoonboardUtils(char *buf, uint16_t bufLen);
  bool readProblem(Problem *p, char *in);
  void addCatType(const char *catType);
  void selectCat(const char *catTypeName, uint8_t z_catNum);
  void selectCat(uint8_t z_catType, uint8_t z_catNum);
  void unselectCat(const char *catTypeName);
  void unselectCat(uint8_t z_catType);
  char *getSelectedCatName(uint8_t z_catType);
  char *catNumToName(uint8_t z_catType, uint8_t z_catNum);
  int8_t catTypeNameToNum(const char *catTypeName);
  int8_t catNameToNum(uint8_t z_catType, const char *catName);
  void getListNameFromCats(char *buf, uint8_t bufLen);
  void findLists();
  uint8_t openList(const char *listName, const char *sortOrder);
  void closeList();
  void readNextProblem(Problem *p);
protected:
  void beginCatType(char *catTypeName);
  void endCatType();
  void addCat(const char *catName);
private:
  // Internal variables
  char *listNames[MAX_LISTS]; // Pointers to the list names
  char wildcardStr[4] = MB_WILDCARD_STRING;
  char catBuf[CAT_BUF_SIZE] = ""; // Storage for category names
  char *catNames[MAX_CATS]; // Pointers to the category names
  uint8_t numCatNames = 0;
  uint8_t numCatTypes = 0;
  CategoryType m_catTypes[MAX_CAT_TYPES];
  File m_list;
  File m_data;
  char *m_buf;
  uint16_t m_bufLen;
  char listNameBuf[MAX_LISTS * MAX_LISTNAME_SIZE] = ""; // Storage for the list names
  char *t_catBufPtr = catBuf; // While setting up, point to where unused storage starts 
  uint8_t _t_uint8_t = 0;
  uint16_t _t_uint16_t = 0;
  int8_t _t_int8_t = 0;
  char *_t_ptr_char = NULL;
  char t_strtok[2]; // Token for strtok'ing
};

#endif // #ifndef _MOONBOARD_UTILS_H