#include <stdio.h>

size_t  ptr_arr_len(char **ptr_arr)
{
    int count = 0;
   
    while (*ptr_arr)
    {
        count++;
        ptr_arr++;
    }
    return count;
}