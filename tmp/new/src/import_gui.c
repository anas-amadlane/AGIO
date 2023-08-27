#include <gtk/gtk.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

typedef struct  s_var
{
    char    *file;
    char    *bq;
    int     month;
    int		year;
}   t_var;

typedef struct  s_col
{
	int             index;
	char            *colname;
	struct s_col   *next;
}   t_col;

typedef struct	s_var_date
{
	double	v;
	char	*d_range[2];
}				t_var_date;

typedef struct  s_rel
{
	char		**head;
	double		solde;
	int			month;
	int			year;
	t_var_date	line;
	t_var_date	tx_reg;
	t_var_date	tx_pla;
	t_col		*col;
	int			skip[2];
}   t_rel;

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
t_rel	*jsonparser(char *file, char *bq);
int	import_file(t_var *vars);
int init_export(t_var *vars, char *trim);
int json_writer(char *bq, char *wd, int m, int y);

gchar		*file_path = NULL;
t_rel		*releve = NULL;
GtkWidget	*imp_frame[3];
GtkWidget	*tra_frame[2];
GtkWidget	*exp_frame;
GtkWidget	*bq_frame;
GtkWidget	*bq_combo;
GtkWidget	*month_label;
GtkWidget	*ticket_label;
GtkWidget	*exp_combo[2];

// Sidebar GUI
static void	button_clicked(GtkWidget *widget, gpointer data)
{
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
	}
	else if (!strcmp(label, "Exporter")) {
		gtk_widget_show(bq_frame);
		gtk_widget_hide(imp_frame[0]);
		gtk_widget_hide(imp_frame[1]);
		gtk_widget_hide(imp_frame[2]);
		gtk_widget_show(exp_frame);
		gtk_widget_hide(tra_frame[0]);
		gtk_widget_hide(tra_frame[1]);
	}
	else if (!strcmp(label, "Tracker")) {
		gtk_widget_show(bq_frame);
		gtk_widget_hide(imp_frame[0]);
		gtk_widget_hide(imp_frame[1]);
		gtk_widget_hide(imp_frame[2]);
		gtk_widget_hide(exp_frame);
		gtk_widget_show(tra_frame[0]);
		gtk_widget_show(tra_frame[1]);
	}
	else if (!strcmp(label, "Modifier")) {
		gtk_widget_hide(bq_frame);
		gtk_widget_hide(imp_frame[0]);
		gtk_widget_hide(imp_frame[1]);
		gtk_widget_hide(imp_frame[2]);
		gtk_widget_hide(exp_frame);
		gtk_widget_hide(tra_frame[0]);
		gtk_widget_hide(tra_frame[1]);
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

	sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_widget_set_name(sidebar, "sidebar");
	gtk_box_pack_start(GTK_BOX(box), sidebar, FALSE, FALSE, 0);
	gtk_widget_set_valign(sidebar, GTK_ALIGN_CENTER);
	// Sidebar header
	logo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_pack_start(GTK_BOX(sidebar), logo_box, FALSE, FALSE, 10);
	gtk_widget_set_halign(logo_box, GTK_ALIGN_CENTER);
	gtk_box_pack_start(GTK_BOX(logo_box), gtk_image_new_from_file("logo.png"), FALSE, FALSE, 0);
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

// Import part
static void	on_combo_changed(GtkComboBox *combo_box, gpointer user_data) {
	gpointer	selectedBq;
	char buff[100];

	selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
	if (!selectedBq)
		return;
	releve = jsonparser("/home/anas/clones/agios_cal/data/module.json", (char *)selectedBq);
	if (!releve)
		return ;
	snprintf(buff, 100, "<span font_size=\"15000\">%d/%d</span>", releve->month, releve->year);
	gtk_label_set_markup(GTK_LABEL(month_label), buff);
    //gtk_label_set_text(GTK_LABEL(month_label), "Janvier/2023");
	g_free(selectedBq);
}

static void	on_cloture_clicked(GtkButton *button, gpointer user_data) {
    sqlite3         *db;
	sqlite3_stmt    *stmt;
	int				value;
	gpointer		selectedBq;
	char			buff[100];
	char			date[20];
	const unsigned char	*tableName;

	db = NULL;
	selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
	if (!selectedBq)
		return;
	if (releve->month < 9)
		snprintf(date, 20, "%d-0%d-01\0", releve->year, releve->month + 1);
	else if (releve->month == 12)
		snprintf(date, 20, "%d-01-01\0", releve->year + 1);
	else
		snprintf(date, 20, "%d-%d-01\0", releve->year, releve->month + 1);
	value = gtk_spin_button_get_value(GTK_SPIN_BUTTON(user_data));
	if (!sqlite3_open("../agio.db", &db))
	{
        snprintf(buff, 200, "select balance from CONC_OP_%s where S_DATE = date('%s', '-1 day');", selectedBq, date);
        if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK)
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
        else if (sqlite3_step(stmt) == SQLITE_ROW) {
			tableName = sqlite3_column_text(stmt, 0);
            if (atoi(tableName) == value)
				json_writer((char *)selectedBq, "/home/anas/clones/agios_cal", releve->month, releve->year);
        }
		sqlite3_close(db);
    }
	releve = jsonparser("/home/anas/clones/agios_cal/data/module.json", (char *)selectedBq);
	if (!releve)
		return ;
	snprintf(buff, 100, "<span font_size=\"15000\">%d/%d</span>", releve->month, releve->year);
	gtk_label_set_markup(GTK_LABEL(month_label), buff);
	g_free(selectedBq);
}

static void	on_file_selected(GtkFileChooserButton *chooser_button, gpointer user_data) {
	if (file_path)
    	g_free(file_path);
	file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser_button));
    //g_print("Dropped file: %s\n", file_path);
}

static void	on_import_clicked(GtkWidget *widget, gpointer user_data) {
	t_var		vars;
	gpointer	selectedBq;

	selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
	if (!file_path || !selectedBq)
		return ;
	vars.file = file_path;
	vars.bq = (char *)selectedBq;
	vars.month = releve->month;
	vars.year = releve->year;
	import_file(&vars);
	printf("File imported\n");
	g_free(vars.bq);
}

static void	import_part(GtkWidget *content_area) {
	GtkWidget	*grid;
	GtkWidget	*cloture_label;
	GtkWidget	*frame_box;
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
    gtk_file_filter_set_name(file_filter, "XLSX and CSV Files");
    gtk_file_filter_add_mime_type(file_filter, "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
    gtk_file_filter_add_mime_type(file_filter, "text/csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(drag_area), file_filter);
    g_signal_connect(drag_area, "file-set", G_CALLBACK(on_file_selected), NULL);
    gtk_box_pack_start(GTK_BOX(import_box), drag_area, TRUE, TRUE, 5);
	import_button = gtk_button_new_with_label("Importer");
	g_signal_connect(import_button, "clicked", G_CALLBACK(on_import_clicked), NULL);
	gtk_box_pack_start(GTK_BOX(import_box), import_button, FALSE, FALSE, 5);
	gtk_container_add(GTK_CONTAINER(imp_frame[2]), import_box);

	//gtk_container_add(GTK_CONTAINER(content_area), imp_frame[1]);
	//gtk_container_add(GTK_CONTAINER(content_area), imp_frame[2]);
}

// Export part
static void	on_export_clicked(GtkWidget *widget, gpointer data) {
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
		init_export(&vars, trim);
		g_free(vars.file);
		g_free(vars.bq);
		g_free(trim);
		//g_free(vars.year);
	}
	gtk_widget_destroy(dialog);
}

static void	export_part(GtkWidget *content_area) {
	GtkWidget	*exp_box;
	GtkWidget	*exp_button;

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
	char	yearStr[5];
	int		currentYear;
	int		currentYearIndex;
	currentYear = 2023; //relve year
	exp_combo[1] = gtk_combo_box_text_new();
	for (int year = currentYear - 10; year <= currentYear + 10; ++year) {
		snprintf(yearStr, 5, "%d", year);
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

// define query "select sum(NB_DEB_REG * 0.045/360) from NB_DEB_SGMB where date between '2023-01' and '2023-03' group by strftime('%m', date);"
// define query "select sum(NB_DEB_PLA * 0.075/360) from NB_DEB_SGMB where date between '2023-01' and '2023-03' group by strftime('%m', date);"
#define TR1 "\t\t\t\tJanvier\t\t\tFeverier\t\t\tMars\n"
#define TR2 "\t\t\tJanvier\t\t\tFeverier\t\t\tMars\n"
#define TR3 "\t\t\tJanvier\t\t\tFeverier\t\t\tMars\n"
#define TR4 "\t\t\tJanvier\t\t\tFeverier\t\t\tMars\n"
#define TD1 " WHERE DATE BETWEEN '%d-01' AND '%d-04'"
#define TD2 " WHERE DATE BETWEEN '%d-04' AND '%d-07'"
#define TD3 " WHERE DATE BETWEEN '%d-07' AND '%d-10'"
#define TD4 " WHERE DATE BETWEEN '%d-10' AND '%d-12-31'"

static void	on_track_clicked(GtkButton *button, gpointer user_data) {
    sqlite3         *db;
	sqlite3_stmt    *stmt;
	gpointer		selectedBq;
	gpointer		selectedtrim;
        char	buff[300];
		char	text[2000];
		char	*dt_r;
		char	dt_d[100];
		int		value;

	db = NULL;
	selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
	if (!selectedBq)
		return;
	if (!sqlite3_open("../agio.db", &db))
	{
		value = 20;
		selectedtrim = "1er trimestre";
	    if (!strcmp(selectedtrim, "1er trimestre"))
			(dt_r = TR1, snprintf(dt_d, 100, TD1, releve->year, releve->year));
		else if (!strcmp(selectedtrim, "2éme trimestre"))
			(dt_r = TR2, snprintf(dt_d, 100, TD2, releve->year, releve->year));
		else if (!strcmp(selectedtrim, "3éme trimestre"))
			(dt_r = TR3, snprintf(dt_d, 100, TD3, releve->year, releve->year));
		else if (!strcmp(selectedtrim, "4éme trimestre"))
			(dt_r = TR4, snprintf(dt_d, 100, TD4, releve->year, releve->year));
		snprintf(text, 2000, "\tCALCUL TICKET AGIOS %s\t%s\n\nNbres Débiteurs\n%sAutorisé\t\t", selectedBq, selectedtrim, dt_r);
		snprintf(buff, 300, "select round(sum(NB_DEB_REG), 1) from NB_DEB_%s %s group by strftime('%%m', date);", selectedBq, dt_d, releve->year, releve->year);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			return ;
		}
        while (sqlite3_step(stmt) == SQLITE_ROW) {
			strcat(text, sqlite3_column_text(stmt, 0));
			strcat(text, "\t\t\t");
		}
		strcat(text, "\t\nDépassement\t");
		snprintf(buff, 300, "select round(sum(NB_DEB_PLA), 1) from NB_DEB_%s %s group by strftime('%%m', date);", selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			return ;
		}
        while (sqlite3_step(stmt) == SQLITE_ROW) {
			strcat(text, sqlite3_column_text(stmt, 0));
			strcat(text, "\t\t");
        }
		strcat(text, "\t\nTt.Nb Déb\t");
		snprintf(buff, 300, "select round(sum(NB_DEB_REG) + sum(NB_DEB_PLA), 1) from NB_DEB_%s %s group by strftime('%%m', date);", selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			return ;
		}
        while (sqlite3_step(stmt) == SQLITE_ROW) {
			strcat(text, sqlite3_column_text(stmt, 0));
			strcat(text, "\t");
        }
		strcat(text, "\t\n\nInt sur Nbres Débiteurs\n");
		strcat(text, dt_r);
		strcat(text, "Autorisé\t");
		snprintf(buff, 300, "select round(sum(NB_DEB_REG * (SELECT TAUX FROM TREG_%s)/100/360), 1) from NB_DEB_%s %s group by strftime('%%m', date);", selectedBq, selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			return ;
		}
        while (sqlite3_step(stmt) == SQLITE_ROW) {
			strcat(text, sqlite3_column_text(stmt, 0));
			strcat(text, "\t\t");
        }
		strcat(text, "\t\nDépassement\t");
		snprintf(buff, 300, "select round(sum(NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s)/100/360), 1) from NB_DEB_%s %s group by strftime('%%m', date);", selectedBq, selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			return ;
		}
        while (sqlite3_step(stmt) == SQLITE_ROW) {
			strcat(text, sqlite3_column_text(stmt, 0));
			strcat(text, "\t\t");
        }
		strcat(text, "\t\nTt.Nb Déb\t");
		snprintf(buff, 300, "select round(sum(NB_DEB_REG * (SELECT TAUX FROM TREG_%s)/100/360) + sum(NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s)/100/360), 1) from NB_DEB_%s %s group by strftime('%%m', date);", selectedBq, selectedBq, selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			return ;
		}
        while (sqlite3_step(stmt) == SQLITE_ROW) {
			strcat(text, sqlite3_column_text(stmt, 0));
			strcat(text, "\t\t");
        }
		strcat(text, "\t\n\nIntérêts calculés\t\t\t\t");
		snprintf(buff, 300, "select round(sum(NB_DEB_REG * (SELECT TAUX FROM TREG_%s)/100/360) + sum(NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s)/100/360), 1) from NB_DEB_%s %s;", selectedBq, selectedBq, selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			return ;
		}
        else if (sqlite3_step(stmt) == SQLITE_ROW)
			strcat(text, sqlite3_column_text(stmt, 0));
		strcat(text, "\nIntérêts Débités\t\t\t\t");
		snprintf(buff, 300, "%d", value);
		strcat(text, buff);
		strcat(text, "\nDifférence\t\t\t\t\t");
		strcat(text, buff);
		strcat(text, "\nTaux Moyenne Appliqué\t\t\t\t");
		snprintf(buff, 300, "SELECT ROUND(((SUM(NB_DEB_REG * (SELECT TAUX FROM TREG_%s)/100/360) + SUM(NB_DEB_PLA * (SELECT TAUX FROM TPLA_%s)/100/360))/(SUM(NB_DEB_REG) + SUM(NB_DEB_PLA)))*360*100, 2) FROM NB_DEB_%s %s;", selectedBq, selectedBq, selectedBq, dt_d);
		if (sqlite3_prepare_v2(db, buff, -1, &stmt, 0) != SQLITE_OK) {
			fprintf(stderr, "Failed to execute statement1: %s\n", sqlite3_errmsg(db));
			return ;
		}
        while (sqlite3_step(stmt) == SQLITE_ROW) {
			strcat(text, sqlite3_column_text(stmt, 0));
			strcat(text, "%%\n");
        }
		gtk_label_set_markup(GTK_LABEL(ticket_label), text);
		sqlite3_close(db);
    }
	g_free(selectedBq);
}

static void	track_part(GtkWidget *content_area) {
	GtkWidget	*tra_box;
	GtkWidget	*tra_button;

	tra_frame[0] = gtk_frame_new("Tracker");
	gtk_frame_set_label_align(GTK_FRAME(tra_frame[0]), 0.5, 0.5);
	tra_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(tra_box), TRUE);
	tra_button = gtk_button_new_with_label("Tracker");
	g_signal_connect(tra_button, "clicked", G_CALLBACK(on_track_clicked), NULL);
	gtk_box_pack_start(GTK_BOX(tra_box), tra_button, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(tra_frame[0]), tra_box);
	gtk_container_add(GTK_CONTAINER(content_area), tra_frame[0]);

	tra_frame[1] = gtk_frame_new("TICKET AGIOS");
	gtk_frame_set_shadow_type(GTK_FRAME(tra_frame[1]), GTK_SHADOW_IN);
    gtk_container_set_border_width(GTK_CONTAINER(tra_frame[1]), 20);
    ticket_label = gtk_label_new(NULL);
    gtk_container_add(GTK_CONTAINER(tra_frame[1]), ticket_label);
	gtk_container_add(GTK_CONTAINER(content_area), tra_frame[1]);
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
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "BMCI");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "BMCE");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "CIH");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "SGMB");
	g_signal_connect(bq_combo, "changed", G_CALLBACK(on_combo_changed), NULL);
	gtk_frame_set_label_align(GTK_FRAME(bq_frame), 0.5, 0.5);
	gtk_box_pack_start(GTK_BOX(bq_box), bq_combo, TRUE, TRUE, 5);
	gtk_container_add(GTK_CONTAINER(bq_frame), bq_box);
	gtk_container_add(GTK_CONTAINER(content_area), bq_frame);
	import_part(content_area);
	export_part(content_area);
	track_part(content_area);
}

// Main
int	main(int argc, char	**argv)
{
	GtkWidget	*window;
	GtkWidget	*box;

	gtk_init(&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Agio sur le découvert bancaire");
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);
	gtk_window_set_default_size(GTK_WINDOW(window), 1200, 600);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_container_add(GTK_CONTAINER(window), box);
	sidebar_part(box);
	gtk_box_pack_start(GTK_BOX(box), gtk_separator_new(GTK_ORIENTATION_VERTICAL), FALSE, FALSE, 10);
	content_area_part(box);
	gtk_widget_show_all(window);
	gtk_widget_hide(bq_frame);
	gtk_widget_hide(imp_frame[0]);
	gtk_widget_hide(imp_frame[1]);
	gtk_widget_hide(imp_frame[2]);
	gtk_widget_hide(exp_frame);
	gtk_widget_hide(tra_frame[0]);
	gtk_widget_hide(tra_frame[1]);
	gtk_main();
	return (g_free(file_path), 0);
}
