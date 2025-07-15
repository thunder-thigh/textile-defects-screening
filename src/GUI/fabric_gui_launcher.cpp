// fabric_gui_launcher.cpp
// GTK-based GUI to start/stop the three binaries: image_capture, feed_splicer, and creasing_detector

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string>
#include <iostream>

GtkWidget *entry_roll_id, *entry_fabric_speed, *entry_visible_length, *entry_camera_id, *entry_video_url;
GSubprocess *image_proc = nullptr, *splicer_proc = nullptr, *detector_proc = nullptr;

std::string get_entry_text(GtkWidget *entry) {
    return gtk_entry_get_text(GTK_ENTRY(entry));
}

void start_image_capture(GtkWidget *, gpointer) {
    std::string cmd = "./image_capture " + get_entry_text(entry_roll_id) + " " +
                      get_entry_text(entry_fabric_speed) + " " +
                      get_entry_text(entry_visible_length) + " " +
                      get_entry_text(entry_camera_id);
    if (image_proc) g_subprocess_force_exit(image_proc);
    image_proc = g_subprocess_new(G_SUBPROCESS_FLAGS_NONE, NULL, "/bin/sh", "-c", cmd.c_str(), NULL);
}

void stop_image_capture(GtkWidget *, gpointer) {
    if (image_proc) g_subprocess_force_exit(image_proc);
    image_proc = nullptr;
}

void start_feed_splicer(GtkWidget *, gpointer) {
    std::string cmd = "./feed_splicer " + get_entry_text(entry_roll_id) + " " +
                      get_entry_text(entry_video_url) + " " +
                      get_entry_text(entry_fabric_speed) + " " +
                      get_entry_text(entry_visible_length);
    if (splicer_proc) g_subprocess_force_exit(splicer_proc);
    splicer_proc = g_subprocess_new(G_SUBPROCESS_FLAGS_NONE, NULL, "/bin/sh", "-c", cmd.c_str(), NULL);
}

void stop_feed_splicer(GtkWidget *, gpointer) {
    if (splicer_proc) g_subprocess_force_exit(splicer_proc);
    splicer_proc = nullptr;
}

void start_creasing_detector(GtkWidget *, gpointer) {
    if (detector_proc) g_subprocess_force_exit(detector_proc);
    detector_proc = g_subprocess_new(G_SUBPROCESS_FLAGS_NONE, NULL, "./creasing_detector", NULL);
}

void stop_creasing_detector(GtkWidget *, gpointer) {
    if (detector_proc) g_subprocess_force_exit(detector_proc);
    detector_proc = nullptr;
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Textile Defect Detection Launcher");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_widget_set_size_request(window, 500, 400);

    GtkWidget *outer_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), outer_box);
    gtk_widget_set_halign(outer_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(outer_box, GTK_ALIGN_CENTER);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_box_pack_start(GTK_BOX(outer_box), grid, TRUE, TRUE, 0);

    // Input fields
    entry_roll_id = gtk_entry_new();
    entry_fabric_speed = gtk_entry_new();
    entry_visible_length = gtk_entry_new();
    entry_camera_id = gtk_entry_new();
    entry_video_url = gtk_entry_new();

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Roll ID:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_roll_id, 1, 0, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Fabric Speed (mm/s):"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_fabric_speed, 1, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Visible Length (mm):"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_visible_length, 1, 2, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Camera ID:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_camera_id, 1, 3, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Video URL (for splicer):"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_video_url, 1, 4, 1, 1);

    // Buttons
    GtkWidget *btn_img_start = gtk_button_new_with_label("Start image_capture");
    GtkWidget *btn_img_stop = gtk_button_new_with_label("Stop image_capture");
    gtk_grid_attach(GTK_GRID(grid), btn_img_start, 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_img_stop, 1, 5, 1, 1);

    GtkWidget *btn_splice_start = gtk_button_new_with_label("Start feed_splicer");
    GtkWidget *btn_splice_stop = gtk_button_new_with_label("Stop feed_splicer");
    gtk_grid_attach(GTK_GRID(grid), btn_splice_start, 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_splice_stop, 1, 6, 1, 1);

    GtkWidget *btn_detect_start = gtk_button_new_with_label("Start creasing_detector");
    GtkWidget *btn_detect_stop = gtk_button_new_with_label("Stop creasing_detector");
    gtk_grid_attach(GTK_GRID(grid), btn_detect_start, 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_detect_stop, 1, 7, 1, 1);

    // Signals
    g_signal_connect(btn_img_start, "clicked", G_CALLBACK(start_image_capture), NULL);
    g_signal_connect(btn_img_stop, "clicked", G_CALLBACK(stop_image_capture), NULL);

    g_signal_connect(btn_splice_start, "clicked", G_CALLBACK(start_feed_splicer), NULL);
    g_signal_connect(btn_splice_stop, "clicked", G_CALLBACK(stop_feed_splicer), NULL);

    g_signal_connect(btn_detect_start, "clicked", G_CALLBACK(start_creasing_detector), NULL);
    g_signal_connect(btn_detect_stop, "clicked", G_CALLBACK(stop_creasing_detector), NULL);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}
