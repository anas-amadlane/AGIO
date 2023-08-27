#include <jheader.h>

char	*formater(size_t len, char *form, ...) {
	int     i;
	int     j;
	char    *s;
	va_list var;

	if (!form)
		return (NULL);
	s = (char *)malloc(sizeof(char) * (++len));
	if (s) {
		i = -1;
		j = 0;
		va_start(var, form);
		while (form[++i]) {
			if (form[i] == '%' && form[i + 1] == 's') {
				ft_strlcpy(s + j, va_arg(var, char*), len - j);
				j = ft_strlen(s);
				i++;
			}
			else
				s[j++] = form[i];
		}
		s[j] = '\0';
		va_end(var);
	}
	return (s);
}

char	*perfect_join(char *s1, char *s2, char *sep) {
	size_t  ln1;
	size_t  ln2;
	size_t	lns;
	char    *ns;

	ns = NULL;
	if (!sep)
		sep = "";
	if (s1 && s2) {
		ln1 = ft_strlen(s1);
		ln2 = ft_strlen(s2);
		lns = ft_strlen(sep);
		ns = (char *)malloc(sizeof(char) * (ln1 + ln2 + lns + 1));
		if (!ns)
			return (NULL);
		ft_strlcpy(ns, s1, (ln1 + lns + ln2 + 1));
		ft_strlcpy((ns + ln1), sep, (lns + ln2 + 1));
		ft_strlcpy((ns + ln1 + lns), s2, (ln2 + 1));
		free(s1);
	}
	else if (s1 || s2) {
		if (s1)
			return (s1);
		else if (s2)
			return (ft_strdup(s2));
	}
	return (ns);
}

char	*convert_date(char *s)
{
	int			unixDate;
    time_t		rawTime;
    struct tm	*timeInfo;
	char		*rt;

    if (!s)
		return (NULL);
	rt = (char *)malloc(sizeof(char) * 11);
	if (!rt)
		return (NULL);
	unixDate = (ft_atoi(s) - 25569) * 86400;
	rawTime = (time_t)unixDate;
	timeInfo = gmtime(&rawTime);
	sprintf(rt, "%04u-%02u-%02u", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
	//sprintf(rt, "%02u/%02u/%04u", timeInfo->tm_mday, timeInfo->tm_mon + 1, timeInfo->tm_year + 1900);
	return (rt);
}

int	add_colist(t_col **colist, const char *col) {
	char	*tmp_str;
	t_col	*tmp_node;
	t_col	*holder;

	if (!col)
		return (1);
	tmp_node = (t_col *)malloc(sizeof(t_col));
	if (!tmp_node)
		return (1);
	tmp_str = ft_strdup(col);
	if (!tmp_str)
		return (free(tmp_node), 1);
	tmp_node->col_name = tmp_str;
	tmp_node->next = NULL;
	if (!*colist)
		*colist = tmp_node;
	else {
		holder = *colist;
		while (holder->next)
			holder = holder->next;
		holder->next = tmp_node;
	}
	return (0);
}

void	free_colist(t_col **colist) {
	t_col	*holder;

	if (!*colist)
		return ;
	while (*colist) {
		holder = *colist;
		*colist = holder->next;
		free(holder->col_name);
		free(holder);
	}
}

int	del_colist(t_col **colist, const char *col) {
	size_t	len;
	t_col	*holder;
	t_col	*tmp;

	if (!*colist || !col)
		return (1);
	len = ft_strlen(col);
	holder = *colist;
	if (ft_strlen(holder->col_name) == len && !ft_strncmp(holder->col_name, col, len))
	{
		*colist = holder->next;
		free(holder->col_name);
		free(holder);
		return (0);
	}
	tmp = holder;
	holder = holder->next;
	while (holder) {
		if (ft_strlen(holder->col_name) == len && !ft_strncmp(holder->col_name, col, len))
		{
			tmp->next = holder->next;
			free(holder->col_name);
			free(holder);
			return (0);
		}
		tmp = holder;
		holder = holder->next;
	}
	return (1);
}
