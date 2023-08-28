// #include <gtk/gtk.h>

// void on_combo_changed(GtkComboBox *combo_box, gpointer user_data)
// {
//     GtkLabel *label = GTK_LABEL(user_data);
//     const gchar *active_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));
//     gtk_label_set_text(label, active_text);
// }

// int main(int argc, char *argv[])
// {
//     gtk_init(&argc, &argv);

//     // Create the main GTK window
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Combo Box Example");
//     gtk_container_set_border_width(GTK_CONTAINER(window), 10);
//     g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     // Create a grid container
//     GtkWidget *grid = gtk_grid_new();
//     gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE); // Set columns to have equal width
//     gtk_grid_set_column_spacing(GTK_GRID(grid), 10); // Set spacing between columns
//     gtk_container_add(GTK_CONTAINER(window), grid);

//     // Create the first frame
//     GtkWidget *frame1 = gtk_frame_new(NULL);
//     gtk_frame_set_shadow_type(GTK_FRAME(frame1), GTK_SHADOW_ETCHED_IN);
//     gtk_frame_set_label_align(GTK_FRAME(frame1), 0.5, 0.5);
//     gtk_grid_attach(GTK_GRID(grid), frame1, 0, 0, 1, 1);

//     // Create the second frame
//     GtkWidget *frame2 = gtk_frame_new(NULL);
//     gtk_frame_set_shadow_type(GTK_FRAME(frame2), GTK_SHADOW_ETCHED_IN);
//     gtk_frame_set_label_align(GTK_FRAME(frame2), 0.5, 0.5);
//     gtk_grid_attach(GTK_GRID(grid), frame2, 1, 0, 1, 1);

//     // Create a combo box for Frame 1
//     GtkWidget *combo_box = gtk_combo_box_text_new();
//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Option 1");
//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Option 2");
//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo_box), "Option 3");
//     gtk_container_add(GTK_CONTAINER(frame1), combo_box);

//     // Create a label for Frame 2
//     GtkWidget *label = gtk_label_new(NULL);
//     gtk_container_add(GTK_CONTAINER(frame2), label);

//     g_signal_connect(combo_box, "changed", G_CALLBACK(on_combo_changed), label);

//     gtk_widget_show_all(window);

//     gtk_main();
//     return 0;
// }

// #include <gtk/gtk.h>
// #include <glib/gprintf.h>

// void on_button_clicked(GtkButton *button, gpointer user_data)
// {
//     GtkSpinButton *spin_button = GTK_SPIN_BUTTON(user_data);
//     gdouble value = gtk_spin_button_get_value(spin_button);

//     g_printf("Cloture Balance: %.2f\n", value);
// }

// int main(int argc, char *argv[])
// {
//     gtk_init(&argc, &argv);

//     // Create the main GTK window
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Cloture Balance Example");
//     gtk_container_set_border_width(GTK_CONTAINER(window), 10);
//     g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     // Create a VBox container
//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_container_add(GTK_CONTAINER(window), vbox);

//     // Create a label for input description
//     GtkWidget *label = gtk_label_new("Please enter your balance:");
//     gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

//     // Create an adjustment for the spin button
//     GtkAdjustment *adjustment = gtk_adjustment_new(0.0, -G_MAXDOUBLE, G_MAXDOUBLE, 0.01, 0.1, 0.0);

//     // Create a spin button for decimal input
//     GtkWidget *spin_button = gtk_spin_button_new(adjustment, 0.01, 2);
//     gtk_box_pack_start(GTK_BOX(vbox), spin_button, FALSE, FALSE, 0);

//     // Create a button
//     GtkWidget *button = gtk_button_new_with_label("Cloturer");
//     gtk_box_pack_end(GTK_BOX(vbox), button, FALSE, FALSE, 0);
//     g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), spin_button);

//     gtk_widget_show_all(window);

//     gtk_main();
//     return 0;
// }

// #include <gtk/gtk.h>

// void on_file_selected(GtkFileChooserButton *chooser_button, gpointer user_data)
// {
//     gchar *file_path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser_button));
//     g_print("Dropped file: %s\n", file_path);
//     g_free(file_path);
// }

// int main(int argc, char *argv[])
// {
//     gtk_init(&argc, &argv);

//     // Create the main GTK window
//     GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     gtk_window_set_title(GTK_WINDOW(window), "Drag and Drop Example");
//     gtk_container_set_border_width(GTK_CONTAINER(window), 10);
//     g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     // Create a VBox container
//     GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
//     gtk_container_add(GTK_CONTAINER(window), vbox);

//     // Create a drag and drop area
//     GtkWidget *drag_area = gtk_file_chooser_button_new("Drag and drop XLSX or CSV file here", GTK_FILE_CHOOSER_ACTION_OPEN);
//     GtkFileFilter *file_filter = gtk_file_filter_new();
//     gtk_file_filter_set_name(file_filter, "XLSX and CSV Files");
//     gtk_file_filter_add_mime_type(file_filter, "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
//     gtk_file_filter_add_mime_type(file_filter, "text/csv");
//     gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(drag_area), file_filter);
//     g_signal_connect(drag_area, "file-set", G_CALLBACK(on_file_selected), NULL);
//     gtk_box_pack_start(GTK_BOX(vbox), drag_area, TRUE, TRUE, 0);

//     gtk_widget_show_all(window);

//     gtk_main();
//     return 0;
// }

#include <stdio.h>
#include <jansson.h>

int main() {
    // Read JSON data from file
    FILE *file = fopen("data.json", "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file for reading.\n");
        return 1;
    }

    json_t *root = json_loadf(file, 0, NULL);
    fclose(file);

    if (!root) {
        fprintf(stderr, "Failed to parse JSON data.\n");
        return 1;
    }

    // Modify the JSON data (Example: Update month and year for 'BP')
    json_t *bp_obj = json_object_get(root, "BP");
    if (!json_is_object(bp_obj)) {
        fprintf(stderr, "Failed to find 'BP' section in the JSON data.\n");
        json_decref(root);
        return 1;
    }

    json_object_set_new(bp_obj, "Month", json_integer(6)); // Update the month value
    json_object_set_new(bp_obj, "year", json_integer(2022)); // Update the year value

    // Write the modified JSON data to file
    FILE *output_file = fopen("output.json", "w");
    if (output_file == NULL) {
        fprintf(stderr, "Failed to open file for writing.\n");
        json_decref(root);
        return 1;
    }

    json_dumpf(root, output_file, JSON_INDENT(2));
    fclose(output_file);

    json_decref(root);

    return 0;
}

int json_writer(char *bq, char *wd, int m, int y) {
    char    *file;
    json_t  *root;
    json_error_t error;

    if (!bq || !wd)
        return 1;
    file = strcat(wd, "/data/module.json");
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
    json_object_set_new(bp_obj, "Month", json_integer(m));
    json_object_set_new(bp_obj, "year", json_integer(y));
    if (json_dump_file(root, file, JSON_INDENT(2))) {
        fprintf(stderr, "Failed to write JSON to file.\n");
        return (json_decref(root), 1);
    }
    return (free(file), json_decref(root), 0);
}

