/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/11 13:35:28 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:19:33 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	rec_put(long int n, int fd);

void	ft_putnbr_fd(int n, int fd)
{
	if (n < 0)
	{
		ft_putchar_fd('-', fd);
		rec_put((long int)n * -1, fd);
	}
	else
	{
		rec_put(n, fd);
	}
}

static void	rec_put(long int n, int fd)
{
	if (n > 9)
	{
		rec_put((n / 10), fd);
		ft_putchar_fd((n % 10) + 48, fd);
	}
	else
		ft_putchar_fd(n + 48, fd);
}
