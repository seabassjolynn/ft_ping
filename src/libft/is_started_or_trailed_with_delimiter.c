#include "libft.h"

int is_started_or_trailed_with_delimiter(char *str, char delimiter)
{
    if (str[0] == delimiter)
    {
        return TRUE;
    }
    if (str[ft_strlen(str) - 1] == delimiter)
    {
        return TRUE;
    }
    return FALSE;
}