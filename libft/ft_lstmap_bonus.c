/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 16:19:45 by aamadlan          #+#    #+#             */
/*   Updated: 2023/02/17 16:17:04 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*p;
	t_list	*l;

	if (lst == NULL || f == NULL || del == NULL)
		return (0);
	p = NULL;
	while (lst != NULL)
	{
		l = ft_lstnew(f(lst->content));
		if (l == NULL)
		{
			ft_lstclear(&p, del);
			p = NULL;
			return (p);
		}
		ft_lstadd_back(&p, l);
		lst = lst->next;
	}
	return (p);
}
