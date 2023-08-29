#include <sqlite3.h>
#include <xlsxwriter.h>

#define TRIM1 " WHERE DATE BETWEEN '%d-01-01' AND '%d-03-31';"
#define TRIM2 " WHERE DATE BETWEEN '%d-04-01' AND '%d-06-30';"
#define TRIM3 " WHERE DATE BETWEEN '%d-07-01' AND '%d-09-30';"
#define TRIM4 " WHERE DATE BETWEEN '%d-10-01' AND '%d-12-31';"
#define NB_DEB "CREATE VIEW IF NOT EXISTS NB_DEB_%s AS WITH NB_DEB_%s AS (SELECT DATE(S_DATE) AS DATE, CASE WHEN BALANCE >= 0 THEN BALANCE ELSE 0 END AS NB_CRE, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) THEN -1 * BALANCE ELSE (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) END AS NB_DEB_REG, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) THEN 0 ELSE -1 * BALANCE - (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) END AS NB_DEB_PLA FROM CONC_OP_%s_%d) SELECT DATE, NB_CRE, NB_DEB_REG, NB_DEB_PLA, ((NB_DEB_REG * (SELECT TAUX FROM TREG_%s WHERE EDATE>DATE)/100)/360 + (NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s WHERE EDATE>DATE)/100)/360) AS INTER FROM NB_DEB_%s;"

typedef struct  s_var
{
    char    *file;
    char    *bq;
    int     month;
    int     year;
}   t_var;

typedef struct	s_date {
    int d;
	int	m;
	int	y;
}	t_date;

int tableExists1 = 0;

static int get_trim(sqlite3 *db, t_var *vars, char *trim) {
    int                 rc;
    char                buff[2000];
    sqlite3_stmt        *stmt;
    lxw_worksheet       *worksheet;
    lxw_workbook        *workbook;

	int col = 0;
    int row = 0;
    if (!trim) {
        return 1;
    }
	snprintf(buff, 2000, NB_DEB, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->year, vars->bq, vars->bq, vars->bq);
    rc = sqlite3_exec(db, buff, NULL, NULL, NULL);
    snprintf(buff, 2000, "SELECT * FROM NB_DEB_%s%s", vars->bq, trim);
    rc = sqlite3_prepare_v2(db, buff, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
        return 1;
    }
	workbook  = workbook_new(vars->file);
    worksheet = workbook_add_worksheet(workbook, NULL);
    worksheet_write_string(worksheet, 0, 0, "Date", NULL);
    worksheet_write_string(worksheet, 0, 1, "Nb_Cred", NULL);
    worksheet_write_string(worksheet, 0, 2, "Nb_Deb_1", NULL);
    worksheet_write_string(worksheet, 0, 3, "Nb_Deb_2", NULL);
    worksheet_write_string(worksheet, 0, 4, "Tt_Int", NULL);
    row = 1;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        col = -1;
		while (++col < 5) {
            if (col == 0)
                worksheet_write_string(worksheet, row, col, (char *)sqlite3_column_text(stmt, col), NULL);
            else
                worksheet_write_number(worksheet, row, col, sqlite3_column_int(stmt, col), NULL);       
		}
        row++;
    }
    return workbook_close(workbook);
}

static char *trim_conv(t_var *vars, char *buff, char *s1) {
    char    *s2;

    if (!strcmp(s1, "1er trimestre"))
        s2 = TRIM1;
    else if (!strcmp(s1, "2éme trimestre"))
        s2 = TRIM2;
    else if (!strcmp(s1, "3éme trimestre"))
        s2 = TRIM3;
    else if (!strcmp(s1, "4éme trimestre"))
        s2 = TRIM4;
    else
        return NULL;
    snprintf(buff, 200, s2, vars->year, vars->year);
    return(buff);
}

static int init_db(sqlite3	*db, t_var *vars, char *trim, char *wd)
{
	char	buff[200];
    char    path[200];

    if (!wd)
        return -1;
    path[0] = '\0';
    strcat(path, wd);
    strcat(path, "agio.db");
	if (!sqlite3_open(path, &db))
	{
        return(get_trim(db, vars, trim_conv(vars, buff, trim)));
	}
	else
		return (printf("error 1\n"), -1);
	return 0;
}

int init_export(t_var *vars, char *trim, char *wd) {
    sqlite3 *db;

    int r;
      db = NULL;
	r = init_db(db, vars, trim, wd);
    if (r == -1)
        return 1;
    else
        return(sqlite3_close(db), r);
}
