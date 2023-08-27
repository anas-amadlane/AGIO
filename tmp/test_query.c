#include <sqlite3.h>
#include <stdio.h>
#define AG_TRIM "SELECT NB_DEB_REG, NB_DEB_PLA FROM BL;"

#define AG_REG "SELECT SUM(NB_DEB_REG) FROM BL;"
#define AG_PLA "SELECT SUM(NB_DEB_PLA) FROM BL;"

static int execute_query(long long *nb_reg, long long *nb_pla, const char *query) {
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc;

	rc = sqlite3_open("/home/anas/clones/agios_cal/agio.db", &db);
	if (rc != SQLITE_OK)
		return (printf("Cannot open database: %s\n", sqlite3_errmsg(db)), rc);
	rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
	if (rc != SQLITE_OK)
		return (sqlite3_close(db), printf("Cannot prepare statement: %s\n", sqlite3_errmsg(db)), rc);
	while (sqlite3_step(stmt) == SQLITE_ROW) {
		*nb_reg += sqlite3_column_int(stmt, 0);
		*nb_pla += sqlite3_column_int(stmt, 1);
	}
	sqlite3_finalize(stmt);
	sqlite3_close(db);
	return SQLITE_OK;
}
int main() {
    long long nb_reg;
    long long nb_pla;

    nb_reg = 0;
    nb_pla = 0;
    execute_query(&nb_reg, &nb_pla, AG_TRIM);
    printf("%ld\n", nb_reg);
    printf("%ld\n", nb_pla);
}
