// #include <gtk/gtk.h>

// static void activate(GtkApplication* app, gpointer user_data) {
//     GtkWidget *window;

//     // Create a new window
//     window = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window), "GTK+3 with CSS Example");
//     gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

//     // Create a label
//     GtkWidget *label = gtk_label_new("Hello, GTK!");

//     // Set a name for the label (used in CSS)
//     gtk_widget_set_name(label, "myLabel");

//     // Add the label to the window
//     gtk_container_add(GTK_CONTAINER(window), label);

//     // Load CSS
//     // GtkCssProvider *provider = gtk_css_provider_new();
//     GError *error = NULL;

//     GtkCssProvider *cssProvider = gtk_css_provider_new();
//     gtk_css_provider_load_from_path(cssProvider, "style.css", NULL);
//     gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
//         GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);


//     // Apply the CSS to the window
//     // GtkStyleContext *context = gtk_widget_get_style_context(window);
//     // gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

//     // Show all elements
//     gtk_widget_show_all(window);
// }

// int main(int argc, char **argv) {
//     GtkApplication *app;
//     int status;

//     // Create a new GTK application
//     app = gtk_application_new("org.example.GtkCssExample", G_APPLICATION_FLAGS_NONE);
//     g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

//     // Run the application
//     status = g_application_run(G_APPLICATION(app), argc, argv);

//     // Clean up
//     g_object_unref(app);

//     return status;
// }
#include <gtk/gtk.h>

// Global variables to store username and password
const gchar *global_username;
const gchar *global_password;



// Function to create the main window
void create_main_window() {
    // Create and configure the main window
    GtkWidget *main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Main Window");
    gtk_window_set_default_size(GTK_WINDOW(main_window), 300, 200);

    // Create labels to display username and password
    GtkWidget *username_label = gtk_label_new(NULL);
    GtkWidget *password_label = gtk_label_new(NULL);
    gchar *username_text = g_strdup_printf("Username: %s", global_username);
    gchar *password_text = g_strdup_printf("Password: %s", global_password);
    gtk_label_set_text(GTK_LABEL(username_label), username_text);
    gtk_label_set_text(GTK_LABEL(password_label), password_text);
    g_free(username_text);
    g_free(password_text);

    // Create a vertical box to organize widgets
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(main_window), vbox);

    // Add labels to the main window
    gtk_box_pack_start(GTK_BOX(vbox), username_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), password_label, FALSE, FALSE, 0);

    // Show all widgets in the main window
    gtk_widget_show_all(main_window);
}
// Function to handle sign-up button click
void sign_up_clicked(GtkWidget *button, gpointer data) {
    // Extract user input from text fields
    GtkWidget *username_entry = (GtkWidget *)data;
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    GtkWidget *password_entry = (GtkWidget *)g_object_get_data(G_OBJECT(data), "password_entry");
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    // Store username and password
    global_username = username;
    global_password = password;

    // Close the sign-up window
    gtk_widget_destroy(gtk_widget_get_toplevel(button));

    // Create the main window
    create_main_window();
}

// Function to create the sign-up page
void create_sign_up_page() {
    // Create and configure the sign-up window
    GtkWidget *sign_up_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(sign_up_window), "Sign Up");
    gtk_window_set_default_size(GTK_WINDOW(sign_up_window), 300, 200);

    // Create a vertical box to organize widgets
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(sign_up_window), vbox);

    // Create labels and entries for username and password
    GtkWidget *username_label = gtk_label_new("Username:");
    GtkWidget *username_entry = gtk_entry_new();
    GtkWidget *password_label = gtk_label_new("Password:");
    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE); // Hide the password

    // Add labels and entries to the vertical box
    gtk_box_pack_start(GTK_BOX(vbox), username_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), username_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), password_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), password_entry, FALSE, FALSE, 0);

    // Create a sign-up button
    GtkWidget *sign_up_button = gtk_button_new_with_label("Sign Up");
    // Connect sign-up button click event
    g_signal_connect(sign_up_button, "clicked", G_CALLBACK(sign_up_clicked), username_entry);
    // Set data for password entry
    g_object_set_data(G_OBJECT(username_entry), "password_entry", password_entry);

    // Add the sign-up button to the vertical box
    gtk_box_pack_start(GTK_BOX(vbox), sign_up_button, FALSE, FALSE, 0);

    // Show all widgets in the sign-up window
    gtk_widget_show_all(sign_up_window);
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the sign-up page
    create_sign_up_page();

    // Run the GTK main loop
    gtk_main();

    return 0;
}

