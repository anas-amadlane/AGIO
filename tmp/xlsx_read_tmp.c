#include <stdio.h>
#include <jansson.h>
#include <xlsxio_read.h>
#include <libft.h>
#include <string.h>
#include <sqlite3.h>
#define MAX_ALLOC 5000
#define C_QUE "DROP TABLE IF EXISTS %s CREATE TABLE %s (DATE_OP DATE, DEBIT BIGINT, CREDIT BIGINT);"
#define I_QUE "insert into %s values(\'"

extern const char *wd;
// wd = "/home/anas/clones/agios_cal/";


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
int	query_holder(char **query, char *s);
int init_db(const char *tab, const char *qu2, t_var *vars, t_rel *rel);
t_rel	*jsonparser(char *file, char *bq);

static int	init_import(char *file, char *bq, t_rel *releve, char *tab, t_var *vars) {
    int					i;
	int					j;
	int					r;
    int					col;
    int					row;
	const char			*sheetname;
	char				*value;
	xlsxioreader		table;
	xlsxioreadersheet	sheet;
	t_col				*holder;
	char				*query;
	char				buff[100];

	sheetname = NULL;
	if (!file || !releve)
		return 1;
	table = xlsxioread_open(file);
	if (!table)
		return 1;
	query = NULL;
	snprintf(buff, 100, I_QUE, tab);
	if (query_holder(&query, buff))
		return 1;
	i = 0;
	j = 0;
	holder = NULL;
	// while
	// printf("%s\n", releve->rel->colname);
	sheet = xlsxioread_sheet_open(table, sheetname, XLSXIOREAD_SKIP_NONE);
	if (sheet) {
		row = 0;
		while (xlsxioread_sheet_next_row(sheet)) {
			col = 0;
			holder = releve->col;
	        value = xlsxioread_sheet_next_cell(sheet);
	        while (value)
	        {
	            if (row > releve->skip[0] && holder && col == holder->index) {
					if (j < 2)
						date_form(&value, bq);
					else
						check_from(value, bq);
					if (i && !j)
						query_holder(&query, "\'),\n (\'");
					query_holder(&query, value);
					holder = holder->next;
					if (holder) {
						query_holder(&query, "\', \'");
						j++;
					}
					else
						j = 0;
					i++;
				}
				xlsxioread_free(value);
	            value = xlsxioread_sheet_next_cell(sheet);
				col++;
	        }
			if (!strcmp(bq, "AWB") && holder && j == 3)
				j = 0;
			row++;
		}
		query_holder(&query, "\');");
		tider(&query, releve->skip[1]);
		xlsxioread_sheet_close(sheet);
		init_db(tab, query, vars, releve);
	}
	free(query);
	xlsxioread_close(table);
	return 0;
}

int	import_file(t_var *vars) {
	t_rel	*releve;
	char	buff[50];
	char	path[50];

	path[0] ='\0';
	strcat(path, wd);
	strcat(path, "data/module.json");
	releve = jsonparser(path, vars->bq);
	if (!releve)
		return 1;
	snprintf(buff, 50, "%s_%d_%d", vars->bq, vars->year, vars->month);
	init_import(vars->file, vars->bq, releve, buff, vars);
}
