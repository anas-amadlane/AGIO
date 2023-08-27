#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>


typedef struct	s_date {
	int	m;
	int	y;
}	t_date;

int	json_reader(char *file, char *bq, t_date *date) {
	int				rt;
	json_t			*object;
	json_t			*root;
	json_error_t	error;

	rt = 1;
	if (!file || !bq || !date)
		return rt;
	root = json_load_file(file, 0, &error);
	if (!root)
		return (fprintf(stderr, "Failed to parse JSON: %s\n", error.text), rt);
	object = json_object_get(root, bq);
	if (json_is_object(object)) {
		date->m = json_integer_value(json_object_get(object, "Month"));
		date->y = json_integer_value(json_object_get(object, "Year"));
		json_decref(root);
		rt = 0;
	}
	else
		fprintf(stderr, "Failed to find 'BP' section in the JSON data.\n");
	return rt;
}

int	json_writer(char *file, char *bq, t_date *date) {
    int				rt;
	json_t			*object;
    json_t			*root;
    json_error_t	error;

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
// 	json_writer("/home/anas/clones/agios_cal/data/module_tmp.json", "BP");
// }