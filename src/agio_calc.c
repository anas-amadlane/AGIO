#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define NB_DEB "DROP VIEW IF EXISTS NB_DEB_%s ; CREATE VIEW IF NOT EXISTS NB_DEB_%s AS WITH NB_DEB_%s AS (SELECT DATE(S_DATE) AS DATE, CASE WHEN BALANCE >= 0 THEN BALANCE ELSE 0 END AS NB_CRE, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) THEN -1 * BALANCE ELSE (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) END AS NB_DEB_REG, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) THEN 0 ELSE -1 * BALANCE - (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) END AS NB_DEB_PLA FROM CONC_OP_%s_%d) SELECT DATE, NB_CRE, NB_DEB_REG, NB_DEB_PLA, ((NB_DEB_REG * (SELECT TAUX FROM TREG_%s WHERE EDATE>DATE)/100)/365 + (NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s WHERE EDATE>DATE)/100)/365) AS INTER FROM NB_DEB_%s;"

int	gen_conc(sqlite3 *db, t_var *vars)
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
		return (fprintf(stderr, "error 33\n"), sqlite3_finalize(stmt), 1);
	sqlite3_finalize(stmt);
    return 0;
}

int get_trim(sqlite3 *db, t_var *vars, char *trim) {
    int                 rc;
    char                buff[2000];

    if (!trim)
		return 1;
	snprintf(buff, 2000, NB_DEB, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->year, vars->bq, vars->bq, vars->bq);
    rc = sqlite3_exec(db, buff, NULL, NULL, NULL);
}



