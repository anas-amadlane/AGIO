/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/09 13:35:56 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:13:58 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	sub_atoi(const char *str, int i, int j, int s);

int	ft_atoi(const char *str)
{
	int	i;
	int	j;
	int	s;

	i = 0;
	s = 1;
	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	j = i;
	while (str[i] == 43 || str[i] == 45)
		i++;
	if (i - j > 1)
		return (0);
	if (str[j] == 45)
		s *= -1;
	j = i;
	while (str[i] >= 48 && str[i] <= 57)
		i++;
	return (sub_atoi(str, i, j, s));
}

static int	sub_atoi(const char *str, int i, int j, int s)
{
	unsigned long int	k;

	k = 0;
	while (j < i)
	{
		k = (k * 10) + str[j] - 48;
		if (s == -1 && k > 9223372036854775807)
			return (0);
		else if (k > 9223372036854775807)
			return (-1);
		j++;
	}
	return ((int)k * s);
}
