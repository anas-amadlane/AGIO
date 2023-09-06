#include <stdio.h>
#include <jansson.h>
#include <xlsxio_read.h>
// #include <libft.h>
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

// int	query_holder(char **query, char *s)
// {
// 	int			i;
// 	int			l;
// 	static int	len;
// 	char		*tmp;

// 	if (*query && s)
// 	{
// 		l = ft_strlen(s);
// 		if (len + l < MAX_ALLOC)
// 		{
// 			i = -1;
// 			while (s[++i])
// 				(*query)[len++] = s[i];
// 			(*query)[len] = '\0';
// 		}
// 		else
// 		{
// 			len += l;
// 			tmp = *query;
// 			*query = (char *)malloc(sizeof(char) * (len + 1));
// 			if (!(*query))
// 				return (1);
// 			i = -1;
// 			while (tmp[++i])
// 				(*query)[i] = tmp[i];
// 			l = -1;
// 			while (s[++l])
// 				(*query)[i++] = s[l];
// 			(*query)[i] = '\0';
// 			free(tmp);
// 		}
// 	}
// 	else if (s)
// 	{
// 		len = ft_strlen(s);
// 		*query = (char *)malloc(sizeof(char) * MAX_ALLOC);
// 		if (!(*query))
// 			return (1);
// 		i = -1;
// 		while (s[++i])
// 			(*query)[i] = s[i];
// 		(*query)[i] = '\0';
// 	}
// 	else
// 		return (1);
// 	return (0);
// }

// CREATE VIEW IF NOT EXISTS VIEW_%s AS
// SELECT date(DATE_OP) as stat_day, SUM(DEBIT), SUM(CREDIT), (SUM(CREDIT) - SUM(DEBIT))
// from %s GROUP BY date(DATE_OP) order by stat_day;

