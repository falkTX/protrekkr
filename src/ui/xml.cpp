// ------------------------------------------------------
// Protrekkr
// Written by Franck Charlet
// Based on the work of Juan Antonio Arguelles Rius 
// ------------------------------------------------------

// ------------------------------------------------------
// Includes
#include <SDL/SDL_stdinc.h>
#include "../include/main.h"

// ------------------------------------------------------
// Variables
list <skin_file> skin_files;

// Entities:
// - Background.
// - Button.
// - Static.
// - Container (black).
// - Vertical scrollbar.
// - Horizontal scrollbar.
// - Slider.
// - Textbox.
// - Spin (2 arrows with a number).
// - Picture.

TiXmlDocument doc("skins/skin.xml");
char xml_error[256];

extern char Window_Title[256];

// ------------------------------------------------------
// Functions
const char *XML_get_value(TiXmlHandle *handle, char *essemble, char *name);

void Print_Xml_Error(void)
{
    printf("XML (%d): %s\n", doc.ErrorId(), doc.ErrorDesc());
}

int XML_Init(void)
{
    if(!doc.LoadFile())
    {
        Print_Xml_Error();
        return(FALSE);
    }
    return(TRUE);
}

const char *XML_get_string(char *section, char *key, char *name, char *value)
{
    return(XML_get_value(section, key, name, value));
}

int XML_get_integer(char *section, char *key, char *name, char *value, int *dest)
{
    const char *dat = XML_get_string(section, key, name, value);
    if(!dat) return(FALSE);
    if(dat[0] == '#')
    {
        sscanf(&dat[1], "%x", dest);
    }
    else
    {
        sscanf(dat, "%d", dest);
    }
    return(TRUE);
}

const char *XML_get_value(char *tag, char *essemble, char *name, char *attr)
{
    TiXmlHandle *files;
    TiXmlHandle *file;
    TiXmlElement *element;
    TiXmlHandle hdl(&doc);
    const char *value;

    files = &hdl.FirstChildElement(tag);
    if(!files) return(NULL);

    file = &files->FirstChildElement(essemble);
    if(!file) return(NULL);

    element = file->Element();
    while(element)
    {
        if(SDL_strcasecmp(element->Attribute("name"), name) == 0)
        {
            value = element->Attribute(attr);
            while(*value == ' ' || *value == '\t')
            {
                *value++;
            }
            return(value);
        }
        element = element->NextSiblingElement();
    }
    return(NULL);
}
