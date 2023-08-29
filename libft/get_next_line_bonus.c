/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aamadlan <aamadlan@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/07 13:09:59 by anas              #+#    #+#             */
/*   Updated: 2023/03/26 20:40:50 by aamadlan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define GET_NEXT_LINE_BONUS_C
#include "libft.h"

char	*get_next_line(int fd)
{
	static char	*buff[OPEN_MAX];

	if (fd >= 0 && fd < OPEN_MAX && BUFFER_SIZE > 0)
		return (get_the_line(fd, &buff[fd]));
	return (NULL);
}

static char	*get_the_line(int fd, char **buff)
{
	char	*tmp;
	char	*line;
	size_t	i;

	if (!ft_strchr(*buff, '\n'))
		tmp = fill_buff(fd, *buff);
	else
		tmp = *buff;
	*buff = NULL;
	if (ft_strchr(tmp, '\n'))
	{
		i = (ft_strchr(tmp, '\n') + 1) - tmp;
		if (ft_strlen(tmp) > i)
		{
			*buff = ft_substr(tmp, i, ft_strlen(tmp) - i);
			if (!*buff)
				return (ft_free(&tmp));
		}
		line = ft_substr(tmp, 0, i);
		if (!line)
			return (ft_free(buff), ft_free(&tmp), NULL);
		ft_free(&tmp);
		return (line);
	}
	return (tmp);
}

static char	*fill_buff(int fd, char *buff)
{
	char	*line;
	char	*tmp;
	ssize_t	i;

	tmp = buff;
	buff = (char *)malloc(sizeof(char) * (BUFFER_SIZE + 1ULL));
	if (!buff)
		return (ft_free(&tmp), NULL);
	i = read(fd, buff, BUFFER_SIZE);
	if (i < 0)
		return (ft_free(&buff), ft_free(&tmp));
	else if (i == 0)
		return (ft_free(&buff), tmp);
	buff[i] = '\0';
	if (tmp)
	{
		line = ft_strjoin(tmp, buff);
		ft_free(&tmp);
	}
	else
		line = ft_substr(buff, 0, ft_strlen(buff));
	if (!line)
		return (ft_free(&buff));
	return (ft_loop(fd, i, &line, &buff));
}

static char	*ft_loop(int fd, ssize_t i, char **line, char **buff)
{
	char	*tmp;

	while (i > 0 && !ft_strchr(*buff, '\n'))
	{
		i = read(fd, *buff, BUFFER_SIZE);
		if (i < 0)
			return (ft_free(buff), ft_free(line));
		(*buff)[i] = '\0';
		tmp = *line;
		*line = ft_strjoin(tmp, *buff);
		ft_free(&tmp);
		if (!*line)
			return (ft_free(buff));
	}
	return (ft_free(buff), *line);
}

static char	*ft_free(char **s)
{
	if (*s)
	{
		free(*s);
		*s = NULL;
	}
	return (NULL);
}
