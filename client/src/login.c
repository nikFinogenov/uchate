#include "uchat-client.h"

static GtkWidget *login_window;
static GtkWidget *error_label = NULL;

// Define a structure to hold the necessary data
typedef struct {
    GtkWidget *username_entry;
    GtkWidget *password_entry;
} EntryWidgets;

void show_login() {
    gtk_widget_show_all(login_window);
}

void go_to_signup() {
    gtk_widget_hide(login_window);
    show_signup();
}

static void display_error_message(char *message) {
    GdkRGBA color_red;
    gdk_rgba_parse(&color_red, "#de34eb");

    error_label = gtk_label_new(message);
    gtk_widget_modify_fg(error_label, GTK_STATE_NORMAL, &color_red);
    gtk_box_pack_start(GTK_BOX(gtk_bin_get_child(GTK_BIN(login_window))), error_label, FALSE, FALSE, 0);
    gtk_widget_show_all(login_window);
}

static void login_button_clicked(GtkWidget *widget, gpointer data) {
    // Cast the data pointer to the EntryWidgets structure
    EntryWidgets *entries = (EntryWidgets *)data;

    // Get the username and password from the entry widgets
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(entries->username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(entries->password_entry));

    // Parsing const gchar* to char*
    char *parsed_username = (char*)username;
    char *parsed_password = (char*)password;

    if (error_label != NULL) {
        gtk_widget_destroy(error_label);
        error_label = NULL;
    }

    if (strcmp(parsed_username, "") == 0) {
        display_error_message("Username cannot be empty");
        return;
    }

    if (strcmp(parsed_password, "") == 0) {
        display_error_message("Password cannot be empty");
        return;
    }

    // Send data and handle response
    char **response = check_login_data(parsed_username, parsed_password);

    if (strcmp(response, "1") == 0) {
        display_error_message("Username or Password is incorrect");
        return;
    }

    char *token = strtok(response, "\n");
    user.username = strdup(token);
    token = strtok(NULL, "\n");
    user.name = strdup(token);
    token = strtok(NULL, "\n");
    user.surname = strdup(token);

    // gtk_widget_destroy(login_window);
    gtk_widget_hide(login_window);
    draw_user_window();
    show_user_window();
}

void draw_login(void) {
    // Login window
    login_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(login_window), "McOk Chat - Login");
    gtk_window_set_default_size(GTK_WINDOW(login_window), 400, 300);
    gtk_window_set_position(GTK_WINDOW(login_window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(login_window), 20);
    // g_signal_connect(login_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *login_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(login_window), login_vbox);

    GtkWidget *login_label = gtk_label_new("McOk");
    gtk_widget_set_halign(login_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(login_vbox), login_label, FALSE, FALSE, 0);

    GtkWidget *signup_link = gtk_label_new("Don't have an account? Sign up");
    gtk_label_set_markup(GTK_LABEL(signup_link), "Don't have an account? <a href=\"#\">Sign up</a>"); //href=\"#\"
    gtk_widget_set_halign(signup_link, GTK_ALIGN_CENTER);
    g_signal_connect(signup_link, "activate-link", G_CALLBACK(go_to_signup), NULL);
    gtk_box_pack_start(GTK_BOX(login_vbox), signup_link, FALSE, FALSE, 0);

    GtkWidget *username_entry_login = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry_login), "Username");
    gtk_box_pack_start(GTK_BOX(login_vbox), username_entry_login, FALSE, FALSE, 0);

    GtkWidget *password_entry_login = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry_login), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry_login), FALSE);
    gtk_box_pack_start(GTK_BOX(login_vbox), password_entry_login, FALSE, FALSE, 0);

    // Create a structure to hold the entry widgets
    EntryWidgets *entries = g_new(EntryWidgets, 1);
    entries->username_entry = username_entry_login;
    entries->password_entry = password_entry_login;

    GtkWidget *login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(login_button_clicked), entries);
    gtk_box_pack_start(GTK_BOX(login_vbox), login_button, FALSE, FALSE, 0);
}


// static void send_request() { void }