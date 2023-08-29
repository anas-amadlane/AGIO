/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 14:26:51 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:16:20 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new)
{
	t_list	*p;

	if (*lst == NULL)
		*lst = new;
	else
	{
		p = *lst;
		while (p != NULL)
		{
			if (p->next == NULL)
				break ;
			p = p->next;
		}
		p->next = new;
	}
}
