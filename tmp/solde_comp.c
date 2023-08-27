#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

int main(int ac, char **av) {
    int             rc;
    int             d;
	sqlite3         *db;
    sqlite3_stmt    *stmt;
    char            buff[200];

    if (ac < 3)
        return 0;
    db = NULL;
	if (!sqlite3_open("agio.db", &db))
	{
        snprintf(buff, 200, "select balance from CONC_OP_SGMB where S_DATE = date('%s', '-1 day');", av[1]);
        rc = sqlite3_prepare_v2(db, buff, -1, &stmt, 0);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
        }
        else {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                const unsigned char *tableName = sqlite3_column_text(stmt, 0);
                if (atoi(tableName) == atoi(av[2]))
                    printf("match\n");
                else
                    printf("fails\n");
            }
        }
		sqlite3_close(db);
        return 1;
    }
	else
		return (printf("error 1\n"), 1);
	return 0;
}