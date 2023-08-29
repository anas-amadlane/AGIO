/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/08 09:50:20 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:20:31 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strchr(const char *s, int c)
{
	size_t	i;
	size_t	j;
	char	k;

	if (!s)
		return (NULL);
	i = 0;
	j = ft_strlen(s);
	k = c;
	while (i <= j)
	{
		if (s[i] == k)
			return ((char *)s + i);
		i++;
	}
	return (NULL);
}
