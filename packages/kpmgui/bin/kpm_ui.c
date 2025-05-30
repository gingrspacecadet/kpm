#include <gtk/gtk.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// Reusable struct to store input and the command option
typedef struct {
    GtkWidget *entry;
    const char *command_flag;
} FeatureInput;

static void run_command(const char *arg_flag, const char *pkg_name) {
    if (!pkg_name || strlen(pkg_name) == 0) return;

    pid_t pid = fork();
    if (pid == 0) {
        execlp("kpm", "kpm", arg_flag, pkg_name, (char *)NULL);
        _exit(1); // if exec fails
    }
}

// Shared entry "activate" handler
static gboolean on_entry_activate(GtkWidget *widget, gpointer data) {
    FeatureInput *input = (FeatureInput *)data;
    const gchar *pkg_name = gtk_entry_get_text(GTK_ENTRY(widget));
    run_command(input->command_flag, pkg_name);

    gtk_entry_set_text(GTK_ENTRY(widget), "");
    gtk_widget_hide(input->entry);
    return TRUE;
}

// Shared button "clicked" handler
static void on_button_clicked(GtkButton *button, gpointer data) {
    FeatureInput *input = (FeatureInput *)data;
    gtk_widget_show(input->entry);
    gtk_widget_grab_focus(input->entry);
}

// Function to add a feature (button + hidden input)
static void add_feature(GtkWidget *grid, int row, const char *label, const char *flag, const char *placeholder) {
    GtkWidget *button = gtk_button_new_with_label(label);
    gtk_grid_attach(GTK_GRID(grid), button, 0, row, 1, 1);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), placeholder);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, row + 1, 1, 1);
    gtk_widget_hide(entry);

    FeatureInput *data = g_malloc(sizeof(FeatureInput));
    data->entry = entry;
    data->command_flag = flag;

    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), data);
    g_signal_connect(entry, "activate", G_CALLBACK(on_entry_activate), data);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "KPM GUI");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_container_add(GTK_CONTAINER(window), grid);

    int row = 0;
    add_feature(grid, row, "Install packages", "-S", "Enter package to install"); row += 2;
    add_feature(grid, row, "Remove packages", "-R", "Enter package to remove");   row += 2;

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
