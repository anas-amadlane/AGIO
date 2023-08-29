/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/09 17:09:52 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:25:54 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*p;
	size_t	i;

	if (s == NULL)
		return (NULL);
	if (ft_strlen(s) < start)
		p = (char *)malloc(1 * sizeof(char));
	else if ((ft_strlen(s) - start) < len)
		p = (char *)malloc(((ft_strlen(s) - start) + 1) * sizeof(char));
	else
		p = (char *)malloc((len + 1) * sizeof(char));
	if (p == NULL)
		return (NULL);
	i = 0;
	while (i < len && start < ft_strlen(s))
	{
		p[i] = s[start];
		i++;
		start++;
	}
	p[i] = '\0';
	return (p);
}
