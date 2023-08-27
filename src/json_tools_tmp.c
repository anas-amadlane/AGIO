#define JSON_TOOLS_C
#include <agios.h>


t_rel	*jsonparser(char *file, char *bq) {
	int				i;
	json_t			*root;
	json_t			*object[10];
	t_rel			*releve;
	t_col			*tmp;
	t_col			*holder;
	json_error_t	error;

	if (!file || !bq)
		return NULL;
	releve = (t_rel *)malloc(sizeof(t_rel));
	if (!releve)
		return NULL;
	root = json_load_file(file, 0, &error);
	if (!root)
		return (free(releve),
			fprintf(stderr, "Error parsing JSON from file: %s\n", error.text), NULL);
	object[0] = json_object_get(root, bq);
	object[1] = json_object_get(object[0], "Solde");
	object[2] = json_object_get(object[0], "d_head");
	object[3] = json_object_get(object[0], "line");
	object[4] = json_object_get(object[0], "tx_reg");
	object[5] = json_object_get(object[0], "tx_pla");
	object[6] = json_object_get(object[0], "sk_be");
	object[7] = json_object_get(object[0], "sk_af");
	object[8] = json_object_get(object[0], "Month");
	object[9] = json_object_get(object[0], "year");
	releve->solde = json_real_value(object[1]);
	releve->line.v = json_real_value(json_array_get(object[3], 2));
	// printf("%f\n", releve->line.v);
	releve->line.d_range[0] = ft_strdup(json_string_value(json_array_get(object[3], 0)));
	releve->line.d_range[1] = ft_strdup(json_string_value(json_array_get(object[3], 1)));
	releve->tx_reg.v = json_real_value(json_array_get(object[4], 2));
	releve->tx_reg.d_range[0] = ft_strdup(json_string_value(json_array_get(object[4], 0)));
	releve->tx_reg.d_range[1] = ft_strdup(json_string_value(json_array_get(object[4], 1)));
	releve->tx_pla.v = json_real_value(json_array_get(object[5], 2));
	releve->tx_pla.d_range[0] = ft_strdup(json_string_value(json_array_get(object[5], 0)));
	releve->tx_pla.d_range[1] = ft_strdup(json_string_value(json_array_get(object[5], 1)));
	// printf("%s\n", releve->tx_pla.d_range[0]);
	releve->skip[0] = json_integer_value(object[6]);
	releve->skip[1] = json_integer_value(object[7]);
	releve->month = json_integer_value(object[8]);
	releve->year = json_integer_value(object[9]);
	i = -1;
	holder = NULL;
	while (json_array_get(object[2], ++i)) {
		tmp = (t_col *)malloc(sizeof(t_col));
		if (!tmp)
			return (free(releve), json_decref(root), NULL);
		tmp->index = json_integer_value(json_array_get(json_array_get(object[2], i), 0));
		tmp->colname = ft_strdup(
			json_string_value(json_array_get(json_array_get(object[2], i), 1))
		);
		tmp->next = NULL;
		if (holder)
			holder->next = tmp;
		else
			releve->col = tmp;
		holder = tmp;
	}
	json_decref(root);
	return releve;
}

void	free_relve(t_rel *releve) {
	t_col	*hold[2];

	if (!releve)
		return;
	free(releve->line.d_range[0]), free(releve->line.d_range[1]),
	free(releve->tx_reg.d_range[0]), free(releve->tx_reg.d_range[1]),
	free(releve->tx_pla.d_range[0]), free(releve->tx_pla.d_range[1]),
	hold[0] = releve->col;
	while(hold[0]) {
		free(hold[0]->colname);
		hold[1] = hold[0];
		free(hold[1]);
		hold[0] = hold[0]->next;
	}
	free(releve);
}

int json_writer(char *bq, char *wd, int m, int y) {
    char file[500];
    json_t  *root;
    json_error_t error;

    if (!bq || !wd)
        return 1;
	strcpy(file, wd);
    strcat(file, "/data/module.json");
    if (!file)
        return 1;
    root = json_load_file(file, 0, &error);
    if (!root)
        return (fprintf(stderr, "Failed to parse JSON: %s\n", error.text), 1);
    json_t *bp_obj = json_object_get(root, bq);
    if (!json_is_object(bp_obj)) {
        fprintf(stderr, "Failed to find 'BP' section in the JSON data.\n");
        return (json_decref(root), 1);
    }
    if (m == 12)
        (m = 1, y++);
	else
		m++;
    json_object_set_new(bp_obj, "Month", json_integer(m));
    json_object_set_new(bp_obj, "year", json_integer(y));
    if (json_dump_file(root, file, JSON_INDENT(3))) {
        fprintf(stderr, "Failed to write JSON to file.\n");
        return (json_decref(root), 1);
    }
    return (json_decref(root), 0);
}