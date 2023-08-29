#include <gtk/gtk.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <jansson.h>
#include <xlsxio_read.h>
// #include <libft.h>

#ifdef  IMPORT_GUI_C
#define APPNAME 5
// #define APPNAME 8
typedef struct  s_var
{
    char    *file;
    char    *bq;
    int     month;
    int		year;
}   t_var;

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

// typedef struct		s_date
// {
// 	int				date;
// 	struct s_date	*next;
// }					t_date;
// typedef struct		s_val
// {
// 	long long		val;
// 	struct s_val	*next;
// }					t_val;
// typedef struct		s_tab
// {
// 	t_date			*date_op;
// 	t_date			*date_va;
// 	t_val			*debit;
// 	t_val			*credit;
// 	t_val			*solde;
// }					t_tab;

// const char *wd = "/home/anas/clones/agios_cal/";
t_rel	*jsonparser(char *file, char *bq);
int	import_file(char *wd, t_var *vars);
int init_export(t_var *vars, char *trim, char *wd);
// int json_writer(char *bq, char *wd, int m, int y);
void	free_relve(t_rel *releve);
#endif

#ifdef JSON_TOOLS_C

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

// typedef struct		s_date
// {
// 	int				date;
// 	struct s_date	*next;
// }					t_date;
// typedef struct		s_val
// {
// 	long long		val;
// 	struct s_val	*next;
// }					t_val;
// typedef struct		s_tab
// {
// 	t_date			*date_op;
// 	t_date			*date_va;
// 	t_val			*debit;
// 	t_val			*credit;
// 	t_val			*solde;
// }					t_tab;
#endif