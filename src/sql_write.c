#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define C_QUE "DROP TABLE IF EXISTS %s ; CREATE TABLE %s (DATE_OP DATE, DEBIT BIGINT, CREDIT BIGINT);"

typedef struct	s_date {
	int	d;
	int	m;
	int	y;
}	t_date;

typedef struct	s_envlop {
	char	wd[500];
	char	db_path[500];
    char	*bq;
    t_date	date;
}	t_envlop;

typedef struct	s_var {
    char	*file;
    char	*bq;
    int		month;
    int		year;
}	t_var;

extern t_envlop	envlop;

static int	gen_conc(sqlite3 *db, t_var *vars)
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
		strcat(sqlStatement, (char *)tableName);
		strcat(sqlStatement, " GROUP BY DATE(DATE_OP) ");
	}
	else
		return (sqlite3_finalize(stmt), 1);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		strcat(sqlStatement, "UNION ALL ");
		const unsigned char *tableName = sqlite3_column_text(stmt, 0);
		strcat(sqlStatement, "SELECT DATE(DATE_OP) AS S_DATE, SUM(DEBIT) AS S_DEBIT, SUM(CREDIT) AS S_CREDIT FROM ");
		strcat(sqlStatement, (char *)tableName);
		strcat(sqlStatement, " GROUP BY DATE(DATE_OP) ");
	}
	strcat(sqlStatement, ") SELECT date_range.S_DATE, COALESCE(tmp.S_DEBIT, 0) AS S_DEBIT, COALESCE(tmp.S_CREDIT, 0) AS S_CREDIT, (SELECT (SELECT SOLDE FROM BQ_SPEC WHERE BQ = '%s') + (SUM(COALESCE(tmp2.S_CREDIT, 0)) - SUM(COALESCE(tmp2.S_DEBIT, 0))) FROM tmp tmp2 WHERE tmp2.S_DATE <= date_range.S_DATE) AS BALANCE FROM date_range LEFT JOIN tmp ON tmp.S_DATE = date_range.S_DATE GROUP BY date_range.S_DATE ORDER BY date_range.S_DATE;\0");
	snprintf(buff, 2000, sqlStatement, vars->bq);
	if (sqlite3_exec(db, buff, NULL, 0, NULL))
		return (printf("error 33\n"), sqlite3_finalize(stmt), 1);
	sqlite3_finalize(stmt);
    return 0;
}

int init_db(const char *tab, const char *qu2, t_var *vars)
{
	sqlite3	*db;
	char	buff[200];

	snprintf(buff, 200, C_QUE, tab, tab);
    db = NULL;
	if (!sqlite3_open(envlop.db_path, &db))
	{
		if (sqlite3_exec(db, buff, NULL, 0, NULL))
			return (printf("%s error 2\n", buff), 1);
		if (sqlite3_exec(db, qu2, NULL, 0, NULL))
			return (printf("error 31\n"), 1);
		if (gen_conc(db, vars))
			return (sqlite3_close(db), 1);
		sqlite3_close(db);
	}
	else
		return (printf("error 1\n"), 1);
	return 0;
}
