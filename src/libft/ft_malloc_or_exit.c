#include <stdlib.h>
#include <unistd.h>
#include "libft.h"

void    *ft_malloc_or_exit_error(size_t len) {
    void *ptr = malloc(len);
    if (ptr == NULL) {
        ft_print_error("Failed to allocate memory.");  
        exit(-1);
    }
    return ptr;
}