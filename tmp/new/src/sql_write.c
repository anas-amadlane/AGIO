#include <stdio.h>
#include <jansson.h>
#include <xlsxio_read.h>
#include <libft.h>
#include <string.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <xlsxwriter.h>

#define MAX_ALLOC 5000
#define C_QUE "DROP TABLE IF EXISTS %s ; CREATE TABLE %s (DATE_OP DATE, DATE_VA DATE, DEBIT BIGINT, CREDIT BIGINT);"
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

typedef struct  s_var
{
    char    *file;
    char    *bq;
    int     month;
    int		year;
}   t_var;

int	gen_conc(sqlite3 *db, t_var *vars, t_rel *rel);
int init_calc(sqlite3 *db, t_var *vars, t_rel *rel);

int init_db(const char *tab, const char *qu2, t_var *vars, t_rel *rel)
{
	sqlite3	*db;
	char	buff[200];

	snprintf(buff, 200, C_QUE, tab, tab);
    db = NULL;
	if (!sqlite3_open("../agio.db", &db))
	{
		if (sqlite3_exec(db, buff, NULL, 0, NULL))
			return (printf("error 2\n"), 1);
		//printf("%s\n", qu2);
		if (sqlite3_exec(db, qu2, NULL, 0, NULL))
			return (printf("error 31\n"), 1);
		init_calc(db, vars, rel);
		sqlite3_close(db);
	}
	else
		return (printf("error 1\n"), 1);
	return 0;
}

//int get_trim(sqlite3 *db, t_var *vars, char *trim);
//int export_trim(sqlite3_stmt *stmt) ;

int init_calc(sqlite3 *db, t_var *vars, t_rel *rel) {

		// if (sqlite3_exec(db, SPEC_Q, NULL, 0, NULL))
		// 	return (printf("error 32\n"), 1);
		gen_conc(db, vars, rel);
		//get_trim(db, vars, " WHERE DATE BETWEEN '2023-01-01' AND '2023-03-31';");
}


// // Callback function to process the result of the SELECT statement
// static int callback(void *data, int argc, char **argv, char **azColName) {
//   // Process each row of the result
//   for (int i = 0; i < argc; i++) {
//     printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
//   }
//   printf("\n");
//   return 0;
// }

int tableExists = 0;

int callback(void *data, int argc, char **argv, char **azColName) {
    tableExists = 1;
    return 0;
}

void	bq_vars_checker(sqlite3 *db, t_var *vars, t_rel *rel);
int	gen_conc(sqlite3 *db, t_var *vars, t_rel *rel) {
  // Get the table names that match the pattern 'BMCE_YYYY_M'
  char	buff[2000];
  snprintf(buff, 1000, "SELECT name FROM sqlite_master WHERE type = 'table' AND name LIKE '%s_%d_%%';", vars->bq, vars->year);
  //const char *tableNamesQuery = "SELECT name FROM sqlite_master WHERE type = 'table' AND name LIKE 'BMCE_%'";
  
  sqlite3_stmt *stmt;
  int rc;
  rc = sqlite3_prepare_v2(db, buff, -1, &stmt, 0);
  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
    return 1;
  }
  
  // Create a string to store the concatenated SQL statement
  char sqlStatement[2000];
  snprintf(sqlStatement, 2000,"DROP VIEW IF EXISTS CONC_OP_%s; CREATE VIEW IF NOT EXISTS CONC_OP_%s AS WITH date_range AS (SELECT DATE('%d-01-01') AS S_DATE UNION ALL SELECT DATE(S_DATE, '+1 day') FROM date_range WHERE S_DATE < DATE('%d-01-01', '-1 day')), tmp AS (", vars->bq, vars->bq, vars->year, vars->year + 1);
  
  // Iterate over the table names and build the SQL statement
  if (sqlite3_step(stmt) == SQLITE_ROW) {
  const unsigned char *tableName = sqlite3_column_text(stmt, 0);
  strcat(sqlStatement, "SELECT DATE(DATE_OP) AS S_DATE, SUM(DEBIT) AS S_DEBIT, SUM(CREDIT) AS S_CREDIT FROM ");
  strcat(sqlStatement, tableName);
  strcat(sqlStatement, " GROUP BY DATE(DATE_OP) ");
  }
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    strcat(sqlStatement, "UNION ALL ");
    const unsigned char *tableName = sqlite3_column_text(stmt, 0);
  strcat(sqlStatement, "SELECT DATE(DATE_OP) AS S_DATE, SUM(DEBIT) AS S_DEBIT, SUM(CREDIT) AS S_CREDIT FROM ");
  strcat(sqlStatement, tableName);
  strcat(sqlStatement, " GROUP BY DATE(DATE_OP) ");
  }
  strcat(sqlStatement, ") SELECT date_range.S_DATE, COALESCE(tmp.S_DEBIT, 0) AS S_DEBIT, COALESCE(tmp.S_CREDIT, 0) AS S_CREDIT, (SELECT (SELECT SOLDE FROM BQ_SPEC WHERE BQ = '%s') + (SUM(COALESCE(tmp2.S_CREDIT, 0)) - SUM(COALESCE(tmp2.S_DEBIT, 0))) FROM tmp tmp2 WHERE tmp2.S_DATE <= date_range.S_DATE) AS BALANCE FROM date_range LEFT JOIN tmp ON tmp.S_DATE = date_range.S_DATE GROUP BY date_range.S_DATE ORDER BY date_range.S_DATE;\0");
//   int len = strlen(sqlStatement);
  snprintf(buff, 2000, sqlStatement, vars->bq);
//   printf("%s\n", buff);
  //printf("%s\n", buff);
  // Remove the last "UNION ALL" from the statement
//   sqlStatement[len] = '\0';
//   if (len > 10) {
//     sqlStatement[len - 10] = '\0';
//   }
//   printf("%s\n", sqlStatement);
//   printf("%s\n", buff);
if (sqlite3_exec(db, buff, NULL, 0, NULL))
	return (printf("error 33\n"), 1);

	bq_vars_checker(db, vars, rel);
    // const char *table_name = "TREG_BP";
    // char queryA[100];
    // snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", table_name);
    
    
    // rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
    
    // if (rc == SQLITE_OK && tableExists) {
	// 	return (0);
    // }
	// else {
	// 	snprintf(queryA, sizeof(queryA), "CREATE TABLE TREG_%s(SDATE DATE, EDATE DATE, TAUX DECIMAL);", vars->bq);
	// 	rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
	// }
//   // Execute the final concatenated SQL statement
//   rc = sqlite3_exec(db, sqlStatement, callback, 0, &errMsg);
//   if (rc != SQLITE_OK) {
//     fprintf(stderr, "Failed to execute statement2: %s\n", errMsg);
//     sqlite3_free(errMsg);
//     return 1;
//   }
  
  // Close the database connection
//   sqlite3_close(db);
  
  return 0;
}

#define NB_DEB "CREATE VIEW IF NOT EXISTS NB_DEB_%s AS WITH NB_DEB_%s AS (SELECT DATE(S_DATE) AS DATE, CASE WHEN BALANCE >= 0 THEN BALANCE ELSE 0 END AS NB_CRE, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) THEN -1 * BALANCE ELSE (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) END AS NB_DEB_REG, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) THEN 0 ELSE -1 * BALANCE - (SELECT TAUX FROM LINE_%s WHERE EDATE>DATE('now')) END AS NB_DEB_PLA FROM CONC_OP_%s) SELECT DATE, NB_CRE, NB_DEB_REG, NB_DEB_PLA, ((NB_DEB_REG * (SELECT TAUX FROM TREG_%s WHERE EDATE>DATE('now'))/100)/360 + (NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s WHERE EDATE>DATE('now'))/100)/360) AS INTER FROM NB_DEB_%s;"

void	bq_vars_checker(sqlite3 *db, t_var *vars, t_rel *rel){
	char table_name[100];
    char queryA[100];
	int rc;

	// char	*tables[] = {"TREG_%s", "TPLA_%s", "LINE_%s", NULL};
	// int i = -1;
	// while(tables[++i]) {
	// 	tableExists = 0;
	// 	snprintf(table_name, 100, tables[i], vars->bq);
    // 	snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", table_name);
    // 	rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
    // 	if (rc == SQLITE_OK) {
	// 		if (!tableExists) {
	// 			snprintf(queryA, sizeof(queryA), "CREATE TABLE %s(SDATE DATE, EDATE DATE, TAUX DECIMAL);", table_name);
	// 			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
	// 		}
	// 		else
	// 			printf("%d, ok\n", tableExists);
	// 	}
	// }

	tableExists = 0;
	snprintf(table_name, 100, "TREG_%s", vars->bq);
    snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", table_name);
    rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
    if (rc == SQLITE_OK) {
		if (!tableExists) {
			snprintf(queryA, sizeof(queryA), "CREATE TABLE %s(SDATE DATE, EDATE DATE, TAUX DECIMAL);", table_name);
			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
			snprintf(queryA, 100, "INSERT INTO %s VALUES('%s', '%s', '%f');", table_name, rel->tx_reg.d_range[0], rel->tx_reg.d_range[1], rel->tx_reg.v);
			// printf("%s\n", queryA);
			rc = sqlite3_exec(db,
			 queryA, NULL, NULL, NULL);
		}
	}
	tableExists = 0;
	snprintf(table_name, 100, "TPLA_%s", vars->bq);
    snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", table_name);
    rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
    if (rc == SQLITE_OK) {
		if (!tableExists) {
			snprintf(queryA, sizeof(queryA), "CREATE TABLE %s(SDATE DATE, EDATE DATE, TAUX DECIMAL);", table_name);
			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
			snprintf(queryA, 100, "INSERT INTO %s VALUES('%s', '%s', '%f');", table_name, rel->tx_pla.d_range[0], rel->tx_pla.d_range[1], rel->tx_pla.v);
			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
		}
	}
	tableExists = 0;
	snprintf(table_name, 100, "LINE_%s", vars->bq);
    snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='%s'", table_name);
    rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
    if (rc == SQLITE_OK) {
		if (!tableExists) {
			snprintf(queryA, sizeof(queryA), "CREATE TABLE %s(SDATE DATE, EDATE DATE, TAUX DECIMAL);", table_name);
			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
			snprintf(queryA, 100, "INSERT INTO %s VALUES('%s', '%s', '%f');", table_name, rel->line.d_range[0], rel->line.d_range[1], rel->line.v);
			rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
		}
	}
	tableExists = 0;
    snprintf(queryA, sizeof(queryA), "SELECT name FROM sqlite_master WHERE type='table' AND name='BQ_SPEC'");
    rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
    if (rc == SQLITE_OK) {
		if (!tableExists) {
			sqlite3_exec(db, SPEC_Q, NULL, 0, NULL);

			//snprintf(queryA, 100, "INSERT INTO %s VALUES('%s', '%s', '%f');", table_name, rel->line.d_range[0], rel->line.d_range[1], rel->line.v);
			//rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
		}
		tableExists = 0;
		snprintf(queryA, 100, "SELECT BQ FROM BQ_SPEC WHERE BQ='%s';", vars->bq);
		rc = sqlite3_exec(db, queryA, callback, NULL, NULL);
		if (rc == SQLITE_OK) {
			if (!tableExists) {
				snprintf(queryA, 100, "INSERT INTO BQ_SPEC VALUES('%s', '%f');", vars->bq, rel->solde);
				printf("%s\n", queryA);
				rc = sqlite3_exec(db, queryA, NULL, NULL, NULL);
			}
		}
	}
}

// int get_trim(sqlite3 *db, t_var *vars, char *trim) {
//     int                 rc;
//     char                buff[2000];
//     sqlite3_stmt        *stmt;
//     const unsigned char *tableName;

// 	int col = 0;
//     int row = 0;
//     lxw_workbook  *workbook;
//     lxw_worksheet *worksheet;


// 	snprintf(buff, 2000, NB_DEB, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq, vars->bq);
//     rc = sqlite3_exec(db, buff, NULL, NULL, NULL);
//     snprintf(buff, 2000, "SELECT * FROM NB_DEB_%s%s", vars->bq, trim);
// 	printf("%s\n", buff);
//     rc = sqlite3_prepare_v2(db, buff, -1, &stmt, 0);
//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
//         return 1;
//     }

// 	workbook  = workbook_new("trim.xlsx");
//     worksheet = workbook_add_worksheet(workbook, NULL);
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

// int export_trim(sqlite3_stmt *stmt) {
//     int col = 0;
//     int row = 0;
//     lxw_workbook  *workbook;
//     lxw_worksheet *worksheet;

//     workbook  = workbook_new("trim.xlsx");
//     worksheet = workbook_add_worksheet(workbook, NULL);
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
