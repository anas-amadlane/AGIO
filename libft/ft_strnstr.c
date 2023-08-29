/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/09 11:11:21 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:25:08 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *ss, const char *sf, size_t len)
{
	size_t	i;
	size_t	j;

	if (ss == NULL && len == 0)
		return (0);
	else if (ft_strlen(sf) == 0)
		return ((char *)ss);
	i = 0;
	while (ss[i] && i < len)
	{
		if (ss[i++] == sf[0])
		{
			j = 0;
			while (sf[j])
			{
				if (ss[(i - 1) + j] == sf[j])
					j++;
				else
					break ;
			}
			if (j == ft_strlen(sf) && (i - 1) + j <= len)
				return ((char *)ss + (i - 1));
		}
	}
	return (0);
}
