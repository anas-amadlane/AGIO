/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstlast_bonus.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 14:17:14 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:16:59 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstlast(t_list *lst)
{
	t_list	*p;

	p = lst;
	while (p != NULL)
	{
		if (p->next == NULL)
			break ;
		p = p->next;
	}
	return (p);
}
