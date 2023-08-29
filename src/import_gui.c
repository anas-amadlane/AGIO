#include <gtk/gtk.h>
#include <sqlite3.h>
#include <jansson.h>

#define APPNAME 5
// #define APPNAME 8

typedef struct	s_var
{
    char    *file;
    char    *bq;
    int     month;
    int		year;
}   t_var;

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

typedef struct s_prov {
	GtkWidget		*button;
	GtkCssProvider	*provider;
}	t_prov;
t_prov provider;

int	import_file(char *wd, t_var *vars);
int init_export(t_var *vars, char *trim, char *wd);
int checker(t_envlop *envlop);
int creater(t_envlop *envlop, double *value);

gchar		*file_path = NULL;
GtkWidget	*imp_frame[3];
GtkWidget	*tra_frame[2];
GtkWidget	*mod_frame;
GtkWidget	*exp_frame;
GtkWidget	*bq_frame;
GtkWidget	*bq_combo;
GtkWidget	*month_label;
GtkWidget	*ticket_label;
GtkWidget   *label[21][4];
GtkWidget	*exp_combo[2];
GtkWidget	*mod_combo;
GtkWidget	*tra_combo[2];
GtkWidget	*bq_spec_frame;
GtkWidget	*bq_spin_button[4];
const char	*appdata;
t_envlop	envlop;
t_date		date;

static void	bzeros(void * s, size_t n) {
	memset(s, 0, n);
}

static int	json_writer(char *file, char *bq, t_date *date) {
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

static int	json_reader(char *file, char *bq, t_date *date) {
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

static void	set_widget_color(const char *color) {
	char	css[100];
	bzeros(css, 100);

	snprintf(css, 100, "button { background-color: %s; }", color);
    GtkStyleContext *context = gtk_widget_get_style_context(provider.button);
	provider.provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data(provider.provider, css, -1, NULL);
	gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider.provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

static void	remove_widget_color(void) {
    GtkStyleContext *context = gtk_widget_get_style_context(provider.button);
    if (provider.provider != NULL) {
        gtk_style_context_remove_provider(context, GTK_STYLE_PROVIDER(provider.provider));
        provider.provider = NULL;
    }
}

// Sidebar GUI
static void	button_clicked(GtkWidget *widget, gpointer data)
{
	(void)data;
	const gchar	*label;

	label = gtk_button_get_label(GTK_BUTTON(widget));
	if (!strcmp(label, "Importer")) {
		gtk_widget_show(bq_frame);
		gtk_widget_show(imp_frame[0]);
		gtk_widget_show(imp_frame[1]);
		gtk_widget_show(imp_frame[2]);
		gtk_widget_hide(exp_frame);
		gtk_widget_hide(tra_frame[0]);
		gtk_widget_hide(tra_frame[1]);
		gtk_widget_hide(bq_spec_frame);
		gtk_widget_hide(mod_frame);
	}
	else if (!strcmp(label, "Exporter")) {
		gtk_widget_show(bq_frame);
		gtk_widget_hide(imp_frame[0]);
		gtk_widget_hide(imp_frame[1]);
		gtk_widget_hide(imp_frame[2]);
		gtk_widget_show(exp_frame);
		gtk_widget_hide(tra_frame[0]);
		gtk_widget_hide(tra_frame[1]);
		gtk_widget_hide(mod_frame);
		gtk_widget_hide(bq_spec_frame);
	}
	else if (!strcmp(label, "Tracker")) {
		gtk_widget_show(bq_frame);
		gtk_widget_hide(imp_frame[0]);
		gtk_widget_hide(imp_frame[1]);
		gtk_widget_hide(imp_frame[2]);
		gtk_widget_hide(exp_frame);
		gtk_widget_show(tra_frame[0]);
		gtk_widget_show(tra_frame[1]);
		gtk_widget_hide(mod_frame);
		gtk_widget_hide(bq_spec_frame);
	}
	else if (!strcmp(label, "Modifier")) {
		gtk_widget_show(bq_frame);
		gtk_widget_hide(imp_frame[0]);
		gtk_widget_hide(imp_frame[1]);
		gtk_widget_hide(imp_frame[2]);
		gtk_widget_hide(exp_frame);
		gtk_widget_hide(tra_frame[0]);
		gtk_widget_hide(tra_frame[1]);
		gtk_widget_show(mod_frame);
		gtk_widget_hide(bq_spec_frame);
	}
}

static void	sidebar_part(GtkWidget *box) {
	GtkWidget	*sidebar;
	GtkWidget	*logo_box;
	GtkWidget	*buttons_box;
	GtkWidget	*imp_button;
	GtkWidget	*exp_button;
	GtkWidget	*tra_button;
	GtkWidget	*mod_button;
	GtkWidget	*footer_label;
	char		path[500];

	(bzeros(path, 500), strcat(path, appdata), strcat(path, "/data/logo.png"));
	sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(sidebar, "sidebar");
	gtk_box_pack_start(GTK_BOX(box), sidebar, FALSE, FALSE, 0);
	gtk_widget_set_valign(sidebar, GTK_ALIGN_CENTER);
	// Sidebar header
	logo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(sidebar), logo_box, FALSE, FALSE, 10);
	gtk_widget_set_halign(logo_box, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(logo_box), gtk_image_new_from_file(path), FALSE, FALSE, 0);
	// Header separator
	gtk_box_pack_start(GTK_BOX(sidebar), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);
	// Buttons
	buttons_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(sidebar), buttons_box, FALSE, FALSE, 10);
	gtk_widget_set_halign(buttons_box, GTK_ALIGN_CENTER);
	imp_button = gtk_button_new_with_label("Importer");
	exp_button = gtk_button_new_with_label("Exporter");
	tra_button = gtk_button_new_with_label("Tracker");
	mod_button = gtk_button_new_with_label("Modifier");
	// set_widget_color(imp_button, "#41B64A");
	// set_widget_color(exp_button, "#41B64A");
	// set_widget_color(tra_button, "#41B64A");
	// set_widget_color(mod_button, "#41B64A");
	g_signal_connect(imp_button, "clicked", G_CALLBACK(button_clicked), NULL);
	g_signal_connect(exp_button, "clicked", G_CALLBACK(button_clicked), NULL);
	g_signal_connect(tra_button, "clicked", G_CALLBACK(button_clicked), NULL);
	g_signal_connect(mod_button, "clicked", G_CALLBACK(button_clicked), NULL);
	gtk_box_pack_start(GTK_BOX(buttons_box), imp_button, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(buttons_box), exp_button, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(buttons_box), tra_button, FALSE, FALSE, 5);
	gtk_box_pack_start(GTK_BOX(buttons_box), mod_button, FALSE, FALSE, 5);
	// Footer separator
	gtk_box_pack_start(GTK_BOX(sidebar), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 10);
	// Sidebar Footer
	footer_label = gtk_label_new("© 2023 Soremed. All right reserved.");
	gtk_box_pack_end(GTK_BOX(sidebar), footer_label, FALSE, FALSE, 10);
	gtk_widget_set_halign(footer_label, GTK_ALIGN_CENTER);
}

// BQ BALANCE LINE TAUX
static void	on_calendar_date_selected(GtkCalendar *calendar, gpointer user_data) {
    (void)user_data;
	guint year, month, day;
    gtk_calendar_get_date(calendar, &year, &month, &day);
	envlop.date.d = day;
	envlop.date.m = month + 1;
	envlop.date.y = year;
}

static void	save_clicked(GtkComboBox *combo_box, gpointer user_data) {
	(void)combo_box;
	(void)user_data;
	double	value[4];
	gpointer	selectedBq;
	char		path[500];

	value[3] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(bq_spin_button[0]));
	value[0] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(bq_spin_button[1]));
	value[1] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(bq_spin_button[2]));
	value[2] = gtk_spin_button_get_value(GTK_SPIN_BUTTON(bq_spin_button[3]));
	if (value[0] && value[1] && value[2] && value[3]) {
		selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
		if (!selectedBq)
			return;
		if (envlop.bq)
			free(envlop.bq);
		envlop.bq = (char *)selectedBq;
		creater(&envlop, value);
		(bzeros(path, 500), strcat(path, appdata), strcat(path, "/data/module.json"));
        json_writer(path, envlop.bq, &envlop.date);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(bq_spin_button[0]), 0);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(bq_spin_button[1]), 0);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(bq_spin_button[2]), 0);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(bq_spin_button[3]), 0);
		snprintf(path, 500, "<span font_size=\"15000\">%d/%d</span>", envlop.date.m, envlop.date.y);
		gtk_label_set_markup(GTK_LABEL(month_label), path);
		gtk_widget_show(bq_frame);
		gtk_widget_show(imp_frame[0]);
		gtk_widget_show(imp_frame[1]);
		gtk_widget_show(imp_frame[2]);
		gtk_widget_hide(exp_frame);
		gtk_widget_hide(tra_frame[0]);
		gtk_widget_hide(tra_frame[1]);
		gtk_widget_hide(bq_spec_frame);
		gtk_widget_hide(mod_frame);
	}
}

static void cancel_clicked(GtkComboBox *combo_box, gpointer user_data) {
	(void)combo_box;
	(void)user_data;

	gtk_combo_box_set_active(GTK_COMBO_BOX(bq_combo), -1);
	gtk_widget_show(bq_frame);
	gtk_widget_show(imp_frame[0]);
	gtk_widget_show(imp_frame[1]);
	gtk_widget_show(imp_frame[2]);
	gtk_widget_hide(exp_frame);
	gtk_widget_hide(tra_frame[0]);
	gtk_widget_hide(tra_frame[1]);
	gtk_widget_hide(bq_spec_frame);
	gtk_widget_hide(mod_frame);
}

static void	bqspec_part(GtkWidget *content_area) {
	GtkWidget	*grid;
	GtkWidget	*space[7];
	GtkWidget	*bq_spec_label[6];
	GtkWidget	*bq_spec_button[2];

	bq_spec_frame = gtk_frame_new("Banque détail");
	gtk_frame_set_label_align(GTK_FRAME(bq_spec_frame), 0.5, 0.5);
	grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_container_add(GTK_CONTAINER(bq_spec_frame), grid);
	gtk_container_add(GTK_CONTAINER(content_area), bq_spec_frame);
	// Border
	space[0] = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), space[0], 0, 0, 3, 1);
	// Date
	bq_spec_label[1] = gtk_label_new("Selected date");
	gtk_grid_attach(GTK_GRID(grid), bq_spec_label[1], 0, 1, 1, 2);
	GtkWidget *bq_date = gtk_calendar_new();
	g_signal_connect(bq_date, "day-selected", G_CALLBACK(on_calendar_date_selected), NULL);
	gtk_grid_attach(GTK_GRID(grid), bq_date, 0, 3, 1, 5);
	// solde
	bq_spec_label[2] = gtk_label_new("Solde");
	bq_spin_button[0] = gtk_spin_button_new(gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 0.01, 0.1, 0.0), 0.01, 2);
	gtk_grid_attach(GTK_GRID(grid), bq_spec_label[2], 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), bq_spin_button[0], 2, 1, 1, 1);
	space[1] = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), space[1], 1, 2, 2, 1);
	// Line
	bq_spec_label[3] = gtk_label_new("Line");
	bq_spin_button[1] = gtk_spin_button_new(gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 0.01, 0.1, 0.0), 0.01, 2);
	gtk_grid_attach(GTK_GRID(grid), bq_spec_label[3], 1, 3, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), bq_spin_button[1], 2, 3, 1, 1);
	space[2] = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), space[2], 1, 4, 2, 1);
	// TREG
	bq_spec_label[4] = gtk_label_new("TREG");
	bq_spin_button[2] = gtk_spin_button_new(gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 0.01, 0.1, 0.0), 0.01, 2);
	gtk_grid_attach(GTK_GRID(grid), bq_spec_label[4], 1, 5, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), bq_spin_button[2], 2, 5, 1, 1);
	space[3] = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), space[3], 1, 6, 2, 1);
	// TPLA
	bq_spec_label[5] = gtk_label_new("TPLA");
	bq_spin_button[3] = gtk_spin_button_new(gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 0.01, 0.1, 0.0), 0.01, 2);
	gtk_grid_attach(GTK_GRID(grid), bq_spec_label[5], 1, 7, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), bq_spin_button[3], 2, 7, 1, 1);
	space[4] = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), space[4], 0, 8, 3, 2);
	// Button
	bq_spec_button[0] = gtk_button_new_with_label("Cancel");
	bq_spec_button[1] = gtk_button_new_with_label("Save");
	gtk_grid_attach(GTK_GRID(grid), bq_spec_button[0], 0, 10, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), bq_spec_button[1], 1, 10, 2, 1);
	space[5] = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), space[5], 0, 11, 3, 1);
    g_signal_connect(bq_spec_button[0], "clicked", G_CALLBACK(cancel_clicked), NULL);
    g_signal_connect(bq_spec_button[1], "clicked", G_CALLBACK(save_clicked), NULL);
}

// Import part
static void	on_combo_changed(GtkComboBox *combo_box, gpointer user_data) {
	(void)combo_box;
	(void)user_data;
	gpointer	selectedBq;
	char		buff[100];
    char    path[500];

	selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
	if (!selectedBq)
		return;
	if (envlop.bq)
		free(envlop.bq);
	envlop.bq = (char *)selectedBq;
    if (checker(&envlop) == 1) {
		gtk_widget_hide(bq_frame);
		gtk_widget_hide(imp_frame[0]);
		gtk_widget_hide(imp_frame[1]);
		gtk_widget_hide(imp_frame[2]);
		gtk_widget_hide(exp_frame);
		gtk_widget_hide(tra_frame[0]);
		gtk_widget_hide(tra_frame[1]);
		gtk_widget_hide(mod_frame);
		gtk_widget_show(bq_spec_frame);
		gtk_label_set_markup(GTK_LABEL(month_label), " ");
    }
	else {
		(bzeros(path, 500), strcat(path, appdata), strcat(path, "/data/module.json"));
		json_reader(path, selectedBq, &envlop.date);
		snprintf(buff, 100, "<span font_size=\"15000\">%d/%d</span>", envlop.date.m, envlop.date.y);
		gtk_label_set_markup(GTK_LABEL(month_label), buff);
	}
}


static void	on_cloture_clicked(GtkButton *button, gpointer user_data) {
	(void)button;
    sqlite3				*db;
	sqlite3_stmt		*stmt;
	int					value;
	gpointer			selectedBq;
	char				buff[200];
	char				path[500];
	const  char	*tableName;

	db = NULL;
	selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
	if (!selectedBq)
		return ;
	value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(user_data));
	(bzeros(path, 500), strcat(path, appdata), strcat(path, "/data/module.json"));
	if (!sqlite3_open(envlop.db_path, &db))
	{
		bzeros(buff, 200);
		if (envlop.date.m < 12) {
			if (envlop.date.m < 10)
				snprintf(buff, 200, "select balance from CONC_OP_%s_%d where S_DATE = date('%d-0%d-01', '-1 day');", (char *)selectedBq, envlop.date.y, envlop.date.y, envlop.date.m+1);
			else
				snprintf(buff, 200, "select balance from CONC_OP_%s_%d where S_DATE = date('%d-%d-01', '-1 day');", (char *)selectedBq, envlop.date.y, envlop.date.y, envlop.date.m+1);
		}
		else
			snprintf(buff, 200, "select balance from CONC_OP_%s_%d where S_DATE = '%d-12-31';", (char *)selectedBq, envlop.date.y, envlop.date.y);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK)
			fprintf(stderr, "Failed to execute statement: %s\n", sqlite3_errmsg(db));
        else if (sqlite3_step(stmt) == SQLITE_ROW) {
			tableName = (char *)sqlite3_column_text(stmt, 0);
            if (atoi(tableName) == value) {
				if (envlop.date.m < 12)
					envlop.date.m += 1;
				else {
					bzeros(buff, 200);
					snprintf(buff, 200, "UPDATE BQ_SPEC SET SOLDE = (SELECT BALANCE FROM CONC_OP_%s_%d WHERE S_date = '%d-12-31') WHERE BQ = '%s';", (char *)selectedBq, envlop.date.y, envlop.date.y, (char *)selectedBq);
					sqlite3_exec(db, buff, NULL, NULL, NULL);
					(envlop.date.m = 1, envlop.date.y += 1);
				}
				json_writer(path, (char *)selectedBq, &envlop.date);
				gtk_spin_button_set_value(GTK_SPIN_BUTTON(user_data), 0.0);
			}
			sqlite3_finalize(stmt);
        }
		sqlite3_close(db);
    }
	json_reader(path, selectedBq, &envlop.date);
	snprintf(buff, 100, "<span font_size=\"15000\">%d/%d</span>", envlop.date.m, envlop.date.y);
	gtk_label_set_markup(GTK_LABEL(month_label), buff);
	g_free(selectedBq);
}

static void	on_file_selected(GtkFileChooserButton *chooser_button, gpointer user_data) {
	(void)user_data;
	remove_widget_color();
	if (file_path)
    	g_free(file_path);
	file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser_button));
}

static void	on_import_clicked(GtkWidget *widget, gpointer user_data) {
	(void)widget;
	(void)user_data;
	t_var		vars;
	gpointer	selectedBq;

	selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
	if (!file_path || !selectedBq)
		return ;
	vars.file = file_path;
	vars.bq = (char *)selectedBq;
	vars.month = envlop.date.m;
	vars.year = envlop.date.y;
	if (!import_file(envlop.wd, &vars))
		set_widget_color("#41B64A");
	else
		set_widget_color("#FF0000");
	g_free(vars.bq);
}

static void	import_part(GtkWidget *content_area) {
	GtkWidget	*grid;
	GtkWidget	*cloture_label;
	GtkWidget	*cloture_box;
	GtkWidget	*cloture_button;
	GtkWidget	*spin_button;
	GtkWidget	*drag_area;
	GtkWidget	*import_button;
	GtkWidget	*import_box;
	GtkFileFilter	*file_filter;

	imp_frame[0] = gtk_frame_new("Mois/Année en cours");
	gtk_frame_set_shadow_type(GTK_FRAME(imp_frame[0]), GTK_SHADOW_IN);
    gtk_container_set_border_width(GTK_CONTAINER(imp_frame[0]), 20);
	imp_frame[1] = gtk_frame_new("Clôture du mois");
	imp_frame[2] = gtk_frame_new("Importion du relevé");
	gtk_frame_set_label_align(GTK_FRAME(imp_frame[0]), 0.5, 0.5);
	gtk_frame_set_label_align(GTK_FRAME(imp_frame[1]), 0.5, 0.5);
	gtk_frame_set_label_align(GTK_FRAME(imp_frame[2]), 0.5, 0.5);
	// Current Month/Year
    month_label = gtk_label_new(NULL);
    gtk_container_add(GTK_CONTAINER(imp_frame[0]), month_label);
	gtk_container_add(GTK_CONTAINER(content_area), imp_frame[0]);
	// grid
	grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_grid_attach(GTK_GRID(grid), imp_frame[1], 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), imp_frame[2], 1, 0, 1, 1);
	gtk_container_add(GTK_CONTAINER(content_area), grid);
	// Cloture part
	cloture_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    cloture_label = gtk_label_new("veuillez entrer votre solde de clôture :");
    spin_button = gtk_spin_button_new(gtk_adjustment_new(0.0,
			-G_MAXDOUBLE, G_MAXDOUBLE, 0.01, 0.1, 0.0), 0.01, 2);
    cloture_button = gtk_button_new_with_label("Cloturer");
    g_signal_connect(cloture_button, "clicked", G_CALLBACK(on_cloture_clicked), spin_button);
	gtk_box_pack_start(GTK_BOX(cloture_box), cloture_label, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(cloture_box), spin_button, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(cloture_box), cloture_button, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(imp_frame[1]), cloture_box);
	// Import part
	import_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  	drag_area = gtk_file_chooser_button_new("Select file", GTK_FILE_CHOOSER_ACTION_OPEN);
    file_filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(file_filter, "*.xls");
    gtk_file_filter_add_pattern(file_filter, "*.xlsx");
    gtk_file_filter_add_pattern(file_filter, "*.csv");
    gtk_file_filter_set_name(file_filter, "Excel and CSV Files");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(drag_area), file_filter);
    g_signal_connect(drag_area, "file-set", G_CALLBACK(on_file_selected), NULL);
    gtk_box_pack_start(GTK_BOX(import_box), drag_area, TRUE, TRUE, 5);
	import_button = gtk_button_new_with_label("Importer");
	provider.button = import_button;
	g_signal_connect(import_button, "clicked", G_CALLBACK(on_import_clicked), NULL);
	gtk_box_pack_start(GTK_BOX(import_box), import_button, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(imp_frame[2]), import_box);
}

// Export part
static void	on_export_clicked(GtkWidget *widget, gpointer data) {
	(void)widget;
	GtkWidget				*dialog;
	GtkFileChooser			*chooser;
	gint					res;
	GtkFileChooserAction	action;
	t_var					vars;
	char					*filename;
	gpointer			 	selectedBq;
	gpointer 				selectedYear;
	gpointer 				selectedtrim;
	char 					*trim;

	action = GTK_FILE_CHOOSER_ACTION_SAVE;
	dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(data), action, "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL);
	chooser = GTK_FILE_CHOOSER(dialog);
	gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);
	gtk_file_chooser_set_current_name(chooser, "Ticket.xlsx");
	res = gtk_dialog_run(GTK_DIALOG(dialog));
	if (res == GTK_RESPONSE_ACCEPT) {
		chooser = GTK_FILE_CHOOSER(dialog);
		filename = gtk_file_chooser_get_filename(chooser);
		vars.file = filename;
		selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
		selectedtrim = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(exp_combo[0]));
		selectedYear = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(exp_combo[1]));
		if (!selectedBq || !selectedYear || !selectedtrim) {
			gtk_widget_destroy(dialog);
			return ;
		}
		vars.bq = (char *)selectedBq;
		vars.year = atoi((char *)selectedYear);
		trim = (char *)selectedtrim;
		init_export(&vars, trim, envlop.wd);
		g_free(vars.file);
		g_free(selectedYear);
		g_free(vars.bq);
		g_free(trim);
	}
	gtk_widget_destroy(dialog);
}

static void	export_part(GtkWidget *content_area) {
	GtkWidget	*exp_box;
	GtkWidget	*exp_button;
	char		yearStr[20];
	int			currentYear;
	int			currentYearIndex;
	time_t		t;

	exp_frame = gtk_frame_new("Export tickets");
	gtk_frame_set_label_align(GTK_FRAME(exp_frame), 0.5, 0.5);
	exp_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(exp_box), TRUE);
	// trimestre combo
	exp_combo[0] = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(exp_combo[0]), "1er trimestre");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(exp_combo[0]), "2éme trimestre");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(exp_combo[0]), "3éme trimestre");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(exp_combo[0]), "4éme trimestre");
	gtk_box_pack_start(GTK_BOX(exp_box), exp_combo[0], TRUE, TRUE, 5);
	// year combo
    t = time(NULL);
    struct tm tm = *localtime(&t);
	currentYear = tm.tm_year+1900;
	exp_combo[1] = gtk_combo_box_text_new();
	for (int year = currentYear - 10; year <= currentYear + 10; ++year) {
		snprintf(yearStr, 20, "%d", year);
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(exp_combo[1]), yearStr);
	}
	currentYearIndex = currentYear - (currentYear - 10);
	gtk_combo_box_set_active(GTK_COMBO_BOX(exp_combo[1]), currentYearIndex);
	gtk_box_pack_start(GTK_BOX(exp_box), exp_combo[1], TRUE, TRUE, 5);
	// button
	exp_button = gtk_button_new_with_label("Exporter");
	g_signal_connect(exp_button, "clicked", G_CALLBACK(on_export_clicked), NULL);
	gtk_box_pack_start(GTK_BOX(exp_box), exp_button, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(exp_frame), exp_box);
	gtk_container_add(GTK_CONTAINER(content_area), exp_frame);
}

// Track part
#define TR1 {"Janvier", "Feverier", "Mars", NULL}
#define TR2 {"Janvier", "Feverier", "Mars", NULL}
#define TR3 {"Janvier", "Feverier", "Mars", NULL}
#define TR4 {"Janvier", "Feverier", "Mars", NULL}
#define TD1 " WHERE DATE BETWEEN '%s-01' AND '%s-04'"
#define TD2 " WHERE DATE BETWEEN '%s-04' AND '%s-07'"
#define TD3 " WHERE DATE BETWEEN '%s-07' AND '%s-10'"
#define TD4 " WHERE DATE BETWEEN '%s-10' AND '%s-12-31'"
#define NB_DEB "CREATE VIEW IF NOT EXISTS NB_DEB_%s AS WITH NB_DEB_%s AS (SELECT DATE(S_DATE) AS DATE, CASE WHEN BALANCE >= 0 THEN BALANCE ELSE 0 END AS NB_CRE, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) THEN -1 * BALANCE ELSE (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) END AS NB_DEB_REG, CASE WHEN BALANCE >= 0 THEN 0 WHEN -1 * BALANCE <= (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) THEN 0 ELSE -1 * BALANCE - (SELECT TAUX FROM LINE_%s WHERE EDATE>S_DATE) END AS NB_DEB_PLA FROM CONC_OP_%s_%s) SELECT DATE, NB_CRE, NB_DEB_REG, NB_DEB_PLA, ((NB_DEB_REG * (SELECT TAUX FROM TREG_%s WHERE EDATE>DATE)/100)/360 + (NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s WHERE EDATE>DATE)/100)/360) AS INTER FROM NB_DEB_%s;"

static void on_track_clicked(GtkButton *button, gpointer user_data) {
	(void)button;
    int             i;
    int             j;
    sqlite3         *db;
	sqlite3_stmt    *stmt;
	gpointer		selectedBq;
	gpointer		selectedtrim;
	gpointer		selectedYear;
    char            buff[2000];
	char            dt_d[100];
	char            *dt_r[4];
	int             value;

    db = NULL;
	value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(user_data));
	selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
	selectedtrim = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(tra_combo[0]));
	selectedYear = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(tra_combo[1]));
	if (!selectedBq || !selectedYear || !selectedtrim)
		return ;
	if (!strcmp(selectedtrim, "1er trimestre"))
	{
		(dt_r[0] = "Janvier", dt_r[1] = "Feverier", dt_r[2] = "Mars", dt_r[3] = NULL);
		snprintf(dt_d, 100, TD1, (char *)selectedYear, (char *)selectedYear);
	}
	else if (!strcmp(selectedtrim, "2éme trimestre")) {

		(dt_r[0] = "Janvier", dt_r[1] = "Feverier", dt_r[2] = "Mars", dt_r[3] = NULL);
		snprintf(dt_d, 100, TD2, (char *)selectedYear, (char *)selectedYear);
	}
	else if (!strcmp(selectedtrim, "3éme trimestre")) {

		(dt_r[0] = "Janvier", dt_r[1] = "Feverier", dt_r[2] = "Mars", dt_r[3] = NULL);
		snprintf(dt_d, 100, TD3, (char *)selectedYear, (char *)selectedYear);
	}
	else {

		(dt_r[0] = "Janvier", dt_r[1] = "Feverier", dt_r[2] = "Mars", dt_r[3] = NULL);
		snprintf(dt_d, 100, TD4, (char *)selectedYear, (char *)selectedYear);
	}
    i = -1;
    while (++i < 21) {
        j = -1;
        while (++j < 4)
			gtk_label_set_text(GTK_LABEL(label[i][j]), NULL);
    }
	if (!sqlite3_open(envlop.db_path, &db))
	{
		bzeros(buff, 2000);
		snprintf(buff, 2000, NB_DEB, (char *)selectedBq, (char *)selectedBq, (char *)selectedBq, (char *)selectedBq, (char *)selectedBq, (char *)selectedBq, (char *)selectedBq, (char *)selectedYear, (char *)selectedBq, (char *)selectedBq, (char *)selectedBq);
		sqlite3_exec(db, buff, NULL, NULL, NULL);
		bzeros(buff, 2000);
		snprintf(buff, 300, "select round(sum(NB_DEB_REG), 1) from NB_DEB_%s %s group by strftime('%%m', date);", (char *)selectedBq, dt_d);
        if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
		snprintf(buff, 500, "CALCUL TICKET AGIOS %s %s", (char *)selectedBq, (char *)selectedtrim);
        gtk_label_set_text(GTK_LABEL(label[0][1]), buff);
        gtk_label_set_text(GTK_LABEL(label[3][0]), "Nbres Débiteurs  ");
		i = 0;
		while (dt_r[i]) {
			gtk_label_set_text(GTK_LABEL(label[4][i + 1]), dt_r[i]);
			i++;
		}
        gtk_label_set_text(GTK_LABEL(label[5][0]), "Autorisé           ");
        i = 1;
        while (sqlite3_step(stmt) == SQLITE_ROW)
            gtk_label_set_text(GTK_LABEL(label[5][i++]), (char *)sqlite3_column_text(stmt, 0));
        gtk_label_set_text(GTK_LABEL(label[6][0]), "Dépassement      ");
		snprintf(buff, 300, "select round(sum(NB_DEB_PLA), 1) from NB_DEB_%s %s group by strftime('%%m', date);", (char *)selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", (char *)sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
        i = 1;
        while (sqlite3_step(stmt) == SQLITE_ROW)
            gtk_label_set_text(GTK_LABEL(label[6][i++]), (char *)sqlite3_column_text(stmt, 0));
        gtk_label_set_text(GTK_LABEL(label[7][0]), "Total Nb_Déb      ");
		snprintf(buff, 300, "select round(sum(NB_DEB_REG) + sum(NB_DEB_PLA), 1) from NB_DEB_%s %s group by strftime('%%m', date);", (char *)selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", (char *)sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
        i = 1;
        while (sqlite3_step(stmt) == SQLITE_ROW)
            gtk_label_set_text(GTK_LABEL(label[7][i++]), (char *)sqlite3_column_text(stmt, 0));
        gtk_label_set_text(GTK_LABEL(label[10][0]), "Intérêt /Nb_Déb  ");
		i = 0;
		while (dt_r[i]) {
			gtk_label_set_text(GTK_LABEL(label[11][i + 1]), dt_r[i]);
			i++;
		}
        gtk_label_set_text(GTK_LABEL(label[12][0]), "Autorisé           ");
		snprintf(buff, 300, "select round(sum(NB_DEB_REG * (SELECT TAUX FROM TREG_%s)/100/360), 1) from NB_DEB_%s %s group by strftime('%%m', date);", (char *)selectedBq, (char *)selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
        i = 1;
        while (sqlite3_step(stmt) == SQLITE_ROW)
            gtk_label_set_text(GTK_LABEL(label[12][i++]), (char *)sqlite3_column_text(stmt, 0));
        gtk_label_set_text(GTK_LABEL(label[13][0]), "Dépassement      ");
		snprintf(buff, 300, "select round(sum(NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s)/100/360), 1) from NB_DEB_%s %s group by strftime('%%m', date);", (char *)selectedBq, (char *)selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
        i = 1;
        while (sqlite3_step(stmt) == SQLITE_ROW)
            gtk_label_set_text(GTK_LABEL(label[13][i++]), (char *)sqlite3_column_text(stmt, 0));
        gtk_label_set_text(GTK_LABEL(label[14][0]), "Total Intérêt     ");
		snprintf(buff, 300, "select round(sum(NB_DEB_REG * (SELECT TAUX FROM TREG_%s)/100/360) + sum(NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s)/100/360), 1) from NB_DEB_%s %s group by strftime('%%m', date);", (char *)selectedBq, (char *)selectedBq, (char *)selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
        i = 1;
        while (sqlite3_step(stmt) == SQLITE_ROW)
            gtk_label_set_text(GTK_LABEL(label[14][i++]), (char *)sqlite3_column_text(stmt, 0));
        gtk_label_set_text(GTK_LABEL(label[16][0]), "Intérêts calculés");
		snprintf(buff, 300, "select round(sum(NB_DEB_REG * (SELECT TAUX FROM TREG_%s)/100/360) + sum(NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s)/100/360), 1) from NB_DEB_%s %s;", (char *)selectedBq, (char *)selectedBq, (char *)selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
        else if (sqlite3_step(stmt) == SQLITE_ROW)
            gtk_label_set_text(GTK_LABEL(label[16][1]), (char *)sqlite3_column_text(stmt, 0));
        gtk_label_set_text(GTK_LABEL(label[17][0]), "Intérêts Débités ");


		snprintf(buff, 300, "%d", value);
        gtk_label_set_text(GTK_LABEL(label[17][1]), buff);
        gtk_label_set_text(GTK_LABEL(label[18][0]), "Différence        ");
		snprintf(buff, 300, "select round((sum(NB_DEB_REG * (SELECT TAUX FROM TREG_%s)/100/360) + sum(NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s)/100/360))-%d, 1) from NB_DEB_%s %s;", (char *)selectedBq, (char *)selectedBq, value, (char *)selectedBq, dt_d);
        if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
        else if (sqlite3_step(stmt) == SQLITE_ROW) {
            snprintf(buff, 300, "%s", sqlite3_column_text(stmt, 0));
            gtk_label_set_text(GTK_LABEL(label[18][1]), buff);
        }
        gtk_label_set_text(GTK_LABEL(label[19][0]), "Taux Moyenne     ");
		snprintf(buff, 300, "SELECT ROUND(((SUM(NB_DEB_REG * (SELECT TAUX FROM TREG_%s)/100/360) + SUM(NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s)/100/360))/(SUM(NB_DEB_REG) + SUM(NB_DEB_PLA)))*360*100, 2) FROM NB_DEB_%s %s;", (char *)selectedBq, (char *)selectedBq, (char *)selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
        else if (sqlite3_step(stmt) == SQLITE_ROW) {
            snprintf(buff, 300, "%s%%", sqlite3_column_text(stmt, 0));
            gtk_label_set_text(GTK_LABEL(label[19][1]), buff);
        }
		sqlite3_close(db);
    }
}

static void	track_part(GtkWidget *content_area) {
	GtkWidget	*tra_box;
	GtkWidget	*tra_button;
	char		yearStr[20];
	int			currentYear;
	int			currentYearIndex;
	time_t		t;

	tra_frame[0] = gtk_frame_new("Get Agio Ticket");
	gtk_frame_set_label_align(GTK_FRAME(tra_frame[0]), 0.5, 0.5);
	tra_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(tra_box), TRUE);
    GtkWidget *tra_label = gtk_label_new("veuillez entrer l'intérêts Débités");
    GtkWidget *spin_button = gtk_spin_button_new(gtk_adjustment_new(0.0,
			-G_MAXDOUBLE, G_MAXDOUBLE, 0.01, 0.1, 0.0), 0.01, 2);
	gtk_box_pack_start(GTK_BOX(tra_box), tra_label, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(tra_box), spin_button, TRUE, TRUE, 5);
	// trimestre combo
	tra_combo[0] = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(tra_combo[0]), "1er trimestre");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(tra_combo[0]), "2éme trimestre");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(tra_combo[0]), "3éme trimestre");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(tra_combo[0]), "4éme trimestre");
	gtk_box_pack_start(GTK_BOX(tra_box), tra_combo[0], TRUE, TRUE, 5);
	// year combo
    t = time(NULL);
    struct tm tm = *localtime(&t);
	currentYear = tm.tm_year+1900;
	tra_combo[1] = gtk_combo_box_text_new();
	for (int year = currentYear - 10; year <= currentYear + 10; ++year) {
		snprintf(yearStr, 20, "%d", year);
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(tra_combo[1]), yearStr);
	}
	currentYearIndex = currentYear - (currentYear - 10);
	gtk_combo_box_set_active(GTK_COMBO_BOX(tra_combo[1]), currentYearIndex);
	gtk_box_pack_start(GTK_BOX(tra_box), tra_combo[1], TRUE, TRUE, 5);
	// button
	tra_button = gtk_button_new_with_label("Tracker");
	g_signal_connect(tra_button, "clicked", G_CALLBACK(on_track_clicked), spin_button);
	gtk_box_pack_start(GTK_BOX(tra_box), tra_button, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(tra_frame[0]), tra_box);
	gtk_container_add(GTK_CONTAINER(content_area), tra_frame[0]);
	tra_frame[1] = gtk_frame_new("TICKET AGIOS");
	gtk_frame_set_shadow_type(GTK_FRAME(tra_frame[1]), GTK_SHADOW_IN);
    gtk_container_set_border_width(GTK_CONTAINER(tra_frame[1]), 20);
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    int i;
    int j;
    i = -1;
    while (++i < 21) {
        j = -1;
        while (++j < 4) {
            label[i][j] = gtk_label_new(NULL);
            if (j == 0)
            gtk_label_set_xalign(GTK_LABEL(label[i][j]), 0.0);
        }
    }
    i = -1;
    while (++i < 21) {
        j = -1;
        while (++j < 4) {
            if (i == 0)
                gtk_grid_attach(GTK_GRID(grid), label[i][j], 1, 0, 2, 1);
            else
                gtk_grid_attach(GTK_GRID(grid), label[i][j], j, i, 1, 1);
        }
    }
    gtk_container_add(GTK_CONTAINER(tra_frame[1]), grid);
	gtk_container_add(GTK_CONTAINER(content_area), tra_frame[1]);
}

// Mod part
static void	on_mod_clicked(GtkButton *button, gpointer user_data) {
	(void)button;
	gpointer	selectedBq;
	gpointer	selectedval;
	sqlite3		*db;
	int			value;
	char		*mod_val;
	char		buff[500];


	selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
	selectedval = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(mod_combo));
	if (!selectedBq || !selectedval)
		return ;
	if (!strcmp(selectedval, "Taux reg"))
		mod_val = "TREG";
	else if (!strcmp(selectedval, "Taux pla"))
		mod_val = "TPLA";
	else
		mod_val = "LINE";
	value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(user_data));
	bzeros(buff, 500);
	if (!sqlite3_open(envlop.db_path, &db))
	{

		snprintf(buff, 500, "UPDATE %s_%s SET EDATE = '%d-%d-%d' WHERE EDATE > '%d-%d-%d';", mod_val, (char *)selectedBq, envlop.date.y, envlop.date.m, envlop.date.d, envlop.date.y, envlop.date.m, envlop.date.d);
		if (sqlite3_exec(db, buff, NULL, NULL, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
		snprintf(buff, 500, "INSERT INTO %s_%s VALUES('%d-%d-%d', '%d-%d-%d', %d);", mod_val, (char *)selectedBq, envlop.date.y, envlop.date.m, envlop.date.d, envlop.date.y+100, envlop.date.m, envlop.date.d, value);
		if (sqlite3_exec(db, buff, NULL, NULL, NULL) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			sqlite3_close(db);
			return ;
		}
		sqlite3_close(db);
	}
}
static void	mod_part(GtkWidget *content_area) {
	GtkWidget	*mod_button;
	GtkWidget	*grid;

	mod_frame = gtk_frame_new("Modifier Line/Taux");
	gtk_frame_set_label_align(GTK_FRAME(mod_frame), 0.5, 0.5);
	grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
	gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
	gtk_container_add(GTK_CONTAINER(mod_frame), grid);
	gtk_container_add(GTK_CONTAINER(content_area), mod_frame);
	// modifiable value combo
	mod_combo = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mod_combo), "Taux reg");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mod_combo), "Taux pla");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(mod_combo), "Line");
	gtk_grid_attach(GTK_GRID(grid), mod_combo, 0, 1, 1, 1);
	// new value
    GtkWidget *spin_button = gtk_spin_button_new(gtk_adjustment_new(0.0,
			-G_MAXDOUBLE, G_MAXDOUBLE, 0.01, 0.1, 0.0), 0.01, 2);
	gtk_grid_attach(GTK_GRID(grid), spin_button, 0, 2, 1, 1);
	// button
	mod_button = gtk_button_new_with_label("Modifier");
	g_signal_connect(mod_button, "clicked", G_CALLBACK(on_mod_clicked), spin_button);
	gtk_grid_attach(GTK_GRID(grid), mod_button, 0, 3, 1, 1);
	GtkWidget	*date_label = gtk_label_new("Veuillez choisir la date");
	GtkWidget	*date_widget = gtk_calendar_new();
	g_signal_connect(date_widget, "day-selected", G_CALLBACK(on_calendar_date_selected), NULL);
	gtk_grid_attach(GTK_GRID(grid), date_label, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), date_widget, 1, 1, 1, 3);
}

// Content area
static void	content_area_part(GtkWidget *box) {
	GtkWidget	*content_area;
	GtkWidget	*bq_box;

	content_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_hexpand(content_area, TRUE);
	gtk_widget_set_valign(content_area, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(box), content_area, TRUE, TRUE, 0);

	bq_frame = gtk_frame_new("Veuillez selection une banque SVP!");
	bq_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	bq_combo = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "BP");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "CDM");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "BMCI");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "BMCE");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "CIH");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "AWB");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "ARB");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "SGMB");
	g_signal_connect(bq_combo, "changed", G_CALLBACK(on_combo_changed), NULL);
	gtk_frame_set_label_align(GTK_FRAME(bq_frame), 0.5, 0.5);
	gtk_box_pack_start(GTK_BOX(bq_box), bq_combo, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(bq_frame), bq_box);
	gtk_container_add(GTK_CONTAINER(content_area), bq_frame);
	import_part(content_area);
	export_part(content_area);
	track_part(content_area);
	mod_part(content_area);
	bqspec_part(content_area);
}

// Main
int	main(int argc, char	**argv)
{
	GtkWidget	*window;
	GtkWidget	*box;

	envlop.bq = NULL;
	(bzeros(envlop.wd, 500), strncpy(envlop.wd, argv[0], strlen(argv[0]) - APPNAME));
	appdata = g_build_filename(g_get_user_data_dir(), "agio", NULL);
	(bzeros(envlop.db_path, 500), strncpy(envlop.db_path, appdata, strlen(appdata)), strcat(envlop.db_path, "/agio.db"));
	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Agio sur le découvert bancaire");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_maximize(GTK_WINDOW(window));
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(window), box);
	sidebar_part(box);
	gtk_box_pack_start(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_VERTICAL), FALSE, FALSE, 10);
	content_area_part(box);
	gtk_widget_show_all(window);
	// window
	gtk_widget_hide(bq_frame);
	gtk_widget_hide(imp_frame[0]);
	gtk_widget_hide(imp_frame[1]);
	gtk_widget_hide(imp_frame[2]);
	gtk_widget_hide(exp_frame);
	gtk_widget_hide(mod_frame);
	gtk_widget_hide(tra_frame[0]);
	gtk_widget_hide(tra_frame[1]);
	gtk_widget_hide(bq_spec_frame);
	gtk_main();
	return (g_free(file_path), 0);
}
