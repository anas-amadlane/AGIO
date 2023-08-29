/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/07 09:11:19 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:19:04 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	if (dst == NULL && (len == 0 || src == NULL))
		return (NULL);
	if (dst == src)
		return (dst);
	if (dst > src)
	{
		while (len > 0)
		{
			len--;
			((char *)dst)[len] = ((char *)src)[len];
		}
	}
	else
		ft_memcpy(dst, src, len);
	return (dst);
}
