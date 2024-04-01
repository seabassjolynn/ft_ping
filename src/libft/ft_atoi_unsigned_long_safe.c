#include "libft.h"

#include <limits.h>
/*
	Function extracts unsigned long number from string and writes extracted number to result.
	It returns TRUE if number was extracted successfully i.e. no overflow occured, string contained
	only digits. Signs or alfanumeric characters are not allowed as both starting and trailing characters.
	E.g. +1 -> FALSE
		 -2 -> FALSE
		 a10 -> FALSE
		 10b -> FALSE 
*/
int ft_atoi_unsigned_long_safe(unsigned long *result, char *str)
{
	if (!str)
		return FALSE;
	
	*result = 0;
	
	while (*str && *str >= '0' && *str <= '9')
	{
		if (*result > ULONG_MAX / 10 || ((*result * 10 == ULLONG_MAX % 10) && ((unsigned long)(*str - '0') > ULONG_MAX % 10)))
            return FALSE;
        
        *result = (*result * 10) + (*str - '0');
		str++;
	}
	
    return !*str;
}