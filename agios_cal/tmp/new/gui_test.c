#include <gtk/gtk.h>
#include <time.h>

GtkWidget *frame1;
GtkWidget *frame2;
GtkWidget *frame3;
GtkWidget *frame4;
GtkWidget *month_combo;
GtkWidget *trim_combo;
GtkWidget *bq_combo;
GtkWidget *bq_combo2;
GtkWidget *year_combo;
GtkWidget *year_combo2;

typedef struct  s_var
{
    char    *file;
    char    *bq;
    int     month;
    char    *year;
}   t_var;

int	import_file(t_var *vars);

int int_month(char *month) {
    if (!strcmp(month, "January"))
        return 1;
    else if (!strcmp(month, "February"))
        return 2;
    else if (!strcmp(month, "March"))
        return 3;
    else if (!strcmp(month, "April"))
        return 4;
    else if (!strcmp(month, "May"))
        return 5;
    else if (!strcmp(month, "June"))
        return 6;
    else if (!strcmp(month, "July"))
        return 7;
    else if (!strcmp(month, "August"))
        return 8;
    else if (!strcmp(month, "September"))
        return 9;
    else if (!strcmp(month, "October"))
        return 10;
    else if (!strcmp(month, "November"))
        return 11;
    else if (!strcmp(month, "December"))
        return 12;
    else
        return 0;
}

static gboolean on_drag_data_received(GtkWidget *widget, GdkDragContext *drag_context,
                                      gint x, gint y, GtkSelectionData *data, guint info,
                                      guint time, gpointer user_data)
{
    if (data && gtk_selection_data_get_length(data) > 0)
    {
        gchar *file_uri = gtk_selection_data_get_text(data);
        g_print("Dropped file: %s\n", file_uri);
        g_free(file_uri);
    }

    return TRUE;
}

// Function to handle drag and drop motion
static gboolean on_drag_motion(GtkWidget *widget, GdkDragContext *drag_context,
                               gint x, gint y, guint time, gpointer user_data)
{
    // Determine if the drag operation is supported
    GdkDragAction suggested_action = gdk_drag_context_get_suggested_action(drag_context);
    if (suggested_action & GDK_ACTION_COPY)
    {
        gdk_drag_status(drag_context, GDK_ACTION_COPY, time);
        return TRUE;
    }

    return FALSE;
}

static void button_clicked(GtkWidget *widget, gpointer data)
{
    const gchar *label = gtk_button_get_label(GTK_BUTTON(widget));

    if (strcmp(label, "Import") == 0) {
        gtk_widget_show(frame1);
        gtk_widget_hide(frame2);
        gtk_widget_hide(frame3);
        gtk_widget_hide(frame4);
    } else if (strcmp(label, "Export") == 0) {
        gtk_widget_hide(frame1);
        gtk_widget_show(frame2);
        gtk_widget_hide(frame3);
        gtk_widget_hide(frame4);
    } else if (strcmp(label, "Track") == 0) {
        gtk_widget_hide(frame1);
        gtk_widget_hide(frame2);
        gtk_widget_show(frame3);
        gtk_widget_hide(frame4);
    } else if (strcmp(label, "Edit") == 0) {
        gtk_widget_hide(frame1);
        gtk_widget_hide(frame2);
        gtk_widget_hide(frame3);
        gtk_widget_show(frame4);
    }
}
int init_export(t_var *vars, char *trim);

void on_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Save File", GTK_WINDOW(data), action, "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT, NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    gtk_file_chooser_set_current_name(chooser, "Untitled.xlsx");

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT) {
        t_var   vars;
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);
        vars.file = filename;
        // Export XLSX file using libxlsxwriter
        gpointer selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo2));
        if (selectedBq != NULL)
            vars.bq = (char *)selectedBq;
        else
            return;
        gpointer selectedYear = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(year_combo2));
        if (selectedYear != NULL)
            vars.year = (char *)selectedYear;
        else
            return;
        char *trim;
        gpointer selectedtrim = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(trim_combo));
        if (selectedtrim != NULL)
            trim = (char *)selectedtrim;

        init_export(&vars, trim);
        g_free(vars.file);
        g_free(vars.bq);
        g_free(trim);
        g_free(vars.year);
    }

    gtk_widget_destroy(dialog);
}

static void button1_1_clicked(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    // Create a file chooser dialog
    dialog = gtk_file_chooser_dialog_new("Open File", GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                         action, "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT, NULL);

    // Limit the file types to xls, xlsx, and csv
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.xls");
    gtk_file_filter_add_pattern(filter, "*.xlsx");
    gtk_file_filter_add_pattern(filter, "*.csv");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    // Run the dialog and handle the response
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        t_var   vars;
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        // TODO: Process the imported file
        vars.file = filename;
        // g_free(filename);


        // Get the selected month
        gpointer selectedMonth = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(month_combo));
        if (selectedMonth != NULL) {
            vars.month = int_month((char *)selectedMonth);
            // g_print("Selected month: %d\n", int_month((char *)selectedMonth));
            g_free(selectedMonth);
        }
        else
            return;
        
        // Get the selected value from bq_combo
        gpointer selectedBq = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(bq_combo));
        if (selectedBq != NULL) {
            vars.bq = (char *)selectedBq;
            // g_print("Selected bq: %s\n", selectedBq);
            // g_free(selectedBq);
        }
        else
            return;
        
        // Get the selected year
        gpointer selectedYear = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(year_combo));
        if (selectedYear != NULL) {
            vars.year = (char *)selectedYear;
            // g_print("Selected year: %s\n", selectedYear);
            // g_free(selectedYear);
        }
        else
            return;
        //g_print("Selected file: %s\n", filename);
        import_file(&vars);
        g_free(vars.file);
        g_free(vars.bq);
        g_free(vars.year);
    }

    // Close the dialog
    gtk_widget_destroy(dialog);
}


int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Sidebar Example");
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    gtk_window_set_default_size(GTK_WINDOW(window), 1200, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_name(sidebar, "sidebar");
    gtk_box_pack_start(GTK_BOX(box), sidebar, FALSE, FALSE, 0);
    gtk_widget_set_valign(sidebar, GTK_ALIGN_CENTER);

    GtkWidget *logo_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), logo_box, FALSE, FALSE, 10);
    gtk_widget_set_halign(logo_box, GTK_ALIGN_CENTER);

    GtkWidget *logo = gtk_image_new_from_file("logo.png");
    gtk_box_pack_start(GTK_BOX(logo_box), logo, FALSE, FALSE, 0);

    GtkWidget *separator1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(sidebar), separator1, FALSE, FALSE, 10);

    GtkWidget *buttons_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(sidebar), buttons_box, FALSE, FALSE, 10);
    gtk_widget_set_halign(buttons_box, GTK_ALIGN_CENTER);

    GtkWidget *button1 = gtk_button_new_with_label("Import");
    g_signal_connect(button1, "clicked", G_CALLBACK(button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(buttons_box), button1, FALSE, FALSE, 5);

    GtkWidget *button2 = gtk_button_new_with_label("Export");
    g_signal_connect(button2, "clicked", G_CALLBACK(button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(buttons_box), button2, FALSE, FALSE, 5);

    GtkWidget *button3 = gtk_button_new_with_label("Track");
    g_signal_connect(button3, "clicked", G_CALLBACK(button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(buttons_box), button3, FALSE, FALSE, 5);

    GtkWidget *button4 = gtk_button_new_with_label("Edit");
    g_signal_connect(button4, "clicked", G_CALLBACK(button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(buttons_box), button4, FALSE, FALSE, 5);

    GtkWidget *separator2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(sidebar), separator2, FALSE, FALSE, 10);

    GtkWidget *footer_label = gtk_label_new("© 2023 Soremed. All right reserved.");
    gtk_box_pack_end(GTK_BOX(sidebar), footer_label, FALSE, FALSE, 10);
    gtk_widget_set_halign(footer_label, GTK_ALIGN_CENTER);

    GtkWidget *separator3 = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    gtk_box_pack_start(GTK_BOX(box), separator3, FALSE, FALSE, 10);

    GtkWidget *content_area = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_hexpand(content_area, TRUE);
    gtk_widget_set_valign(content_area, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box), content_area, TRUE, TRUE, 0);

    frame1 = gtk_frame_new("Import Relevé");
    frame2 = gtk_frame_new("Export Agio Ticket");
    frame3 = gtk_frame_new("Frame3");
    frame4 = gtk_frame_new("Frame4");
    GtkWidget *frame6 = gtk_frame_new("Veuillez selection une Banque SVP!");


    GtkWidget *frame_box1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *frame_box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *frame_box3 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *frame_box4 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *frame_box6 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);



    //Export
    bq_combo2 = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo2), "BP");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo2), "BMCI");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo2), "BMCE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo2), "CIH");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo2), "SGMB");

    trim_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(trim_combo), "1er trimestre");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(trim_combo), "2éme trimestre");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(trim_combo), "3éme trimestre");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(trim_combo), "4éme trimestre");

    year_combo2 = gtk_combo_box_text_new();
    int currentYear = 2023; // Replace with your logic to get the current year
    for (int year = currentYear - 10; year <= currentYear + 10; ++year) {
        char yearStr[5];
        snprintf(yearStr, sizeof(yearStr), "%d", year);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(year_combo2), yearStr);
    }
    // Set the current year as the default selection
    int currentYearIndex = currentYear - (currentYear - 10);
    gtk_combo_box_set_active(GTK_COMBO_BOX(year_combo2), currentYearIndex);

    GtkWidget *combo_box2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(combo_box2), TRUE);
    // gtk_box_pack_start(GTK_BOX(combo_box2), bq_combo2_1, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(frame_box6), bq_combo2, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(combo_box2), year_combo2, TRUE, TRUE, 5);

    GtkWidget *buttonEx = gtk_button_new_with_label("Export");
    g_signal_connect(buttonEx, "clicked", G_CALLBACK(on_button_clicked), NULL);

    GtkWidget *combo_box_ex = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    // gtk_box_set_homogeneous(GTK_BOX(combo_box_ex), TRUE);
    gtk_box_pack_start(GTK_BOX(combo_box_ex), trim_combo, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(combo_box_ex), combo_box2, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(combo_box_ex), buttonEx, TRUE, TRUE, 5);

    gtk_box_pack_start(GTK_BOX(frame_box2), combo_box_ex, TRUE, TRUE, 5);
    // gtk_box_pack_start(GTK_BOX(frame_box2), bq_combo2_1, TRUE, TRUE, 5);
    // gtk_box_pack_start(GTK_BOX(frame_box2), combo_box2, TRUE, TRUE, 5);
    // gtk_box_pack_start(GTK_BOX(frame_box2), buttonEx, TRUE, TRUE, 5);
    

    // gtk_box_pack_start(GTK_BOX(frame_box2), button2, FALSE, FALSE, 5);


    // Create a dropdown box for month selection
    month_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "January");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "February");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "March");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "April");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "May");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "June");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "July");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "August");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "September");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "October");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "November");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(month_combo), "December");
    //gtk_box_pack_start(GTK_BOX(frame_box1), month_combo, FALSE, FALSE, 5);



    // Get the current month and set it as the default selection
    GDateTime *currentTime = g_date_time_new_now_local();
    int currentMonth = g_date_time_get_month(currentTime);
    gtk_combo_box_set_active(GTK_COMBO_BOX(month_combo), currentMonth - 1); // Subtract 1 to account for zero-based index

    g_date_time_unref(currentTime);
    //gtk_box_pack_start(GTK_BOX(frame_box1), month_combo, FALSE, FALSE, 5);

    bq_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "BP");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "BMCI");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "BMCE");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "CIH");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(bq_combo), "SGMB");
    //gtk_box_pack_start(GTK_BOX(frame_box1), bq_combo, FALSE, FALSE, 5);


    year_combo = gtk_combo_box_text_new();
    currentYear = 2023; // Replace with your logic to get the current year
    for (int year = currentYear - 10; year <= currentYear + 10; ++year) {
        char yearStr[5];
        snprintf(yearStr, sizeof(yearStr), "%d", year);
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(year_combo), yearStr);
    }

    // Set the current year as the default selection
    currentYearIndex = currentYear - (currentYear - 10);
    gtk_combo_box_set_active(GTK_COMBO_BOX(year_combo), currentYearIndex);

    //gtk_box_pack_start(GTK_BOX(frame_box1), year_combo, FALSE, FALSE, 5);
    GtkWidget *combo_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(combo_box), TRUE);
    gtk_box_pack_start(GTK_BOX(combo_box), bq_combo, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(combo_box), month_combo, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(combo_box), year_combo, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(frame_box1), combo_box, TRUE, TRUE, 5);


// Create the label widget
GtkWidget *label = gtk_label_new("Drag and Drop File here!");

// Create the event box widget
GtkWidget *event_box = gtk_event_box_new();
gtk_container_add(GTK_CONTAINER(event_box), label);

// Set the event box properties
gtk_widget_set_size_request(event_box, 200, 200); // Set the desired width and height

// Set the event box as a drop destination
gtk_widget_set_events(event_box, GDK_ALL_EVENTS_MASK);
gtk_drag_dest_set(event_box, GTK_DEST_DEFAULT_ALL, NULL, 0, GDK_ACTION_COPY);

// Connect the drag and drop signals
g_signal_connect(event_box, "drag_data_received", G_CALLBACK(on_drag_data_received), NULL);
g_signal_connect(event_box, "drag_motion", G_CALLBACK(on_drag_motion), NULL);

// Add the event box to the desired container
gtk_box_pack_start(GTK_BOX(frame_box1), event_box, FALSE, FALSE, 5);
    
    GtkWidget *button1_1 = gtk_button_new_with_label("Import");
    g_signal_connect(button1_1, "clicked", G_CALLBACK(button1_1_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(frame_box1), button1_1, FALSE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(frame1), frame_box1);
    gtk_container_add(GTK_CONTAINER(frame2), frame_box2);
    gtk_container_add(GTK_CONTAINER(frame3), frame_box3);
    gtk_container_add(GTK_CONTAINER(frame4), frame_box4);
    gtk_container_add(GTK_CONTAINER(frame6), frame_box6);

    gtk_container_add(GTK_CONTAINER(content_area), frame1);
    gtk_container_add(GTK_CONTAINER(content_area), frame2);
    gtk_container_add(GTK_CONTAINER(content_area), frame3);
    gtk_container_add(GTK_CONTAINER(content_area), frame4);
    gtk_container_add(GTK_CONTAINER(content_area), frame6);

    gtk_frame_set_label_align(GTK_FRAME(frame1), 0.5, 0.5);
    gtk_frame_set_label_align(GTK_FRAME(frame2), 0.5, 0.5);
    gtk_frame_set_label_align(GTK_FRAME(frame3), 0.5, 0.5);
    gtk_frame_set_label_align(GTK_FRAME(frame4), 0.5, 0.5);
    gtk_frame_set_label_align(GTK_FRAME(frame6), 0.5, 0.5);

    gtk_widget_show_all(window);
    gtk_widget_hide(frame1);
    gtk_widget_hide(frame2);
    gtk_widget_hide(frame3);
    gtk_widget_hide(frame4);

    gtk_main();

    return 0;
}
