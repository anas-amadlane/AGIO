#define TRIM1 " WHERE DATE BETWEEN '%d-01-01' AND '%d-03-31';"
#define TRIM2 " WHERE DATE BETWEEN '%d-04-01' AND '%d-06-30';"
#define TRIM3 " WHERE DATE BETWEEN '%d-07-01' AND '%d-09-30';"
#define TRIM4 " WHERE DATE BETWEEN '%d-10-01' AND '%d-12-31';"
#include <stdio.h>
#include <jansson.h>
#include <xlsxio_read.h>
#include <libft.h>
#include <string.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <xlsxwriter.h>

typedef struct  s_var
{
    char    *file;
    char    *bq;
    int     month;
    int     year;
}   t_var;

int tableExists1 = 0;
int init_export(t_var *vars, char *trim);
int get_trim(sqlite3 *db, t_var *vars, char *trim);
static int callback(void *data, int argc, char **argv, char **azColName) {
    tableExists1 = 1;
    return 0;
}

char *trim_conv(t_var *vars, char *buff, char *s1) {
    char    *s2;

    //printf("%s\n", s1);
    if (!strcmp(s1, "1er trimestre"))
        s2 =TRIM1;
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

static int init_db(sqlite3	*db, t_var *vars, char *trim)
{
	char	buff[200];
    int rc;

	if (!sqlite3_open("../agio.db", &db))
	{
        // tableExists1 = 0;
        // snprintf(buff, 200, "SELECT name FROM sqlite_master WHERE type='view' AND name='NB_DEB_%s';", vars->bq);
        // //printf("%s\n", buff);
        // rc = sqlite3_exec(db, buff, callback, NULL, NULL);
        // if (rc == SQLITE_OK) {
		//     if (!tableExists1)
        //         return 1;
        //     else
                return(get_trim(db, vars, trim_conv(vars, buff, trim)));
	    // }
        // else
        //     return 2;
	}
	else
		return (printf("error 1\n"), -1);
	return 0;
}

int init_export(t_var *vars, char *trim) {
    sqlite3	*db;
    //t_rel *releve;
    int r;
      db = NULL;
	//releve = jsonparser("/home/anas/clones/agios_cal/data/module.json", vars->bq);
	//if (!releve)
	//	return 1;
	r = init_db(db, vars, trim);
    printf("%d\n", r);
    if (r == -1)
        return 1;
    else
        return(sqlite3_close(db), r);
}
#define NB_DEB "CREATE VIEW IF NOT EXISTS NB_DEB_%s AS WITH NB_DEB_%s AS (SELECT DATE(S_DATE) AS DATE, CASE WHEN BALANCE >= 0 THEN BALANCE ELSE 0 END AS NB_CRE, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) THEN -1 * BALANCE ELSE (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) END AS NB_DEB_REG, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) THEN 0 ELSE -1 * BALANCE - (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) END AS NB_DEB_PLA FROM CONC_OP_%s) SELECT DATE, NB_CRE, NB_DEB_REG, NB_DEB_PLA, ((NB_DEB_REG * (SELECT TAUX FROM TREG_%s WHERE EDATE>DATE('now'))/100)/360 + (NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s WHERE EDATE>DATE('now'))/100)/360) AS INTER FROM NB_DEB_%s;"
int get_trim(sqlite3 *db, t_var *vars, char *trim) {
    int                 rc;
    char                buff[2000];
    sqlite3_stmt        *stmt;
    const unsigned char *tableName;

	int col = 0;
    int row = 0;
    lxw_workbook  *workbook;
    lxw_worksheet *worksheet;

    if (!trim)
        return 1;
	snprintf(buff, 2000, NB_DEB, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq);
    //printf("%s\n", buff);
    rc = sqlite3_exec(db, buff, NULL, NULL, NULL);
    snprintf(buff, 2000, "SELECT * FROM NB_DEB_%s%s", vars->bq, trim);
	printf("%s\n", buff);
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
                worksheet_write_string(worksheet, row, col, sqlite3_column_text(stmt, col), NULL);
            else
                worksheet_write_number(worksheet, row, col, sqlite3_column_int(stmt, col), NULL);       
		}
        row++;
    }
    return workbook_close(workbook);
}

// int get_trim(sqlite3 *db, t_var *vars, char *trim) {
//     int                 rc;
//     char                buff[2000];
//     sqlite3_stmt        *stmt;
//     const unsigned char *tableName

//     snprintf(buff, 2000, "SELECT * FROM CONC_OP_%s%s", vars->bq, trim);
//     rc = sqlite3_prepare_v2(db, buff, -1, &stmt, 0);
//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
//         return 1;
//     }
//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         tableName = sqlite3_column_text(stmt, 0);
//         printf("%s\n", tableName);
//     }
// }

// int export_trim() {
//     int col = 0;
//     int row = 0;
//     lxw_workbook  *workbook;
//     lxw_worksheet *worksheet;

//     lxw_workbook  *workbook  = workbook_new("trim.xlsx");
//     lxw_worksheet *worksheet = workbook_add_worksheet(workbook, NULL);
//     row = 0;
//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         col = -1;
// 		while (++col < 5) {
//             if (col == 0)
//                 worksheet_write_string(worksheet, row, col, sqlite3_column_text(stmt, col), NULL);
//             else
//                 worksheet_write_number(worksheet, row, col, sqlite3_column_int(stmt, col), NULL);       
// 		}
//         row++;
//     }
//     return workbook_close(workbook);
// }
