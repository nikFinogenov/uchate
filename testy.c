#include <gtk/gtk.h>

void on_button_press(GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog;

    dialog = gtk_message_dialog_new(GTK_WINDOW(user_data),
                                    GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Open Settings Page!");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *box;
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    gtk_init(&argc, &argv);

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple App with Settings");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box container
    box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), box);

    // Create the settings button
    button = gtk_button_new();
    gtk_widget_set_name(button, "settings-button");
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_button_press), window);

    // Add the button to the box
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);

    // Load CSS from file
    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen,
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_css_provider_load_from_path(provider, "client/style.css", NULL);

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}
