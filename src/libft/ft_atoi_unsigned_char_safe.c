#include "libft.h"

#include <limits.h>
/*
		Function extracts unsigned char number from string and writes extracted number to result.
	It returns TRUE if number was extracted successfully i.e. no overflow occured, string contained
	only digits. Signs or alfanumeric or space characters are not allowed as both starting and trailing characters.
	Exampeles of error caces: 
		+1 -> FALSE
		-2 -> FALSE
		a10 -> FALSE
		10b -> FALSE 
*/
int ft_atoi_unsigned_char_safe(unsigned char *result, char *str)
{
	if (!str)
		return FALSE;
	
	int result_tmp = 0;
	
	while (*str && *str >= '0' && *str <= '9')
	{
        result_tmp = (result_tmp * 10) + (*str - '0');
		if (result_tmp > UCHAR_MAX)
			return FALSE;
		str++;
	}
	*result = result_tmp;
    return !*str;
}