#include <gtk/gtk.h>

static GtkWidget *login_window = NULL;
static GtkWidget *signup_window = NULL;

static void login_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Login button clicked\n");
}

static void signup_button_clicked(GtkWidget *widget, gpointer data) {
    g_print("Sign up button clicked\n");
}

static void show_signup_window(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(login_window);
    gtk_widget_show_all(signup_window);
}

static void show_login_window(GtkWidget *widget, gpointer data) {
    gtk_widget_hide(signup_window);
    gtk_widget_show_all(login_window);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Get screen width and height
    GdkScreen *screen = gdk_screen_get_default();
    gint screen_width = gdk_screen_get_width(screen);
    gint screen_height = gdk_screen_get_height(screen);

    // Login window
    login_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(login_window), "McOk Chat - Login");
    gtk_window_set_default_size(GTK_WINDOW(login_window), 400, 300);
    gtk_window_set_position(GTK_WINDOW(login_window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(login_window), 20);
    g_signal_connect(login_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *login_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(login_window), login_vbox);

    GtkWidget *login_label = gtk_label_new("McOk");
    gtk_widget_set_halign(login_label, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(login_vbox), login_label, FALSE, FALSE, 0);

    GtkWidget *signup_link = gtk_label_new("Don't have an account? Sign up");
    gtk_label_set_markup(GTK_LABEL(signup_link), "Don't have an account? <a href=\"#\">Sign up</a>");
    gtk_widget_set_halign(signup_link, GTK_ALIGN_CENTER);
    g_signal_connect(signup_link, "activate-link", G_CALLBACK(show_signup_window), NULL);
    gtk_box_pack_start(GTK_BOX(login_vbox), signup_link, FALSE, FALSE, 0);

    GtkWidget *username_entry_login = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry_login), "Username");
    gtk_box_pack_start(GTK_BOX(login_vbox), username_entry_login, FALSE, FALSE, 0);

    GtkWidget *password_entry_login = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry_login), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry_login), FALSE);
    gtk_box_pack_start(GTK_BOX(login_vbox), password_entry_login, FALSE, FALSE, 0);

    GtkWidget *login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(login_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(login_vbox), login_button, FALSE, FALSE, 0);

    // Signup window
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
    g_signal_connect(login_link, "activate-link", G_CALLBACK(show_login_window), NULL);
    gtk_box_pack_start(GTK_BOX(signup_vbox), login_link, FALSE, FALSE, 0);

    GtkWidget *username_entry_signup = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry_signup), "Username");
    gtk_box_pack_start(GTK_BOX(signup_vbox), username_entry_signup, FALSE, FALSE, 0);

    GtkWidget *password_entry_signup = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry_signup), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry_signup), FALSE);
    gtk_box_pack_start(GTK_BOX(signup_vbox), password_entry_signup, FALSE, FALSE, 0);

    GtkWidget *repeat_password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(repeat_password_entry), "Repeat Password");
    gtk_entry_set_visibility(GTK_ENTRY(repeat_password_entry), FALSE);
    gtk_box_pack_start(GTK_BOX(signup_vbox), repeat_password_entry, FALSE, FALSE, 0);

    GtkWidget *signup_button = gtk_button_new_with_label("Sign Up");
    g_signal_connect(signup_button, "clicked", G_CALLBACK(signup_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(signup_vbox), signup_button, FALSE, FALSE, 0);

    // Set background color for both windows
    GdkRGBA color;
    gdk_rgba_parse(&color, "#171717");
    gtk_widget_override_background_color(login_window, GTK_STATE_FLAG_NORMAL, &color);
    gtk_widget_override_background_color(signup_window, GTK_STATE_FLAG_NORMAL, &color);

    gtk_widget_show_all(login_window);

    gtk_main();

    return 0;
}
