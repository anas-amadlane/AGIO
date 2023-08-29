/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/10 13:43:11 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:30:01 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	nbr_len(long int n);
static void	nbr_cpy(char *s, long int n, int i);

char	*ft_itoa(int n)
{
	char	*c;
	int		i;

	i = nbr_len(n);
	c = (char *)malloc((i + 1) * sizeof(char));
	if (!c)
		return (NULL);
	if (n < 0)
	{
		c[0] = '-';
		nbr_cpy(c, ((long int)n * -1), (i - 1));
	}
	else
		nbr_cpy(c, n, (i - 1));
	c[i] = '\0';
	return (c);
}

static int	nbr_len(long int n)
{	
	if (n < 0)
		return (nbr_len(n * -1) + 1);
	else if (n > 9)
		return (nbr_len(n / 10) + 1);
	return (1);
}

static void	nbr_cpy(char *s, long int n, int i)
{
	if (n > 9)
	{
		s[i] = (n % 10) + 48;
		nbr_cpy(s, (n / 10), i - 1);
	}
	else
		s[i] = n + 48;
}
