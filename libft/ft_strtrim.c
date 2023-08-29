/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/09 18:05:17 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:25:32 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	first_index(char const *s1, char const *s2);
static int	last_index(char const *s1, char const *s2);

char	*ft_strtrim(char const *s1, char const *set)
{
	int		i;
	int		j;
	char	*p;

	if (s1 == NULL || set == NULL)
		return (0);
	i = first_index(s1, set);
	j = (last_index(s1, set) - i) + 1;
	if (j < 0)
		j = 0;
	p = ft_substr(s1, (unsigned int)i, (size_t)j);
	if (p == NULL)
		return (0);
	return (p);
}

static int	first_index(char const *s1, char const *s2)
{
	int	i;
	int	j;

	i = 0;
	while (s1[i])
	{
		j = 0;
		while (s2[j])
		{
			if (s1[i] == s2[j])
				break ;
			j++;
		}
		if (j == (int)ft_strlen(s2))
			break ;
		i++;
	}
	return (i);
}

static int	last_index(char const *s1, char const *s2)
{
	int	i;
	int	j;

	i = (int)ft_strlen(s1);
	while (i >= 0)
	{
		j = (int)ft_strlen(s2);
		while (j >= 0)
		{
			if (s1[i] == s2[j])
				break ;
			j--;
		}
		if (j < 0)
			break ;
		i--;
	}
	return (i);
}
