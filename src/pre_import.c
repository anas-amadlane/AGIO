#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

typedef struct	s_date {
    int d;
	int	m;
	int	y;
}	t_date;

typedef struct s_envlop
{
    char	wd[500];
	char	db_path[500];
    char    *bq;
    t_date	date;
}   t_envlop;

static void bzeros(void * s, size_t n)
{
        memset(s, 0, n);
}

static int check_table(char *db_path, char *table, char *bq)
{
    int                 rs;
    int                 rt;
	char                buff[200];
    sqlite3             *db;
    struct sqlite3_stmt *selectstmt;

    if (!db_path || !bq || !table)
        return -1;
    rt = 2;
	if (!sqlite3_open(db_path, &db)) {
        bzeros(buff, 200);
        snprintf(buff, 200, "SELECT name FROM sqlite_master WHERE type = 'table' AND name LIKE '%s_%s';", table, bq);
        rs = sqlite3_prepare_v2(db, buff, -1, &selectstmt, NULL);
        if (rs == SQLITE_OK) {
            if (sqlite3_step(selectstmt) == SQLITE_ROW)
                rt = 0;
            else
                rt = 1;
        }
        sqlite3_finalize(selectstmt);
        sqlite3_close(db);
	}
	else
        fprintf(stderr, "error 1\n");
    return rt;
}

static int check_balance(char *db_path, char *bq)
{
    int                 rs;
    int                 rt;
	char                buff[200];
    sqlite3             *db;
    struct sqlite3_stmt *selectstmt;

    if (!db_path || !bq)
        return -1;
    rt = 2;
	if (!sqlite3_open(db_path, &db)) {
        bzeros(buff, 200);
        snprintf(buff, 200, "SELECT * FROM BQ_SPEC WHERE BQ='%s';", bq);
        rs = sqlite3_prepare_v2(db, buff, -1, &selectstmt, NULL);
        if (rs == SQLITE_OK) {
            if (sqlite3_step(selectstmt) == SQLITE_ROW)
                rt = 0;
            else
                rt = 1;
        }
        else {
            if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS BQ_SPEC(BQ VARCHAR, SOLDE DECIMAL);", NULL, NULL, NULL) == SQLITE_OK)
                rt = 1;
        }
        sqlite3_finalize(selectstmt);
        sqlite3_close(db);
	}
	else
        fprintf(stderr, "error 1\n");
    return rt;
}

static int insert_balance(char *db_path, char *bq, double value)
{
    int                 rt;
	char                buff[200];
    sqlite3             *db;

    if (!db_path || !bq)
        return -1;
    rt = 2;
	if (!sqlite3_open(db_path, &db)) {
        bzeros(buff, 200);
        snprintf(buff, 200, "DELETE FROM BQ_SPEC WHERE BQ='%s'; INSERT INTO BQ_SPEC VALUES('%s', %f);", bq, bq, value);
        if (sqlite3_exec(db, buff, NULL, NULL, NULL) == SQLITE_OK)
            rt = 0;
        else
            rt = 1;
        sqlite3_close(db);
    }
	else
        fprintf(stderr, "error 1\n");
    return rt;
}

static int create_table(t_envlop *envlop, char *table, double value) {
    int     rt;
    sqlite3 *db;
    char    buff[200];

    if (!envlop || !table)
        return -1;
    rt = 2;
    if (!sqlite3_open(envlop->db_path, &db)) {
        bzeros(buff, 200);
        snprintf(buff, 200, "DROP TABLE IF EXISTS %s_%s; CREATE TABLE IF NOT EXISTS %s_%s(SDATE DATE, EDATE DATE, TAUX DECIMAL);", table, envlop->bq, table, envlop->bq);
        if(sqlite3_exec(db, buff, NULL, NULL, NULL) == SQLITE_OK) {
            snprintf(buff, 200, "INSERT INTO %s_%s VALUES('%d-%d-%d', '%d-%d-%d', %f);", table, envlop->bq, envlop->date.y, envlop->date.m, envlop->date.d, envlop->date.y + 100, envlop->date.m, envlop->date.d, value);
            if(sqlite3_exec(db, buff, NULL, NULL, NULL) == SQLITE_OK)
                rt = 0;
            else
                rt = 1;
        }
        else
            rt = 1;
        sqlite3_close(db);
    }
    else
        printf ("Error opening\n");
    return rt;
}

int checker(t_envlop *envlop) {
    int     i;
    char    *tab[4] = {"LINE", "TREG", "TPLA", NULL};

    if (!envlop)
        return -1;
    if (check_balance(envlop->db_path, envlop->bq) == 1)
        return 1;
    i = -1;
    while (tab[++i]) {
        if (check_table(envlop->db_path, tab[i], envlop->bq) == 1)
            return 1;
    }
    return 0;
}

int creater(t_envlop *envlop, double *value) {
    int     i;
    char    *tab[4] = {"LINE", "TREG", "TPLA", NULL};

    if (!envlop || !value)
        return -1;
    i = -1;
    while(tab[++i])
        create_table(envlop, tab[i], value[i]);
    insert_balance(envlop->db_path, envlop->bq, value[i]);
    return 0;
}
