#include <stdio.h>
#include <jansson.h>
#include <xlsxio_read.h>
#include <libft.h>
#include <string.h>
#include <sqlite3.h>
//#define MAX_ALLOC 5000
//#define C_QUE "DROP TABLE IF EXISTS %s CREATE TABLE %s (DATE_OP DATE, DEBIT BIGINT, CREDIT BIGINT);"
//#define I_QUE "insert into %s values(\'"

#define MAX_ALLOC 10000
#define HEADER {{"Date d'opération", "Date opération", "Date Opération", "DATE OPERATION", "Date de la transaction", "Date comptable", "DT opération", "Date opérati", "DATE OPER", "DATE", "Date", NULL}, {"Débit (DH)", "Débit", "DEBIT", " Montant débit", "Montant débit", "debit", "débit", "Vous avez séléctionné Montant de Débit", NULL}, {"Crédit (DH)", "Crédit", "CREDIT", " Montant crédit", "Montant crédit", "crédit", "credit", NULL}, NULL}
#define I_QUE "insert into %s values"

// extern t_envlop envlop;
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


// int	tider(char **query, int x);
// int	rep_char(char *s, char c1, char c2);
// int	rm_char(char *s, char c);
// int	check_from(char *s, char *bq);
// int	date_mode(char *s);
// int	date_conv(char **s);
// int	date_form(char **s, char *bq);
// int	query_holder(char **query, char *s);


int	query_holder(char **query, char *s);
int init_db(const char *tab, const char *qu2, t_var *vars);//, t_rel *rel);
t_rel	*jsonparser(char *file, char *bq);

// Checkers
int iswhitespace(char c) {
    if ((c > 8 && c < 14) || c == 32)
        return 0;
    else
        return 1;
}
int isnum (char c) {
    if (c > 47 && c < 58)
        return 0;
    else
        return 1;
}
int isallnum(const char *s) {
    int i;

    if (!s)
        return -1;
    i = -1;
    while (s[++i]) {
        if (isnum(s[i]))
            return 1;
    }
    return 0;
}
int isspecial(char c) {
    if (c == 45 || c == 47 || c == 92)
        return 0;
    else
        return 1;
}
int is_empty(const char *s) {
    int i;

    if (!s)
        return -1;
    i = -1;
    while (s[++i]) {
        if (iswhitespace(s[i]))
            return 1;
    }
    return 0;
}
int is_date(const char *s) {
    int i;

    if (!s || !is_empty(s))
        return -1;
    i = -1;
    while (s[++i]) {
        if (iswhitespace(s[i]) && isnum(s[i]) && isspecial(s[i]))
            return 1;
    }
    return 0;
}
// Checkers

int pars_date(const char *s, char *ns) {
    char        d[3], m[3], y[5];
    int         unixDate;
    time_t      rawTime;
    struct tm   *timeInfo;
	char        *rt;
    int         i;

    if (!s || !ns)
        return 1;
    bzero(ns, 11);
    if (isallnum(s)) {
        i = -1;
        while (s[++i]) {
            if (i < 2)
                d[i] = s[i];
            else if (i > 2 && i < 5)
                m[i - 3] = s[i];
            else if (i > 5)
                y[i - 6] = s[i];
        }
        (d[2] = '\0', m[2] = '\0', y[4] = '\0');
        i = -1;
        while (++i < 10) {
            if (i < 4)
                ns[i] = y[i];
            else if (i > 4 && i < 7)
                ns[i] = m[i - 5];
            else if (i > 7)
                ns[i] = d[i - 8];
            else
                ns[i] = '-';
        }
        ns[10] = '\0';
    }
    else {
        unixDate = (atoi(s) - 25569) * 86400;
        rawTime = (time_t)unixDate;
        timeInfo = gmtime(&rawTime);
        snprintf(ns, 11, "%04u-%02u-%02u", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
    }
    return 0;
}
int pars_mnt(const char *s, char *ns) {
    int i, j;

    if (!s || !ns)
        return 1;
    (i = -1, j = 0);
    bzero(ns, 100);
    if (!is_empty(s)) {
        (ns[0] = '0', ns[1] = '\0');
    }
    else if (strchr(s, ' ') && strchr(s, ',')) {
        while (s[++i]) {
            if (s[i] != ' ' && s[i] != ',' && s[i] != '-')
                ns[j++] = s[i];
            else if (s[i] == ',')
                ns[j++] = '.';
        }
        ns[j] = '\0';
    }
    else if (strchr(s, ',') && strchr(s, '.')) {
        while (s[++i]) {
            if (s[i] != ',' && s[i] != '-')
                ns[j++] = s[i];
        }
        ns[j] = '\0';
    }
    else if (strchr(s, ',')) {
        while (s[++i]) {
            if (s[i] != ',' && s[i] != '-')
                ns[j++] = s[i];
            else if (s[i] == ',')
                ns[j++] = '.';
        }
        ns[j] = '\0';
    }
    else if (strchr(s, '-')) {
        while (s[++i]) {
            if (s[i] != '-')
                ns[j++] = s[i];
        }
        ns[j] = '\0';
    }
    else
        return 1;
    return 0;
}


int comp_head(const char *val, int i) {
    int j;
    char    *head[4][12] = HEADER;
    if (!val)
        return 1;
    j = -1;
    while (head[i][++j]) {
        if (!strcmp(val, head[i][j]))
            return 0;
    }
    return 1;
}
int get_index_by_col(xlsxioreadersheet sheet, int *index) {
    const char          *value;
    int                 i;
    int                 col;
    int                 row;

    if (!sheet || !index)
        return 1;
    i = 0;
    row = 0;
    while (i < 3 && xlsxioread_sheet_next_row(sheet)) {
        col = 0;
        value = xlsxioread_sheet_next_cell(sheet);
        while (i < 3 && value) {
            if (!comp_head(value, i)) {
                index[i++] = col;
            }
            free((char *)value);
            value = xlsxioread_sheet_next_cell(sheet);
            col++;
        }
        row++;
    }
    if (i < 3)
        return 1;
    return 0;
}
int get_rows(xlsxioreadersheet sheet, char **query, int *index, char *table) {
    const char  *value;
    char        buff[100];
    char        date[11];
    char        mnt[100];
    int         i;
    int         col;
    int         row;
	int			w;

    if (!sheet || !query || !index || !table)
        return 1;
    bzero(buff, 100);
	*query = NULL;
    snprintf(buff, 100, I_QUE, table);
    if (query_holder(query, buff))
        return 1;
    row = 0;
	w = 0;
    while (xlsxioread_sheet_next_row(sheet)) {
        i = 0;
        col = 0;
        value = xlsxioread_sheet_next_cell(sheet);
        while (value) {
            if (i < 3 && index[i] == col) {
                if (i == 0 && is_date(value))
                    i = 4;
                else {
                    if (w > 0 && i == 0)
                        query_holder(query, "), \n");
                    if (i == 0 && !pars_date(value, date)) {
                        snprintf(buff, 100, "('%s'", date);
                        query_holder(query, buff);
                    }
                    else {
                        if (!pars_mnt(value, mnt))
                            snprintf(buff, 100, ", %s", mnt);
                        else
                            snprintf(buff, 100, ", %s", value);
                        query_holder(query, buff);
                    }
                    i++;
                }
				if (i == 3)
					w++;
            }
            free((char *)value);
            value = xlsxioread_sheet_next_cell(sheet);
            col++;
        }
        row++;
    }
    query_holder(query, ");");
    return 0;
}
int import_xlsx(t_var *vars, char *table) {
    xlsxioreader        workbook;
    xlsxioreadersheet   sheet;
    char                *query;
	int					index[3];

    if (!vars || !table)
        return 1;
    workbook = xlsxioread_open(vars->file);
    if (!workbook)
        return (printf("Error opening XLSX file.\n"), 1);
    sheet = xlsxioread_sheet_open(workbook, NULL, XLSXIOREAD_SKIP_EMPTY_ROWS);
    if (!sheet)
        return (xlsxioread_close(workbook), printf("Error opening sheet in XLSX file.\n"), 1);
    if (get_index_by_col(sheet, index))
        return (xlsxioread_sheet_close(sheet), xlsxioread_close(workbook), printf("Incompatible table.\n"), 1);
    if (get_rows(sheet, &query, index, table)) {
        xlsxioread_sheet_close(sheet);
        xlsxioread_close(workbook);
        return 1;
    }
    xlsxioread_sheet_close(sheet);
    xlsxioread_close(workbook);
    if (init_db(table, query, vars))
        return(free(query), 1);
    free(query);
    return 0;
}
int	import_file(char *wd, t_var *vars) {
	char	buff[50];

	snprintf(buff, 50, "%s_%d_%d", vars->bq, vars->year, vars->month);
    return (import_xlsx(vars, buff));
}

int	query_holder(char **query, char *s)
{
	int			i;
	int			l;
	static int	len;
	char		*tmp;

	if (*query && s)
	{
		l = strlen(s);
		if (len + l < MAX_ALLOC)
		{
			i = -1;
			while (s[++i])
				(*query)[len++] = s[i];
			(*query)[len] = '\0';
		}
		else
		{
			len += l;
			tmp = *query;
			*query = (char *)malloc(sizeof(char) * (len + 1));
			if (!(*query))
				return (1);
			i = -1;
			while (tmp[++i])
				(*query)[i] = tmp[i];
			l = -1;
			while (s[++l])
				(*query)[i++] = s[l];
			(*query)[i] = '\0';
			free(tmp);
		}
	}
	else if (s)
	{
		len = strlen(s);
		*query = (char *)malloc(sizeof(char) * MAX_ALLOC);
		if (!(*query))
			return (1);
		i = -1;
		while (s[++i])
			(*query)[i] = s[i];
		(*query)[i] = '\0';
	}
	else
		return (1);
	return (0);
}