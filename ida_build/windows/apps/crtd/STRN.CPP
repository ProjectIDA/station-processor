#include <string.h>
#include "stdafx.h"

int strncasecmp(char *Str1, char *Str2, int n)
        {
        return _strnicmp(Str1,Str2,n);
        }
int strcasecmp(char *Str1, char *Str2)
        {
        return stricmp(Str1,Str2);
        }
