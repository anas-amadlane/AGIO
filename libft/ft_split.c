/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/07 19:00:43 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:20:05 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static char	**split_suite(int j, char *c, char *tmp, char **ret)
{
	int	i;

	i = -1;
	while (++i < j && ft_strchr(tmp, c[0]))
	{
		ret[i] = ft_substr(tmp, 0, ft_strchr(tmp, c[0]) - tmp);
		ret[i + 1] = ft_substr(tmp, ft_strchr(tmp, c[0]) - tmp,
				(&tmp[ft_strlen(tmp) - 1] - ft_strchr(tmp, c[0])) + 1);
		free(tmp);
		tmp = ft_strtrim(ret[i + 1], c);
		free(ret[i + 1]);
		if (!ret[i] || !ret[i + 1] || !tmp)
		{
			while (i >= 0)
				free(ret[i--]);
			return (free(ret), free(tmp), NULL);
		}
	}
	ret[i] = tmp;
	ret[i + 1] = NULL;
	return (ret);
}

char	**ft_split(char const *s, char c)
{
	int		i;
	int		j;
	char	**ret;
	char	*tmp;
	char	c2[2];

	if (!s)
		return (NULL);
	c2[0] = c;
	c2[1] = '\0';
	tmp = ft_strtrim(s, c2);
	if (!tmp)
		return (NULL);
	i = -1;
	j = 1;
	while (s[++i])
	{
		while (s[i] && s[i] == c)
			i++;
		j++;
	}
	ret = (char **)malloc(sizeof(char *) * (j + 1));
	if (!ret)
		return (NULL);
	return (split_suite(j, c2, tmp, ret));
}
