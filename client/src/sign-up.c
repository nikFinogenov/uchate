#include "uchat-client.h"

static GtkWidget *signup_window;

// Define a structure to hold the necessary data
typedef struct {
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *repeat_password_entry;
} EntryWidgets;

void show_signup() {
    gtk_widget_show_all(signup_window);
}

void go_to_login() {
    gtk_widget_hide(signup_window);
    show_login();
}

static void signup_button_clicked(GtkWidget *widget, gpointer data) {
    // Cast the data pointer to the EntryWidgets structure
    EntryWidgets *entries = (EntryWidgets *)data;

    // Get the username and password from the entry widgets
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(entries->username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(entries->password_entry));
    const gchar *repeat_password = gtk_entry_get_text(GTK_ENTRY(entries->repeat_password_entry));

    // Parsing const gchar* to char*
    char *parsed_username = (char*)username;
    char *parsed_password = (char*)password;
    char *parsed_repeat_password = (char*)repeat_password;

    if (strcmp(parsed_password, parsed_repeat_password) != 0) {
        // TODO: display error message on the sign up page
        printf("The passwords do not match. Please try again!\n");
        return;
    }

    // Print the username and password (you can replace this with whatever you want to do with the data)
    g_print("Username: %s\n", username);
    g_print("Password: %s\n", password);
    g_print("Password: %s\n", repeat_password);

    char *response = send_sign_up_data(username, password);

    g_print("%s\n", response);


    // If login data is correct
    gtk_widget_destroy(signup_window);
    // gtk_widget_hide(login_window);
    draw_user_window();
    show_user_window();
}

void draw_singup() {
    // Sign up window
    signup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(signup_window), "McOk Chat - Sign Up");
    gtk_window_set_default_size(GTK_WINDOW(signup_window), 400, 300);
    gtk_window_set_position(GTK_WINDOW(signup_window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(signup_window), 20);
    g_signal_connect(signup_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_hide(signup_window);

    GtkWidget *signup_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(signup_window), signup_vbox);

    GtkWidget *signup_label = gtk_label_new("McOk");
    gtk_widget_set_halign(signup_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(signup_vbox), signup_label, FALSE, FALSE, 0);

    GtkWidget *login_link = gtk_label_new("Already have an account? Login");
    gtk_label_set_markup(GTK_LABEL(login_link), "Already have an account? <a href=\"#\">Login</a>");
    gtk_widget_set_halign(login_link, GTK_ALIGN_CENTER);
    g_signal_connect(login_link, "activate-link", G_CALLBACK(go_to_login), NULL);
    gtk_box_pack_start(GTK_BOX(signup_vbox), login_link, FALSE, FALSE, 0);

    GtkWidget *username_entry_signup = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry_signup), "Username");
    gtk_box_pack_start(GTK_BOX(signup_vbox), username_entry_signup, FALSE, FALSE, 0);

    GtkWidget *password_entry_signup = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry_signup), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry_signup), FALSE);
    gtk_box_pack_start(GTK_BOX(signup_vbox), password_entry_signup, FALSE, FALSE, 0);

    GtkWidget *repeat_password_signup = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(repeat_password_signup), "Repeat Password");
    gtk_entry_set_visibility(GTK_ENTRY(repeat_password_signup), FALSE);
    gtk_box_pack_start(GTK_BOX(signup_vbox), repeat_password_signup, FALSE, FALSE, 0);

    // Create a structure to hold the entry widgets
    EntryWidgets *entries = g_new(EntryWidgets, 1);
    entries->username_entry = username_entry_signup;
    entries->password_entry = password_entry_signup;
    entries->repeat_password_entry = repeat_password_signup;

    GtkWidget *signup_button = gtk_button_new_with_label("Sign Up");
    g_signal_connect(signup_button, "clicked", G_CALLBACK(signup_button_clicked), entries);
    gtk_box_pack_start(GTK_BOX(signup_vbox), signup_button, FALSE, FALSE, 0);
}

