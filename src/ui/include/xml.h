// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

#ifndef _XML_H_
#define _XML_H_

// ------------------------------------------------------
// Includes
#include "../../extralibs/tinyxml/tinyxml.h"
#include "../../extralibs/tinyxml/tinystr.h"

// ------------------------------------------------------
// Functions
int XML_Init(void);
const char *XML_get_string(char *section, char *key, char *name, char *value);
int XML_get_integer(char *section, char *key, char *name, char *value, int *dest);
const char *XML_get_value(char *tag, char *essemble, char *name, char *attr);

#endif
