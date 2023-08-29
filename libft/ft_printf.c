/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/24 13:34:21 by aamadlan          #+#    #+#             */
/*   Updated: 2023/03/26 21:10:59 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define FT_PRINTF_C
#include "libft.h"

int	ft_printf(const char *f, ...)
{
	va_list	ag;
	int		i;
	int		j;

	va_start(ag, f);
	i = 0;
	j = 0;
	while (f[i])
	{
		if (f[i] == '%')
			j += spfy_printf(f[++i], ag);
		else
			j += sub_printf_c(f[i]);
		if (f[i] != '\0')
			i++;
	}
	va_end(ag);
	return (j);
}

static int	spfy_printf(char c, va_list ag)
{
	if (c == 'c')
		return (sub_printf_c(va_arg(ag, int)));
	else if (c == '%')
		return (sub_printf_c('%'));
	else if (c == 's')
		return (sub_printf_s(va_arg(ag, char *)));
	else if (c == 'i' || c == 'd')
		return (sub_printf_n(va_arg(ag, int)));
	else if (c == 'u')
		return (sub_printf_u(va_arg(ag, unsigned int)));
	else if (c == 'x')
		return (sub_printf_x(va_arg(ag, unsigned int), 0, 87));
	else if (c == 'X')
		return (sub_printf_x(va_arg(ag, unsigned int), 0, 55));
	else if (c == 'p')
		return (sub_printf_p(va_arg(ag, void *)));
	return (0);
}

static int	sub_printf_p(void *n)
{
	return (write(1, "0x", 2) + sub_printf_xp((long unsigned int)n, 0, 87));
}

static int	sub_printf_n(int n)
{
	if (n < 0)
	{
		sub_printf_c('-');
		return (1 + sub_printf_u(n * -1));
	}
	else
		return (sub_printf_u(n));
}

static int	sub_printf_s(char *s)
{
	int	i;

	if (s == NULL)
		return (write(1, "(null)", 6));
	i = 0;
	while (s[i])
		sub_printf_c(s[i++]);
	return (i);
}
