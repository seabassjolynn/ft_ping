#include <stdlib.h>

void ft_free_str_arr(char **str_arr)
{
    int i = 0;
    while (str_arr[i])
    {
        free(str_arr[i]);
        i++;
    }
    free(str_arr);
}