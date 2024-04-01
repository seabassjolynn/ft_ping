/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strjoin.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mac <sjolynn@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/05/17 17:35:42 by mac               #+#    #+#             */
/*   Updated: 2020/05/20 20:43:49 by mac              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include <stdlib.h>

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	s1_len;
	size_t	s2_len;
	char	*joined_str;

	if (s1 == 0 || s2 == 0)
		return (0);
	s1_len = ft_strlen(s1);
	s2_len = ft_strlen(s2);
	joined_str = (char*)malloc((s1_len + s2_len + 1) * sizeof(char));
	if (joined_str)
	{
		ft_strlcpy(joined_str, s1, s1_len + 1);
		ft_strlcat(joined_str, s2, s1_len + s2_len + 1);
		return (joined_str);
	}
	else
	{
		return (0);
	}
}
