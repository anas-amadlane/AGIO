#include <stdio.h>
#include <jansson.h>
#include <xlsxio_read.h>
// #include <libft.h>
#include <string.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <xlsxwriter.h>

#define MAX_ALLOC 5000
#define C_QUE "DROP TABLE IF EXISTS %s ; CREATE TABLE %s (DATE_OP DATE, DEBIT BIGINT, CREDIT BIGINT);"
#define SPEC_Q "CREATE TABLE IF NOT EXISTS BQ_SPEC(BQ VARCHAR, SOLDE DECIMAL);" //, LINE DECIMAL, LINE_SDATE DATE, LINE_EDATE DATE, REG_TX DECIMAL, REG_SDATE DATE, REG_EDATE DATE, PLA_TX DECIMAL, PLA_SDATE DATE, PLA_EDATE DATE);"

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


typedef struct	s_date {
    int d;
	int	m;
	int	y;
}	t_date;

typedef struct s_envlop
{
    char	wd[200];
	char	db_path[200];
    char    *bq;
    t_date	date;
}   t_envlop;

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

typedef struct  s_var
{
    char    *file;
    char    *bq;
    int     month;
    int		year;
}   t_var;

#define NB_DEB "CREATE VIEW IF NOT EXISTS NB_DEB_%s AS WITH NB_DEB_%s AS (SELECT DATE(S_DATE) AS DATE, CASE WHEN BALANCE >= 0 THEN BALANCE ELSE 0 END AS NB_CRE, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) THEN -1 * BALANCE ELSE (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) END AS NB_DEB_REG, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) THEN 0 ELSE -1 * BALANCE - (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) END AS NB_DEB_PLA FROM CONC_OP_%s) SELECT DATE, NB_CRE, NB_DEB_REG, NB_DEB_PLA, ((NB_DEB_REG * (SELECT TAUX FROM TREG_%s WHERE EDATE>DATE('now'))/100)/360 + (NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s WHERE EDATE>DATE('now'))/100)/360) AS INTER FROM NB_DEB_%s;"
int tableExists = 0;

extern t_envlop envlop;
// wd = "/home/anas/clones/agios_cal/";

int		gen_conc(sqlite3 *db, t_var *vars);//, t_rel *rel);
int		init_calc(sqlite3 *db, t_var *vars);//, t_rel *rel);
// void	bq_vars_checker(sqlite3 *db, t_var *vars);//, t_rel *rel);

int init_db(const char *tab, const char *qu2, t_var *vars)//, t_rel *rel)
{
	sqlite3	*db;
	char	buff[200];
	char	path[200];

	snprintf(buff, 200, C_QUE, tab, tab);
    db = NULL;
	// path[0] ='\0';
	// strcat(path, envlop.wd);
	// strcat(path, "agio.db");
	if (!sqlite3_open(envlop.db_path, &db))
	{
		if (sqlite3_exec(db, buff, NULL, 0, NULL))
			return (printf("%s error 2\n", buff), 1);
		if (sqlite3_exec(db, qu2, NULL, 0, NULL))
			return (printf("error 31\n"), 1);
		if (gen_conc(db, vars))
			return (sqlite3_close(db), 1);
		// init_calc(db, vars);//, rel);
		sqlite3_close(db);
	}
	else
		return (printf("error 1\n"), 1);
	return 0;
}

// int init_calc(sqlite3 *db, t_var *vars)//, t_rel *rel)
// {
// 		gen_conc(db, vars);//, rel);
// }

int callback(void *data, int argc, char **argv, char **azColName) {
    tableExists = 1;
    return 0;
}

int	gen_conc(sqlite3 *db, t_var *vars)//, t_rel *rel)
{
	char			buff[2000];
	int				rc;
	sqlite3_stmt	*stmt;
	char			sqlStatement[2000];

	snprintf(buff, 1000, "SELECT name FROM sqlite_master WHERE type = 'table' AND name LIKE '%s_%d_%%';", vars->bq, vars->year);
	rc = sqlite3_prepare_v2(db, buff, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return 1;
	}
	snprintf(sqlStatement, 2000,"DROP TABLE IF EXISTS CONC_OP_%s_%d; CREATE TABLE IF NOT EXISTS CONC_OP_%s_%d AS WITH date_range AS (SELECT DATE('%d-01-01') AS S_DATE UNION ALL SELECT DATE(S_DATE, '+1 day') FROM date_range WHERE S_DATE < DATE('%d-01-01', '-1 day')), tmp AS (", vars->bq, vars->year, vars->bq, vars->year,vars->year, vars->year + 1);
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		const unsigned char *tableName = sqlite3_column_text(stmt, 0);
		strcat(sqlStatement, "SELECT DATE(DATE_OP) AS S_DATE, SUM(DEBIT) AS S_DEBIT, SUM(CREDIT) AS S_CREDIT FROM ");
		strcat(sqlStatement, tableName);
		strcat(sqlStatement, " GROUP BY DATE(DATE_OP) ");
	}
	else
		return (sqlite3_finalize(stmt), 1);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		strcat(sqlStatement, "UNION ALL ");
		const unsigned char *tableName = sqlite3_column_text(stmt, 0);
		strcat(sqlStatement, "SELECT DATE(DATE_OP) AS S_DATE, SUM(DEBIT) AS S_DEBIT, SUM(CREDIT) AS S_CREDIT FROM ");
		strcat(sqlStatement, tableName);
		strcat(sqlStatement, " GROUP BY DATE(DATE_OP) ");
	}
	strcat(sqlStatement, ") SELECT date_range.S_DATE, COALESCE(tmp.S_DEBIT, 0) AS S_DEBIT, COALESCE(tmp.S_CREDIT, 0) AS S_CREDIT, (SELECT (SELECT SOLDE FROM BQ_SPEC WHERE BQ = '%s') + (SUM(COALESCE(tmp2.S_CREDIT, 0)) - SUM(COALESCE(tmp2.S_DEBIT, 0))) FROM tmp tmp2 WHERE tmp2.S_DATE <= date_range.S_DATE) AS BALANCE FROM date_range LEFT JOIN tmp ON tmp.S_DATE = date_range.S_DATE GROUP BY date_range.S_DATE ORDER BY date_range.S_DATE;\0");
	snprintf(buff, 2000, sqlStatement, vars->bq);
	if (sqlite3_exec(db, buff, NULL, 0, NULL))
		return (printf("error 33\n"),sqlite3_finalize(stmt), 1);
	sqlite3_finalize(stmt);
	//bq_vars_checker(db, vars, rel);
    return 0;
}

// void	bq_vars_checker(sqlite3 *db, t_var *vars, t_rel *rel) {
// 	char	table_name[100];
//     char	queryA[100];
// 	int		rc;

// 	tableExists = 0;
// 	snprintf(table_name, 100, "TREG_%s", vars->bq);
//     snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", table_name);
//     rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
//     if (rc == SQLITE_OK) {
// 		if (!tableExists) {
// 			snprintf(queryA, sizeof(queryA), "CREATE TABLE %s(SDATE DATE, EDATE DATE, TAUX DECIMAL);", table_name);
// 			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
// 			snprintf(queryA, 100, "INSERT INTO %s VALUES('%s', '%s', '%f');", table_name, rel->tx_reg.d_range[0], rel->tx_reg.d_range[1], rel->tx_reg.v);
// 			rc = sqlite3_exec(db,
// 			 queryA, NULL, NULL, NULL);
// 		}
// 	}
// 	tableExists = 0;
// 	snprintf(table_name, 100, "TPLA_%s", vars->bq);
//     snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", table_name);
//     rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
//     if (rc == SQLITE_OK) {
// 		if (!tableExists) {
// 			snprintf(queryA, sizeof(queryA), "CREATE TABLE %s(SDATE DATE, EDATE DATE, TAUX DECIMAL);", table_name);
// 			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
// 			snprintf(queryA, 100, "INSERT INTO %s VALUES('%s', '%s', '%f');", table_name, rel->tx_pla.d_range[0], rel->tx_pla.d_range[1], rel->tx_pla.v);
// 			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
// 		}
// 	}
// 	tableExists = 0;
// 	snprintf(table_name, 100, "LINE_%s", vars->bq);
//     snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", table_name);
//     rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
//     if (rc == SQLITE_OK) {
// 		if (!tableExists) {
// 			snprintf(queryA, sizeof(queryA), "CREATE TABLE %s(SDATE DATE, EDATE DATE, TAUX DECIMAL);", table_name);
// 			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
// 			snprintf(queryA, 100, "INSERT INTO %s VALUES('%s', '%s', '%f');", table_name, rel->line.d_range[0], rel->line.d_range[1], rel->line.v);
// 			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
// 		}
// 	}
// 	tableExists = 0;
//     snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='BQ_SPEC'");
//     rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
//     if (rc == SQLITE_OK) {
// 		if (!tableExists) {
// 			sqlite3_exec(db, SPEC_Q, NULL, 0, NULL);
// 		}
// 		tableExists = 0;
// 		snprintf(queryA, 100, "SELECT BQ FROM BQ_SPEC WHERE BQ='%s';", vars->bq);
// 		rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
// 		if (rc == SQLITE_OK) {
// 			if (!tableExists) {
// 				snprintf(queryA, 100, "INSERT INTO BQ_SPEC VALUES('%s', '%f');", vars->bq, rel->solde);
// 				printf("%s\n", queryA);
// 				rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
// 			}
// 		}
// 	}
// }
