#include <agios.h>

static char *jsonreader(char *file);
static char *joinstr(char *s1, char *s2, char *sep);
static int jsonparser(char *file);

#include <jansson.h>

int main() {
    jsonparser("/home/anas/clones/agios_cal/data/module.json");
}

static int jsonparser(char *file) {
    const char* json_string;
    json_t* root;
    json_error_t error;

    json_string = jsonreader(file);
    root = json_loads(json_string, 0, &error);
    if (!root) {
        fprintf(stderr, "Error parsing JSON: %s\n", error.text);
        return 1;
    }
    json_t* bp_object = json_object_get(root, "BP");
    void *iter = json_object_iter(root);
    //const char** bp_head = json_array_value(json_object_get(bp_object, "head"));
    //const char** bp_head = json_unpack(json_object_get(bp_object, "head"));
    int bp_line = json_integer_value(json_object_get(bp_object, "line"));
    double bp_tx_reg = json_real_value(json_object_get(bp_object, "tx_reg"));
    double bp_tx_pla = json_real_value(json_object_get(bp_object, "tx_pla"));


    printf("BP:\n");
    printf("  head: %s\n", bp_head[0]);
    printf("  line: %d\n", bp_line);
    printf("  tx_reg: %.2f\n", bp_tx_reg);
    printf("  tx_pla: %.2f\n", bp_tx_pla);

    // Cleanup
    json_decref(root);

    return 0;
}

static int jsonwriter(void) {
    // Create a JSON object
    json_t* root = json_object();
    if (!root) {
        fprintf(stderr, "Error creating JSON object\n");
        return 1;
    }

    // Add data to the JSON object
    json_t* bp_object = json_object();
    json_object_set_new(bp_object, "head", json_string("hello, world"));
    json_object_set_new(bp_object, "line", json_integer(50000));
    json_object_set_new(bp_object, "tx_reg", json_real(3.31));
    json_object_set_new(bp_object, "tx_pla", json_real(3.80));
    json_object_set_new(root, "BP", bp_object);

    json_t* bq_object = json_object();
    json_object_set_new(bq_object, "head", json_string("what, world"));
    json_object_set_new(bq_object, "line", json_integer(70000));
    json_object_set_new(bq_object, "tx_reg", json_real(3.24));
    json_object_set_new(bq_object, "tx_pla", json_real(4.10));
    json_object_set_new(root, "BQ", bq_object);

    // Write the JSON object to a file
    const char* filename = "output.json";
    int flags = JSON_INDENT(4) | JSON_ENSURE_ASCII;
    int result = json_dump_file(root, filename, flags);
    if (result != 0) {
        fprintf(stderr, "Error writing JSON to file\n");
    }

    // Cleanup
    json_decref(root);

    return 0;
}


static char *jsonreader(char *file) {
    int     fd;
    int     r;
    char    *tmp[3];

    if (!file)
        return (NULL);
    fd = open(file, O_RDONLY);
    if (!fd)
        return (NULL);
    tmp[0] = (char *)malloc(sizeof(char) * 500);
    if (!tmp[0])
        return (close(fd), NULL);
    r = read(fd, tmp[0], 500);
    while (r && r == 500) {
        r = read(fd, tmp[1], 500);
        tmp[2] = joinstr(tmp[0], tmp[1], NULL);
        if (!tmp[2])
            return (free(tmp[0]), free(tmp[1]), NULL);
        free(tmp[0]);
        tmp[0] = tmp[2];
        free(tmp[1]);
        tmp[1] = NULL;
    }
    return (tmp[0]);
}

// #define AG_TRIM "SELECT NB_DEB_REG, NB_DEB_PLA FROM AG_TRIM_%s_%s;"

static char *joinstr(char *s1, char *s2, char *sep) {
    int     i;
    int     j;
    int     k;
    char    *tmp;

    if (!s1 || !s2)
        return NULL;
    if (!sep)
        sep = "";
    i = -1;
    while (s1[++i]);
    j = -1;
    while (s2[++j]);
    j = -1;
    while (s2[++k]);
    k = k + i + j;
    tmp = (char *)malloc(sizeof(char) * k);
    if (!tmp)
        return NULL;
    i = -1;
    while (s1[++i])
        tmp[i] = s1[i];
    k = -1;
    while (sep[++k])
        tmp[i++] = sep[k];
    j = -1;
    while (s2[++j])
        tmp[i++] = s2[j];
    return (tmp);
}

// static int execute_query(const char *query) {
// 	sqlite3 *db;
// 	sqlite3_stmt *stmt;
// 	int rc;
//     long long   nb[2];

// 	rc = sqlite3_open(dbname, &db);
// 	if (rc != SQLITE_OK)
// 		return (printf("Cannot open database: %s\n", sqlite3_errmsg(db)), rc);
// 	rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
// 	if (rc != SQLITE_OK)
// 		return (sqlite3_close(db), printf("Cannot prepare statement: %s\n", sqlite3_errmsg(db)), rc);
// 	while (sqlite3_step(stmt) == SQLITE_ROW) {
// 		nb[0] = sqlite3_column_int(stmt, 0);
// 		nb[1] = sqlite3_column_int(stmt, 1);
// 	}
// 	sqlite3_finalize(stmt);
// 	sqlite3_close(db);
// 	return SQLITE_OK;
// }