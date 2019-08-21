#ifndef _MOONBOARD_UTILS_H
#define _MOONBOARD_UTILS_H

#include "Data.h"
#include <SPIFFS.h>

#define MAX_LISTS 20
#define MAX_LISTNAME_SIZE 15
#define MAX_SORT_ORDERS 4
#define MAX_SORT_ORDER_LEN 8
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
  void addCatType(const char *catType);
  void setSortOrders(const char *sortOrderStr);

  void selectCat_ss(const char *catTypeName, const char *catName);
  void selectCat_is(int8_t z_catType, const char *catName);
  void selectCat_si(const char *catTypeName, int8_t z_catNum);
  void selectCat_ii(int8_t z_catType, int8_t z_catNum);
  void unselectCat_s(const char *catTypeName);
  void unselectCat_i(int8_t z_catType);

  char *getSelectedCatName(int8_t z_catType);
  void getSelectedListName(char *buf, int8_t bufLen);

  int8_t sortOrderToNum(const char *sortOrderName);
  bool sortOrderExists_s(const char *sortOrderName);
  bool sortOrderExists_i(int8_t z_sortOrder);

  int8_t catTypeToNum(const char *catTypeName);
  char *catNumToName(int8_t z_catType, int8_t z_catNum);
  int8_t catNameToNum(int8_t z_catType, const char *catName);

  void findLists();
  uint8_t openList(const char *listName, const char *sortOrder);
  uint8_t openSelectedList(const char *sortOrder);
  void closeList();

  bool readNextProblem(Problem *p);
  bool readProblem(Problem *p, char *in);
protected:
  void beginCatType(char *catTypeName);
  void endCatType();
  void addCat(const char *catName);
  void updateSortOrders();
private:
  // Internal variables
  char m_listNameBuf[MAX_LISTS * MAX_LISTNAME_SIZE] = ""; // Storage for the list names
  char *m_listNames[MAX_LISTS]; // Pointers to the list names

  char m_wildcardStr[4] = MB_WILDCARD_STRING;

  char m_catBuf[CAT_BUF_SIZE] = ""; // Storage for category names
  char *m_catNames[MAX_CATS]; // Pointers to the category names
  uint8_t m_numCatNames = 0;

  uint8_t m_numCatTypes = 0;
  CategoryType m_catTypes[MAX_CAT_TYPES];

  char m_sortOrderBuf[MAX_SORT_ORDERS * MAX_SORT_ORDER_LEN] = ""; // Storage for sort order names
  char *m_sortOrderNames[MAX_SORT_ORDERS]; // Pointers to the sort order names
  bool m_sortOrderExists[MAX_SORT_ORDERS]; // Does each type of index exist for the selected list?
  uint8_t m_numSortOrders = 0;

  File m_list;
  File m_data;
  char *m_buf;
  uint16_t m_bufLen;
  char *t_catBufPtr = m_catBuf; // While setting up, point to where unused storage starts 
  char *t_sortOrderBufPtr = m_sortOrderBuf; // While setting up, point to where unused storage starts 
  uint8_t _t_uint8_t = 0;
  int8_t _t_int8_t = 0;
  char *_t_ptr_char = NULL;
  char t_strtok[2]; // Token for strtok'ing
};

#endif // #ifndef _MOONBOARD_UTILS_H