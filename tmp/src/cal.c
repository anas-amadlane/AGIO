#include <agios.h>

#define DBNAME agios.db

typedef struct	s_val
{
	long long   x;
	s_val		*next;
}	t_val;

typedef struct  s_table
{
	t_val	*debit;
	t_val	*credit;
	t_val	*date_val;
	t_val	*date_op;
}   t_table;

static int  sql_reader(char *bq) {
	sqlite3			*db;
	sqlite3_stmt	*tab;
	char			*query;
	
	if (!bq)
		return 1;
	query = formater(46 + strlen(bq), "SELECT DEBIT, CREDIT, DATE_OP, DATE_VAL FROM %s;", bq);
	if (!query)
		return 1;
	if (init_sql(db, tab, query))
		return (free(query), 1);
	free(query);

}

static int	init_sql(sqlite3 *db, sqlite3_stmt *table, char *query) {
	char	*dbname;

	dbname = joinstr(wd, DBNAME, "/");
	if (!dbname)
		return 1;
	db = NULL;
	table = NULL;
	if (!sqlite3_open(dbname, &db)) {
		if (sqlite3_prepare_v2(db, query, -1, &table, NULL))
			return (printf("%s\n", (char *)sqlite3_errmsg(db), sqlite3_close(db), 1));
	}
	else
		return (printf("%s\n", (char *)sqlite3_errmsg(arg->db), 1));
	return 0;
}