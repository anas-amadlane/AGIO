#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define SEARCH "SELECT name FROM sqlite_master WHERE type = 'table' AND (name='%s_%d_%d' or name='%s_%d_%d' or name='%s_%d_%d');"
int tableExists(sqlite3* db, const char* tableName) {
    char* query = sqlite3_mprintf("SELECT name FROM sqlite_master WHERE type='table' AND name='%q';", tableName);
    sqlite3_stmt* stmt;
    int result = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
    
    if (result != SQLITE_OK) {
        sqlite3_free(query);
        return -1;
    }
    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_free(query);
    
    if (result == SQLITE_ROW)
		return 1;
	else
		return 0;
}


int gen_conc(sqlite3 *db, int trim, t_var *vars) {
    int				rc;
	int				month;
	sqlite3_stmt	*stmt;
	char			buff[200];
	char			sqlStatement[2000];

    month = (trim * 2) + trim - 2;
    bzero(sqlStatement, 2000);
	snprintf(sqlStatement, 1000, SEARCH, vars->bq, vars->year, month, vars->bq, vars->year, month+1, vars->bq, vars->year, month+2);
	rc = sqlite3_prepare_v2(db, sqlStatement, -1, &stmt, 0);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return 1;
	}
	bzero(sqlStatement, 2000);
	snprintf(sqlStatement, 2000,"DROP TABLE IF EXISTS CONC_%s_%d_%d; CREATE TABLE IF NOT EXISTS CONC_%s_%d_%d AS WITH TMP AS (", vars->bq, vars->year, trim, vars->bq, vars->year, trim);
	if (sqlite3_step(stmt) == SQLITE_ROW) {
		const unsigned char *tableName = sqlite3_column_text(stmt, 0);
		strcat(sqlStatement, "SELECT * FROM ");
		strcat(sqlStatement, (char *)tableName);
	}
	else
		return (sqlite3_finalize(stmt), 1);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		strcat(sqlStatement, " UNION ALL");
		const unsigned char *tableName = sqlite3_column_text(stmt, 0);
		strcat(sqlStatement, " SELECT * FROM ");
		strcat(sqlStatement, (char *)tableName);
	}
	strcat(sqlStatement, ") SELECT DATE_OP, SUM(COALESCE(DEBIT, 0)), SUM(COALESCE(CREDIT, 0)) FROM TMP GROUP BY DATE_OP ORDER BY DATE_OP;\0");
	if (sqlite3_exec(db, sqlStatement, NULL, 0, NULL))
		return (fprintf(stderr, "error 33\n"), sqlite3_finalize(stmt), 1);
	sqlite3_finalize(stmt);
	
    bzero(buff, 200);
	snprintf(buff, 200, "TMP_%s_%d", vars->bq, vars->year);
	if (!tableExists(db, buff)) {
		bzero(sqlStatement, 2000);
		snprintf(sqlStatement, 2000, "CREATE TABLE IF NOT EXISTS TMP_%s_%d (DATE_OP DATE, DEBIT BIGINT, CREDIT BIGINT);", vars->bq, vars->year);
		if (sqlite3_exec(db, sqlStatement, NULL, 0, NULL))
			return (fprintf(stderr, "error 33\n"), 1);	
	}
    bzero(sqlStatement, 2000);
	if (month < 7)
		snprintf(sqlStatement, 2000, "INSERT INTO CONC_%s_%d_%d SELECT * FROM TMP_%s_%d WHERE DATE_OP BETWEEN '%d-0%d-01' AND date('%d-0%d-01', '-1 day');", vars->bq, vars->year, trim, vars->bq, vars->year, vars->year, month, vars->year, month+3);
	else if (month < 8)
		snprintf(sqlStatement, 2000, "INSERT INTO CONC_%s_%d_%d SELECT * FROM TMP_%s_%d WHERE DATE_OP BETWEEN '%d-0%d-01' AND date('%d-%d-01', '-1 day');", vars->bq, vars->year, trim, vars->bq, vars->year, vars->year, month, vars->year, month+3);
	else
		snprintf(sqlStatement, 2000, "INSERT INTO CONC_%s_%d_%d SELECT * FROM TMP_%s_%d WHERE DATE_OP BETWEEN '%d-%d-01' AND date('%d-%d-01', '-1 day');", vars->bq, vars->year, trim, vars->bq, vars->year, vars->year, month, vars->year, month+3);
	if (sqlite3_exec(db, sqlStatement, NULL, 0, NULL))
		return (fprintf(stderr, "error 34\n"), 1);
	bzero(sqlStatement, 2000);
	if (month+3 < 10)
		snprintf(sqlStatement, 2000, "INSERT OR REPLACE INTO TMP_%s_%d SELECT * FROM CONC_%s_%d_%d WHERE DATE_OP > date('%d-0%d-01', '-1 day'); DELETE FROM CONC_%s_%d_%d WHERE DATE_OP > date('%d-0%d-01', '-1 day');", vars->bq, vars->year, vars->bq, vars->year, trim, vars->year, month+3, vars->bq, vars->year, trim, vars->year, month+3);
	else
		snprintf(sqlStatement, 2000, "INSERT OR REPLACE INTO TMP_%s_%d SELECT * FROM CONC_%s_%d_%d WHERE DATE_OP > date('%d-%d-01', '-1 day'); DELETE FROM CONC_%s_%d_%d WHERE DATE_OP > date('%d-%d-01', '-1 day');", vars->bq, vars->year, vars->bq, vars->year, trim, vars->year, month+3, vars->bq, vars->year, trim, vars->year, month+3);
	if (sqlite3_exec(db, sqlStatement, NULL, 0, NULL))
		return (fprintf(stderr, "error 35\n"), 1);
	bzero(sqlStatement, 2000);
	if (month < 10)
		snprintf(sqlStatement, 2000, "INSERT INTO CONC_%s_%d_%d VALUES('%d-0%d-01', (case when (SELECT SOLDE FROM VSOLDE_%s_%d WHERE TRM=%d)<0 then (SELECT SOLDE FROM VSOLDE_%s_%d WHERE TRM=%d)*(-1) else 0 end), (case when (SELECT SOLDE FROM VSOLDE_%s_%d WHERE TRM=%d)>0 then (SELECT SOLDE FROM VSOLDE_%s_%d WHERE TRM=%d) else 0 end));", vars->bq, vars->year, trim, vars->year, month, vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim);
	else
		snprintf(sqlStatement, 2000, "INSERT INTO CONC_%s_%d_%d VALUES('%d-%d-01', (case when (SELECT SOLDE FROM VSOLDE_%s_%d WHERE TRM=%d)<0 then (SELECT SOLDE FROM VSOLDE_%s_%d WHERE TRM=%d)*(-1) else 0 end), (case when (SELECT SOLDE FROM VSOLDE_%s_%d WHERE TRM=%d)>0 then (SELECT SOLDE FROM VSOLDE_%s_%d WHERE TRM=%d) else 0 end));", vars->bq, vars->year, trim, vars->year, month, vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim);
	if (sqlite3_exec(db, sqlStatement, NULL, 0, NULL))
		return (fprintf(stderr, "error 36\n"), 1);
    return 0;
}

int gen_bal(sqlite3 *db, int trim, t_var *vars)
{
	int				rc;
	int				month;
	sqlite3_stmt	*stmt;
	char			buff[200];
	char			sqlStatement[2000];

	(void)db;
	(void)rc;
	(void)buff;
	(void)stmt;
    month = (trim * 2) + trim - 2;

	bzero(sqlStatement, 2000);
	snprintf(sqlStatement, 2000, "DROP TABLE IF EXISTS BALANCE_%s_%d_%d; CREATE TABLE BALANCE_%s_%d_%d AS WITH TMP AS (SELECT DATE_OP, SUM(DEBIT) AS DEBIT, SUM(CREDIT) AS CREDIT FROM CONC_%s_%d_%d GROUP BY DATE_OP) SELECT DATE_OP, DEBIT, CREDIT, SUM(CREDIT-DEBIT) OVER(ORDER BY DATE_OP ASC) AS BALANCE FROM TMP;", vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim);
	if (sqlite3_exec(db, sqlStatement, NULL, 0, NULL))
		return (fprintf(stderr, "error 36\n"), 1);
	bzero(sqlStatement, 2000);
	snprintf(sqlStatement, 2000, "ALTER TABLE BALANCE_%s_%d_%d ADD COLUMN JOUR INT; UPDATE BALANCE_%s_%d_%d SET JOUR = (SELECT CAST(JULIANDAY(t2.DATE_OP) - JULIANDAY(t1.DATE_OP) AS INT) FROM BALANCE_%s_%d_%d t1 LEFT JOIN BALANCE_%s_%d_%d t2 ON t1.rowid = t2.rowid - 1 WHERE t1.DATE_OP = BALANCE_%s_%d_%d.DATE_OP); UPDATE BALANCE_%s_%d_%d SET JOUR = 1 WHERE DATE_OP = (SELECT MAX(DATE_OP) FROM BALANCE_%s_%d_%d);", vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim);
	if (sqlite3_exec(db, sqlStatement, NULL, 0, NULL))
		return (fprintf(stderr, "error 36\n"), 1);
	bzero(sqlStatement, 2000);
	snprintf(sqlStatement, 2000, "DROP TABLE IF EXISTS NB_%s_%d_%d; CREATE TABLE IF NOT EXISTS NB_%s_%d_%d AS WITH TMP AS ( SELECT DATE_OP, JOUR, CASE WHEN BALANCE >= 0 THEN BALANCE ELSE 0 END AS NB_CRE, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_BP WHERE EDATE>DATE_OP) THEN -1 * BALANCE ELSE (SELECT TAUX FROM LINE_BP WHERE EDATE>DATE_OP) END AS NB_DEB_REG, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_BP WHERE EDATE>DATE_OP) THEN 0 ELSE -1 * BALANCE - (SELECT TAUX FROM LINE_BP WHERE EDATE>DATE_OP) END AS NB_DEB_PLA FROM BALANCE_%s_%d_%d) SELECT DATE_OP, ROUND(NB_DEB_REG), ROUND(NB_DEB_PLA), ROUND(((NB_DEB_REG * (SELECT TAUX FROM TREG_BP WHERE EDATE>DATE_OP)/100)/360)*JOUR) AS INT_REG, ROUND(((NB_DEB_PLA * (SELECT TAUX FROM TPLA_BP WHERE EDATE>DATE_OP)/100)/360)*JOUR) AS INT_PLA FROM TMP;", vars->bq, vars->year, trim, vars->bq, vars->year, trim, vars->bq, vars->year, trim);
	if (sqlite3_exec(db, sqlStatement, NULL, 0, NULL))
		return (fprintf(stderr, "error 36\n"), 1);
	bzero(sqlStatement, 2000);
	if (trim == 4)
		snprintf(sqlStatement, 2000, "CREATE IF NOT EXISTS TABLE VSOLDE_%s_%d(TRIM INT PRIMARY KEY, SOLDE BIGINT); INSERT OR REPLACE INTO VSOLDE_%s_%d VALUES(%d, (SELECT BALANCE FROM BALANCE_%s_%d_%d ORDER BY DATE_OP DESC LIMIT 1));", vars->bq, vars->year+1, vars->bq, vars->year+1, trim+1, vars->bq, vars->year, trim);
	else
		snprintf(sqlStatement, 2000, "INSERT OR REPLACE INTO VSOLDE_%s_%d VALUES(%d, (SELECT BALANCE FROM BALANCE_%s_%d_%d ORDER BY DATE_OP DESC LIMIT 1);", vars->bq, vars->year, trim+1, vars->bq, vars->year, trim);
	if (sqlite3_exec(db, sqlStatement, NULL, 0, NULL))
		return (fprintf(stderr, "error 36\n"), 1);
	return 0;
}



// // 1  1  1 1*2+1-2 1  3  2
// // 4  2  4 2*2+2-2 4  6 2
// // 7  3  7 3*2+3-2 7	 9
// // 10 4  10 4*2+4-2 10 12