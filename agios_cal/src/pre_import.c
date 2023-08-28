#include <stdio.h>
#include<stdlib.h>
#include <jansson.h>
// #include <xlsxio_read.h>
#include <string.h>
#include <sqlite3.h>

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

// t_date date;

int	json_writerr(char *file, char *bq, t_date *date);
// extern const char    *wd;
// const char    *wd;

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
        printf("error 1\n");
    return rt;
}
int check_balance(char *db_path, char *bq)
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
        printf("error 1\n");
    return rt;
}
int insert_balance(char *db_path, char *bq, double value)
{
    int                 rs;
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
        printf("error 1\n");
    return rt;
}
int create_table(t_envlop *envlop, char *table, double value) {
    int     rs;
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
            //printf("%4d-%d-%d\n", envlop->date.y, envlop->date.m, envlop->date.d);
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
    // printf("%s\n", envlop->db_path);
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

// int main() {
//     t_envlop envlop;
//     char    *bq;
//     char    path[200];
//     int     value[3];

//     wd = "/home/anas/clones/agios_cal/";
//     bzeros(path, 200);
//     (strcat(path, wd), strcat(path, "agio.db"));
//     envlop.db_path = path;
//     envlop.bq = "BMCE";
//     envlop.date.d = 12;
//     envlop.date.m = 3;
//     envlop.date.y = 2023;
//     value[0] = 230045;
//     value[1] = 2.5;
//     value[2] = 4;
//     if (checker(&envlop) == 1) {
//         printf("passed\n");
//         creater(&envlop, 2000, value);
//         bzeros(path, 200);
//         (strcat(path, wd), strcat(path, "data/module_tmp.json"));
//         json_writer(path, envlop.bq, &envlop.date);
//     }
//     return 0;
// }

int	json_writerr(char *file, char *bq, t_date *date) {
    int				rt;
	json_t			*object;
    json_t			*root;
    json_error_t	error;
	// t_date			date;

	rt = 1;
    if (!file || !bq || !date)
        return rt;
    root = json_load_file(file, 0, &error);
    if (!root)
        return (fprintf(stderr, "Failed to parse JSON: %s\n", error.text), rt);
    object = json_object_get(root, bq);
    if (json_is_object(object)) {
		json_object_set_new(object, "Month", json_integer(date->m));
		json_object_set_new(object, "Year", json_integer(date->y));
		if (json_dump_file(root, file, JSON_INDENT(3)))
			fprintf(stderr, "Failed to write JSON to file.\n");
		else
			rt = 0;
    }
	else
		fprintf(stderr, "Failed to find 'BP' section in the JSON data.\n");
    return (json_decref(root), rt);
}

// int main() {
// 	t_date date;

// 	json_reader("/home/anas/clones/agios_cal/data/module_tmp.json", "BP", &date);
// 	printf("%d %d\n", date.m, date.y);
// 	json_writer("/home/anas/clones/agios_cal/data/module_tmp.json", "BP", &date);
// }