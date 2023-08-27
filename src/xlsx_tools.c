#include <stdio.h>
#include <jansson.h>
#include <xlsxio_read.h>
#include <libft.h>
#include <string.h>
#include <sqlite3.h>
#define MAX_ALLOC 5000
#define C_QUE "DROP TABLE IF EXISTS %s_%s_%s CREATE TABLE %s_%s_%s (DATE_OP DATE, DATE_VA DATE, DEBIT BIGINT, CREDIT BIGINT);"

typedef struct  s_col
{
	int             index;
	char            *colname;
	struct s_col   *next;
}   t_col;

typedef struct	s_var_date
{
	double	v;
	char	*d_range[2];
}				t_var_date;

typedef struct  s_rel
{
	char		**head;
	double		solde;
	int			month;
	int			year;
	t_var_date	line;
	t_var_date	tx_reg;
	t_var_date	tx_pla;
	t_col		*col;
	int			skip[2];
}   t_rel;

typedef struct		s_date
{
	int				date;
	struct s_date	*next;
}					t_date;
typedef struct		s_val
{
	long long		val;
	struct s_val	*next;
}					t_val;
typedef struct		s_tab
{
	t_date			*date_op;
	t_date			*date_va;
	t_val			*debit;
	t_val			*credit;
	t_val			*solde;
}					t_tab;


int	tider(char **query, int x);
int	rep_char(char *s, char c1, char c2);
int	rm_char(char *s, char c);
int	check_from(char *s, char *bq);
int	date_mode(char *s);
int	date_conv(char **s);
int	date_form(char **s, char *bq);

int	tider(char **query, int x) {
	int		i;
	char	*tmp;

	if (!x || !query || !*query)
		return 1;
	tmp = *query;
	i = -1;
	while (tmp[++i]);
	while (x && tmp[--i]) {
		if (tmp[i] == ',' && tmp[i - 1] == ')') {
			tmp[i] = ';';
			tmp[i + 1] = '\0';
			x--;
		}
	}
	return 0;
}

int	rep_char(char *s, char c1, char c2) {
	int	i;

	if (!s)
		return 1;
	i = -1;
	while (s[++i]) {
		if (s[i] == c1)
			s[i] = c2;
	}
	return 0;
}

int	rm_char(char *s, char c) {
	int	i;
	int j;

	if (!s)
		return 1;
	i = -1;
	while (s[++i]) {
		if (s[i] == c) {
			j = i;
			while (s[j] && s[j + 1]) {
				s[j] = s[j + 1];
				j++;
			}
			s[j] = '\0';
		}
	}
	return 0;
}

int	check_from(char *s, char *bq) {
	if (!s || !bq)
		return 1;
	if (!strcmp(bq, "CIH")) {
		rm_char(s, ' ');
		rep_char(s, ',', '.');
	}
	else if (!strcmp(bq, "BMCI")) {
		rm_char(s, ' ');
		rep_char(s, ',', '.');
	}
	else if (!strcmp(bq, "ARB")) {
		rm_char(s, ',');
		rm_char(s, '-');
	}
	else if (!strcmp(bq, "SGMB"))
		rm_char(s, '-');
	else if (!strcmp(bq, "BMCE"))
		rm_char(s, '-');
	else
		return 1;
	return 0;
}

int	date_mode(char *s) {
	char	buff[20];
	int		i;
	int		k;
	int		j;

	if (!s)
		return 1;
	i = -1;
	while (s[++i])
		buff[i] = s[i];
	buff[i] = '\0';
	if (i < 10)
		return 1;
	j = i - 4;
	k = 0;
	while (j < i)
		s[k++] = buff[j++];
	s[k++] = '-';
	j = i - 7;
	while (j < i - 5)
		s[k++] = buff[j++];
	s[k++] = '-';
	j = i - 10;
	while (j < i - 8)
		s[k++] = buff[j++];
	s[k] = '\0';
	return 0;
}

int	date_conv(char **s)
{
	int			unixDate;
    time_t		rawTime;
    struct tm	*timeInfo;
	char		*rt;

    if (!s || !*s)
		return 1;
	rt = (char *)malloc(sizeof(char) * 11);
	if (!rt)
		return 1;
	unixDate = (ft_atoi(*s) - 25569) * 86400;
	rawTime = (time_t)unixDate;
	timeInfo = gmtime(&rawTime);
	sprintf(rt, "%04u-%02u-%02u", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
	xlsxioread_free(*s);
	*s = rt;
	return 0;
}

int	date_form(char **s, char *bq) {
	if (!s || !bq)
		return 1;
	if (!strcmp(bq, "BMCI") || !strcmp(bq, "ARB"))
		return date_mode(*s);
	else
		return date_conv(s);
}
