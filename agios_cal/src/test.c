#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define TR1 "\t\t\t\tJanvier\t\t\tFeverier\t\t\tMars\n"
#define TR2 "\t\t\tJanvier\t\t\tFeverier\t\t\tMars\n"
#define TR3 "\t\t\tJanvier\t\t\tFeverier\t\t\tMars\n"
#define TR4 "\t\t\tJanvier\t\t\tFeverier\t\t\tMars\n"
#define TD1 " WHERE DATE BETWEEN '%s-01' AND '%s-04'"
#define TD2 " WHERE DATE BETWEEN '%s-04' AND '%s-07'"
#define TD3 " WHERE DATE BETWEEN '%s-07' AND '%s-10'"
#define TD4 " WHERE DATE BETWEEN '%s-10' AND '%s-12-31'"

GtkWidget   *label[20][4];
static void	on_track_clicked(void);



int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Grid in Frame Example");
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget *frame = gtk_frame_new(NULL);
    gtk_frame_set_label(GTK_FRAME(frame), "Grid Frame");

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    int i;
    int j;
    i = -1;
    while (++i < 20) {
        j = -1;
        while (++j < 4) {
            label[i][j] = gtk_label_new(NULL);
            if (j == 0)
            gtk_label_set_xalign(GTK_LABEL(label[i][j]), 0.0);
        }
    }
    i = -1;
    while (++i < 20) {
        j = -1;
        while (++j < 4) {
            if (i == 0)
                gtk_grid_attach(GTK_GRID(grid), label[i][j], 1, 0, 2, 1);
            // else if (i == 2 || i == 9)            
            //     gtk_grid_attach(GTK_GRID(grid), label[i][j], j, i, 2, 1);
            else
                gtk_grid_attach(GTK_GRID(grid), label[i][j], j, i, 1, 1);
        }
    }
    gtk_container_add(GTK_CONTAINER(frame), grid);
    gtk_container_add(GTK_CONTAINER(window), frame);
    on_track_clicked();
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
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
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    int i;
    int j;
    i = -1;
    while (++i < 20) {
        j = -1;
        while (++j < 4) {
            label[i][j] = gtk_label_new(NULL);
            if (j == 0)
            gtk_label_set_xalign(GTK_LABEL(label[i][j]), 0.0);
        }
    }
    i = -1;
    while (++i < 20) {
        j = -1;
        while (++j < 4) {
            if (i == 0)
                gtk_grid_attach(GTK_GRID(grid), label[i][j], 1, 0, 2, 1);
            // else if (i == 2 || i == 9)            
            //     gtk_grid_attach(GTK_GRID(grid), label[i][j], j, i, 2, 1);
            else
                gtk_grid_attach(GTK_GRID(grid), label[i][j], j, i, 1, 1);
        }
    }
    gtk_container_add(GTK_CONTAINER(tra_frame[1]), grid);
	gtk_container_add(GTK_CONTAINER(content_area), tra_frame[1]);
}