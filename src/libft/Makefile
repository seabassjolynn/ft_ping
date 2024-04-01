# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: sjolynn <sjolynn@student.21-school.ru      +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/03 17:16:27 by sjolynn           #+#    #+#              #
#    Updated: 2020/05/05 20:36:28 by sjolynn          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

SOURCES = ft_memset.c ft_bzero.c ft_memcpy.c ft_memccpy.c ft_memmove.c \
ft_memchr.c ft_memcmp.c ft_strlen.c ft_strlcpy.c ft_strlcat.c ft_strchr.c \
ft_strrchr.c ft_strnstr.c ft_strncmp.c ft_strncmp.c ft_atoi.c \
ft_isalpha.c ft_isdigit.c ft_isalnum.c ft_isascii.c ft_isprint.c \
ft_toupper.c ft_tolower.c ft_calloc.c ft_strdup.c ft_substr.c \
ft_strjoin.c ft_strtrim.c ft_split.c ft_itoa.c ft_strmapi.c ft_putchar_fd.c \
ft_putstr_fd.c ft_putendl_fd.c ft_putnbr_fd.c ft_print_error.c ft_malloc_or_exit.c \
ft_free_str_arr.c is_started_or_trailed_with_delimiter.c has_repeated_delimeter.c \
ptr_arr_len.c ft_atoi_unsigned_long_safe.c ft_atoi_unsigned_char_safe.c

OBJECT_FILES = ft_memset.o ft_bzero.o ft_memcpy.o ft_memccpy.o ft_memmove.o \
ft_memchr.o ft_memcmp.o ft_strlen.o ft_strlcpy.o ft_strlcat.o ft_strchr.o \
ft_strrchr.o ft_strnstr.o ft_strncmp.o ft_strncmp.o ft_atoi.o \
ft_isalpha.o ft_isdigit.o ft_isalnum.o ft_isascii.o ft_isprint.o \
ft_toupper.o ft_tolower.o ft_calloc.o ft_strdup.o ft_substr.o \
ft_strjoin.o ft_strtrim.o ft_split.o ft_itoa.o ft_strmapi.o ft_putchar_fd.o \
ft_putstr_fd.o ft_putendl_fd.o ft_putnbr_fd.o ft_print_error.o ft_malloc_or_exit.o \
ft_free_str_arr.o is_started_or_trailed_with_delimiter.o has_repeated_delimeter.o \
ptr_arr_len.o ft_atoi_unsigned_long_safe.o ft_atoi_unsigned_char_safe.o

all: $(NAME)

$(NAME): $(SOURCES) libft.h
	@ echo compile libft
	@ gcc -c -Wall -Wextra -Werror $(SOURCES) -I .
	@ echo archive libft
	@ ar rc $(NAME) $(OBJECT_FILES)

clean:
	/bin/rm -f $(OBJECT_FILES)

fclean: clean
	/bin/rm -f $(NAME)
re: fclean all

