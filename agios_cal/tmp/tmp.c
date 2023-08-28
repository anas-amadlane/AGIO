// #include <gtk/gtk.h>
// #include <sqlite3.h>

// // Function to execute the SQL query and retrieve results
// static int execute_query(const char *query, GtkListStore *list_store) {
//     sqlite3 *db;
//     sqlite3_stmt *stmt;
//     int rc;

//     // Open the database connection
//     rc = sqlite3_open("journalier.db", &db);
//     if (rc != SQLITE_OK) {
//         g_print("Cannot open database: %s\n", sqlite3_errmsg(db));
//         return rc;
//     }

//     // Prepare the query statement
//     rc = sqlite3_prepare_v2(db, query, -1, &stmt, NULL);
//     if (rc != SQLITE_OK) {
//         g_print("Cannot prepare statement: %s\n", sqlite3_errmsg(db));
//         sqlite3_close(db);
//         return rc;
//     }

//     // Clear the list store before populating new results
//     gtk_list_store_clear(list_store);

//     // Execute the query and fetch the results
//     while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
//         // Retrieve the columns from the result row
//         int id = sqlite3_column_int(stmt, 0);
//         const char *name = (const char *)sqlite3_column_text(stmt, 1);
//         // Add the retrieved data to the list store
//         GtkTreeIter iter;
//         gtk_list_store_append(list_store, &iter);
//         gtk_list_store_set(list_store, &iter, 0, id, 1, name, -1);
//     }

//     // Finalize the statement and close the database connection
//     sqlite3_finalize(stmt);
//     sqlite3_close(db);

//     return SQLITE_OK;
// }

// // Callback function for the search button
// // static void search_button_clicked(GtkButton *button, gpointer data) {
// //     GtkListStore *list_store = GTK_LIST_STORE(data);
// //     const gchar *search_text = gtk_entry_get_text(GTK_ENTRY(search_text_entry));

// //     // Construct the SQL query with the search criteria
// //     char query[256];
// //     snprintf(query, sizeof(query), "SELECT id, name FROM BCL_VIEW_LODIMED WHERE name LIKE '%%%s%%'", search_text);

// //     // Execute the query and populate the list store with the results
// //     execute_query(query, list_store);
// // }

// // Callback function for the search button
// GtkWidget *search_entry;
// static void search_button_clicked(GtkButton *button, gpointer data) {
//     GtkListStore *list_store = GTK_LIST_STORE(data);
//     const gchar *search_text = gtk_entry_get_text(GTK_ENTRY(search_entry));

//     // Construct the SQL query with the search criteria
//     char query[256];
//     snprintf(query, sizeof(query), "SELECT LAB, MARGE FROM BLC_VIEW_LODIMED WHERE TVA LIKE '%%%s%%'", search_text);

//     // Execute the query and populate the list store with the results
//     execute_query(query, list_store);
// }

// int main(int argc, char *argv[]) {
//     gtk_init(&argc, &argv);

//     // Create the main window and other widgets
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     search_entry = gtk_entry_new();
//     GtkWidget *search_button = gtk_button_new_with_label("Search");
//     GtkWidget *treeview = gtk_tree_view_new();

//     // Create the list store to hold the search results
//     GtkListStore *list_store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
//     gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(list_store));

//     // Add columns to the tree view
//     GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
//     GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
//     gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
//     column = gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 1, NULL);
//     gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

//     // Set up the search button's click event handler
//     g_signal_connect(G_OBJECT(search_button), "clicked", G_CALLBACK(search_button_clicked), list_store);

//     // Create a vertical box container and add the widgets
//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
//     gtk_box_pack_start(GTK_BOX(vbox), search_entry, FALSE, FALSE, 0);
//     gtk_box_pack_start(GTK_BOX(vbox), search_button, FALSE, FALSE, 0);
//     gtk_box_pack_start(GTK_BOX(vbox), treeview, TRUE, TRUE, 0);

//     // Add the vertical box container to the window
//     gtk_container_add(GTK_CONTAINER(window), vbox);

//     // Set other properties of the window
//     gtk_window_set_title(GTK_WINDOW(window), "Search Example");
//     gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
//     g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     // Show the main window and start the GTK main loop
//     gtk_widget_show_all(window);
//     gtk_main();

//     return 0;
// }

// #include <stdio.h>
// #include <jansson.h>

// int main() {
//     const char* json_string = "{\"name\":\"John\",\"age\":30,\"city\":\"New York\"}";

//     // Parse the JSON string
//     json_t* root;
//     json_error_t error;
//     root = json_loads(json_string, 0, &error);

//     // Check if parsing was successful
//     if (!root) {
//         fprintf(stderr, "Error parsing JSON: %s\n", error.text);
//         return 1;
//     }

//     // Extract values from the JSON object
//     const char* name = json_string_value(json_object_get(root, "BP"));
//     printf("%s\n", name);
//     exit(0);
//     int age = json_integer_value(json_object_get(root, "age"));
//     const char* city = json_string_value(json_object_get(root, "city"));

//     // Print the extracted values
//     printf("Name: %s\n", name);
//     printf("Age: %d\n", age);
//     printf("City: %s\n", city);

//     // Cleanup
//     json_decref(root);

//     return 0;
// }

// #include <gtk/gtk.h>

// GtkListBox *listbox;

// void on_search_changed(GtkSearchEntry *search_entry, gpointer user_data) {
//     const gchar *search_text = gtk_entry_get_text(GTK_ENTRY(search_entry));

//     GList *children = gtk_container_get_children(GTK_CONTAINER(listbox));
//     while (children != NULL) {
//         GtkWidget *row = GTK_WIDGET(children->data);
//         GtkWidget *label = gtk_bin_get_child(GTK_BIN(row));
//         const gchar *row_text = gtk_label_get_text(GTK_LABEL(label));
		
//         if (g_strstr_len(row_text, -1, search_text) != NULL) {
//             gtk_widget_show(row);
//         } else {
//             gtk_widget_hide(row);
//         }

//         children = g_list_next(children);
//     }

//     g_list_free(children);
// }

// int main(int argc, char *argv[]) {
//     gtk_init(&argc, &argv);

//     // Create the main window
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Scrollable Box Example");
//     gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
//     gtk_container_set_border_width(GTK_CONTAINER(window), 10);
//     g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     // Create a scrolled window
//     GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
//     gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
//                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

//     // Create the list box to be scrollable and selectable
//     listbox = GTK_LIST_BOX(gtk_list_box_new());
//     gtk_list_box_set_selection_mode(GTK_LIST_BOX(listbox), GTK_SELECTION_MULTIPLE);

//     // Add some content to the list box
//     GtkWidget *label;
//     for (int i = 0; i < 50; i++) {
//         char buffer[50];
//         sprintf(buffer, "Label %d", i + 1);
//         label = gtk_label_new(buffer);
//         gtk_widget_set_hexpand(label, TRUE);
//         gtk_widget_set_halign(label, GTK_ALIGN_START);

//         GtkWidget *row = gtk_list_box_row_new();
//         gtk_container_add(GTK_CONTAINER(row), label);
//         gtk_container_add(GTK_CONTAINER(listbox), row);
//     }

//     // Create the search bar
//     GtkWidget *search_bar = gtk_search_bar_new();
//     gtk_search_bar_set_show_close_button(GTK_SEARCH_BAR(search_bar), TRUE);

//     GtkWidget *search_entry = gtk_search_entry_new();
//     gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Search");

//     g_signal_connect(search_entry, "search-changed", G_CALLBACK(on_search_changed), NULL);
//     gtk_container_add(GTK_CONTAINER(search_bar), search_entry);

//     // Add the list box to the scrolled window
//     gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(listbox));

//     // Create a vertical box to hold the search bar and scrolled window
//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//     gtk_box_pack_start(GTK_BOX(vbox), search_bar, FALSE, FALSE, 0);
//     gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

//     // Add the vertical box to the main window
//     gtk_container_add(GTK_CONTAINER(window), vbox);

//     // Show all the widgets
//     gtk_widget_show_all(window);

//     // Run the GTK main loop
//     gtk_main();

//     return 0;
// }

#include <stdio.h>
#include <jansson.h>
#include <xlsxio_read.h>
#include "libft/libft.h"

typedef struct  s_col
{
	int             index;
	char            *colname;
	struct s_col   *next;
}   t_col;

typedef struct  s_rel
{
	int		solde;
	char    **head;
	int     month;
	int     line;
	int     taux[2];
	int     skip[2];
	t_col   *col;
}   t_rel;


static t_rel	*jsonparser(char *file, char *bq) {
	int				i;
	json_t			*root;
	json_t			*object[9];
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
	object[1] = json_object_get(object[0], "solde");
	object[2] = json_object_get(object[0], "d_head");
	object[3] = json_object_get(object[0], "line");
	object[4] = json_object_get(object[0], "tx_reg");
	object[5] = json_object_get(object[0], "tx_pla");
	object[6] = json_object_get(object[0], "sk_be");
	object[7] = json_object_get(object[0], "sk_af");
	object[8] = json_object_get(object[0], "Month");
	releve->solde = json_integer_value(object[1]);
	releve->line = json_integer_value(object[3]);
	releve->taux[0] = json_real_value(object[4]);
	releve->taux[1] = json_real_value(object[5]);
	releve->skip[0] = json_integer_value(object[6]);
	releve->skip[1] = json_integer_value(object[7]);
	releve->month = json_integer_value(object[8]);
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


// static int	query_holder(char **query, char *s) {
// 	static int	i;
// 	static int	l;
// 	int			j;
// 	char		*tmp;

// 	if (!query || !s)
// 		return 1;
// 	if (!*query) {
// 		l = 5000;
// 		*query = (char *)malloc(sizeof(char) * (l + 1));
// 		if (!*query)
// 			return 1;
// 	}
// 	j = 0;
// 	while (s[j]) {
// 		if (i < l) {
// 			(*query)[i] = s[j++];
// 			i++;
// 		}
// 		else {
// 			l += 2000;
// 			tmp = *query;
// 			*query = (char *)malloc(sizeof(char) * (l + 1));
// 			if (!*query)
// 				return 1;
// 			i = -1;
// 			while (tmp[++i]) {
// 				printf("%d\n", i);
// 				(*query)[i] = tmp[i];
// 			}
// 				printf("%d\n", i);
// 			(*query)[i] = '\0';
// 			free(tmp);
// 		}
// 	}
// 	(*query)[i] = '\0';
// 	return 0;
// }
#include <string.h>
#define MAX_ALLOC 5000
static int	query_holder(char **query, char *s)
{
	int			i;
	int			l;
	static int	len;
	char		*tmp;

	if (*query && s)
	{
		l = ft_strlen(s);
		if (len + l < MAX_ALLOC)
		{
			i = -1;
			while (s[++i])
				(*query)[len++] = s[i];
			(*query)[len] = '\0';
		}
		else
		{
			len += l;
			tmp = *query;
			*query = (char *)malloc(sizeof(char) * (len + 1));
			if (!(*query))
				return (1);
			i = -1;
			while (tmp[++i])
				(*query)[i] = tmp[i];
			l = -1;
			while (s[++l])
				(*query)[i++] = s[l];
			(*query)[i] = '\0';
			free(tmp);
		}
	}
	else if (s)
	{
		len = ft_strlen(s);
		*query = (char *)malloc(sizeof(char) * MAX_ALLOC);
		if (!(*query))
			return (1);
		i = -1;
		while (s[++i])
			(*query)[i] = s[i];
		(*query)[i] = '\0';
	}
	else
		return (1);
	return (0);
}

static int	tider(char **query, int x) {
	int		i;
	char	*tmp;

	if (!x || !query || !*query)
		return 1;
	tmp = *query;
	i = -1;
	while (tmp[++i]);
	while (x && tmp[--i]) {
		if (tmp[i] == ',' && tmp[i - 1] == ')') {
			tmp[i] = ';';
			tmp[i + 1] = '\0';
			x--;
		}
	}
	return 0;
}

// static int	number_form(char **s) {
// 	int	i;
// 	int j;

// 	if (!s || !*s)
// 		return 1;
// 	i = -1;
// 	while ((*s)[++i]) {
// 		if ((*s)[i] == ' ') {
// 			j = i;
// 			while ((*s)[j] && (*s)[j + 1]) {
// 				(*s)[j] = (*s)[j + 1];
// 				j++;
// 			}
// 			(*s)[j] = '\0';
// 		}
// 		else if ((*s)[i] == ',')
// 			(*s)[i] = '.';
// 	}
// 	return 0;
// }

#include <sqlite3.h>

#define C_QUE "DROP TABLE IF EXISTS %s_%s_%s CREATE TABLE %s_%s_%s (DATE_OP DATE, DATE_VA DATE, DEBIT BIGINT, CREDIT BIGINT);"

static int	rep_char(char *s, char c1, char c2) {
	int	i;

	if (!s)
		return 1;
	i = -1;
	while (s[++i]) {
		if (s[i] == c1)
			s[i] = c2;
	}
	return 0;
}

static int	rm_char(char *s, char c) {
	int	i;
	int j;

	if (!s)
		return 1;
	i = -1;
	while (s[++i]) {
		if (s[i] == c) {
			j = i;
			while (s[j] && s[j + 1]) {
				s[j] = s[j + 1];
				j++;
			}
			s[j] = '\0';
		}
	}
	return 0;
}

static int	check_from(char *s, char *bq) {
	if (!s || !bq)
		return 1;
	if (!strcmp(bq, "CIH")) {
		rm_char(s, ' ');
		rep_char(s, ',', '.');
	}
	else if (!strcmp(bq, "BMCI")) {
		rm_char(s, ' ');
		rep_char(s, ',', '.');
	}
	else if (!strcmp(bq, "ARB")) {
		rm_char(s, ',');
		rm_char(s, '-');
	}
	else if (!strcmp(bq, "SGMB"))
		rm_char(s, '-');
	else if (!strcmp(bq, "BMCE"))
		rm_char(s, '-');
	else
		return 1;
	return 0;
}

static int	date_mode(char *s) {
	char	buff[20];
	int		i;
	int		k;
	int		j;

	if (!s)
		return 1;
	i = -1;
	while (s[++i])
		buff[i] = s[i];
	buff[i] = '\0';
	if (i < 10)
		return 1;
	j = i - 4;
	k = 0;
	while (j < i)
		s[k++] = buff[j++];
	s[k++] = '-';
	j = i - 7;
	while (j < i - 5)
		s[k++] = buff[j++];
	s[k++] = '-';
	j = i - 10;
	while (j < i - 8)
		s[k++] = buff[j++];
	s[k] = '\0';
	return 0;
}

static int	date_conv(char **s)
{
	int			unixDate;
    time_t		rawTime;
    struct tm	*timeInfo;
	char		*rt;

    if (!s || !*s)
		return 1;
	rt = (char *)malloc(sizeof(char) * 11);
	if (!rt)
		return 1;
	unixDate = (ft_atoi(*s) - 25569) * 86400;
	rawTime = (time_t)unixDate;
	timeInfo = gmtime(&rawTime);
	sprintf(rt, "%04u-%02u-%02u", timeInfo->tm_year + 1900, timeInfo->tm_mon + 1, timeInfo->tm_mday);
	xlsxioread_free(*s);
	//printf("%s  |  %s\n", *s, rt);
	*s = rt;
	return 0;
}

static int	date_form(char **s, char *bq) {
	if (!s || !bq)
		return 1;
	if (!strcmp(bq, "BMCI") || !strcmp(bq, "ARB"))
		return date_mode(*s);
	else
		return date_conv(s);
}

int init_db(const char *qu1, const char *qu2)
{
	sqlite3	*db;

    db = NULL;
	if (!sqlite3_open("agio.db", &db))
	{
		if (sqlite3_exec(db, qu1, NULL, 0, NULL))
			return 1;
		if (sqlite3_exec(db, qu2, NULL, 0, NULL))
			return 1;
		sqlite3_close(db);
	}
	else
		return 1;
	return 0;
}

static int	init_import(char *file, char *bq, t_rel *releve) {
    int					i;
	int					j;
	int					r;
    int					col;
    int					row;
	const char			*sheetname;
	char				*value;
	xlsxioreader		table;
	xlsxioreadersheet	sheet;
	t_col				*holder;
	char				*query;

	sheetname = NULL;
	if (!file || !releve)
		return 1;
	table = xlsxioread_open(file);
	if (!table)
		return 1;
	query = NULL;
	if (query_holder(&query, "insert into BP_2023_01 values(\'"))
		return 1;
	i = 0;
	j = 0;
	holder = NULL;
	sheet = xlsxioread_sheet_open(table, sheetname, XLSXIOREAD_SKIP_NONE);
	if (sheet) {
		row = 0;
		while (xlsxioread_sheet_next_row(sheet)) {
			col = 0;
			holder = releve->col;
	        value = xlsxioread_sheet_next_cell(sheet);
	        while (value)
	        {
	            if (row > releve->skip[0] && holder && col == holder->index) {
					if (j < 2)
						date_form(&value, bq);
					else
						check_from(value, bq);
					if (i && !j)
						query_holder(&query, "\'),\n(\'");
					query_holder(&query, value);
					holder = holder->next;
					if (holder) {
						//printf("%s, %d, %d, %d, %d\n", value, i, j, row, col);
						query_holder(&query, "\', \'");
						j++;
					}
					else
						j = 0;
					i++;
				}
				xlsxioread_free(value);
	            value = xlsxioread_sheet_next_cell(sheet);
				col++;
	        }
			// if (j && holder) {
			// 	if (row > releve->skip[0] && col == holder->index) {
			// 		j = 0;
			// 		query_holder(&query, "\'),\n(\'");
			// 	}
			// }
			row++;
		}
		query_holder(&query, "\');");
		tider(&query, releve->skip[1]);
		xlsxioread_sheet_close(sheet);
		init_db(C_QUE, query);
	}
	free(query);
	xlsxioread_close(table);
	return 0;
}

int main(int ac, char **ag) {
	t_rel *releve;

	if (ac == 0)
		return 1;
	releve = jsonparser("/home/anas/clones/agios_cal/data/module.json", ag[1]);
	if (!releve)
		return 1;
	init_import(ag[2], ag[1], releve);
}

