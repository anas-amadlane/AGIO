/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstclear_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/18 11:48:28 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:16:36 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_lstclear(t_list **lst, void (*del)(void *))
{
	t_list	*p;

	if (lst == NULL || del == NULL)
		return ;
	p = *lst;
	while (p != NULL)
	{
		p = (*lst)->next;
		del((*lst)->content);
		free(*lst);
		*lst = p;
	}
	lst = NULL;
}
