#include "libft.h"

int has_repeated_delimeter(char *str, char delimiter)
{
    while(*str)
    {
        if (*str == delimiter && *(str + 1) == delimiter)
        {
            return FALSE;
        }
        str++;
    }
    return TRUE;
}