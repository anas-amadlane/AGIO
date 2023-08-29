/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/08 10:39:19 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:25:19 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	size_t	i;
	char	j;

	if (!s)
		return (NULL);
	i = ft_strlen(s) + 1;
	j = c;
	while (i > 0)
	{
		i--;
		if (s[i] == j)
			return ((char *)s + i);
	}
	return (NULL);
}
