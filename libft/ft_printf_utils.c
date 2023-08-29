/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/03/26 20:51:29 by aamadlan          #+#    #+#             */
/*   Updated: 2023/03/26 21:10:53 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int	sub_printf_c(char c)
{
	return (write(1, &c, 1));
}

int	sub_printf_xp(long unsigned int n, int i, int letr)
{
	if (n == 0 && i == 0)
		return (sub_printf_c(48));
	else
		i++;
	if (n != 0)
	{
		if ((n % 16) < 10)
			return (
				sub_printf_xp((n / 16), i, letr) + sub_printf_c((n % 16) + 48)
			);
		else
			return (
				sub_printf_xp((n / 16), i, letr) + sub_printf_c((n % 16) + letr)
			);
	}
	else
		return (0);
}

int	sub_printf_u(unsigned int n)
{
	if (n > 9)
		return (sub_printf_u((n / 10)) + sub_printf_c((n % 10) + 48));
	else
		return (sub_printf_c(n + 48));
}

int	sub_printf_x(unsigned int n, int i, int letr)
{
	if (n == 0 && i == 0)
		return (sub_printf_c(48));
	else
		i++;
	if (n != 0)
	{
		if ((n % 16) < 10)
		{
			return (
				sub_printf_x((n / 16), i, letr) + sub_printf_c((n % 16) + 48)
			);
		}
		else
		{
			return (
				sub_printf_x((n / 16), i, letr) + sub_printf_c((n % 16) + letr)
			);
		}
	}
	else
		return (0);
}
