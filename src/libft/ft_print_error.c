#include <unistd.h>
#include "libft.h"

size_t ft_print_error(char str[]) {
    char *str_out = ft_strjoin(str, "\n");
    return write(STDERR_FILENO, str_out, ft_strlen(str_out));
}