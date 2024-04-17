#include "uchat-client.h"

static GtkWidget *error_label = NULL;

typedef struct {
    GtkWidget *username_entry;
    GtkWidget *password_entry;
} EntryWidgets;
    
void show_login(void) {
    gtk_widget_show_all(login_window);
}

void go_to_signup(void) {
    gtk_widget_hide(login_window);
    show_signup();
}

static void display_error_message(char *message) {
    error_label = gtk_label_new(message);
    gtk_widget_override_color(error_label, GTK_STATE_FLAG_NORMAL, RED_CVET);
    gtk_box_pack_start(GTK_BOX(gtk_bin_get_child(GTK_BIN(login_window))), error_label, FALSE, FALSE, 0);
    gtk_widget_show_all(login_window);
}

static void login_button_clicked(GtkWidget *widget, gpointer data) {
    EntryWidgets *entries = (EntryWidgets *)data;
    char *avatar_path = (char *)malloc(strlen(AVATAR_FOLDER) + strlen(user.username) + strlen("_avatar.png") + 1);
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(entries->username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(entries->password_entry));
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
    

    char **response = check_login_data(parsed_username, parsed_password);
    if (strcmp((char *)response, "1") == 0) {
        display_error_message("Username or Password is incorrect");
        return;
    }
    if (strcmp((char *)response, "1488") == 0) {
        display_error_message("Server offline");
        return;
    }
    
    char *token = strtok((char *)response, "\n");
    user.username = strdup(token);

    token = strtok(NULL, "\n");
    user.name = strdup(token);

    token = strtok(NULL, "\n");
    user.surname = strdup(token);

    token = strtok(NULL, "\n");
    user.desc = strdup(token);

    char **status_response = get_user_status(user.username);
    char *tok = strtok((char *)status_response, "\n");
    user.status = strdup(tok);
    if (strcmp(user.status, "online") == 0){
        display_error_message("User is already logged in");
        return;
    }

    create_txt_with_data(login_info, user.username, parsed_password, userdata.button_recognize);
    get_and_save_avatar_to_file(user.username);
    sprintf(avatar_path, "%s%s_avatar.png", AVATAR_FOLDER, user.username);
    user.avatar = gdk_pixbuf_new_from_file(avatar_path, NULL);
    remove(avatar_path);
    free(avatar_path);
    update_user_status("online", user.username);

    gtk_widget_hide(login_window);
    g_print("%d\n", server_chats_quantity(user.username));
    load_chats(user.username);
    load_message(user.username);
    draw_user_window();
    show_user_window();
    start_chat_checker(user.username);
}

static gboolean on_entry_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
    if (event->keyval == GDK_KEY_Return || event->keyval == GDK_KEY_KP_Enter) {
        EntryWidgets *callback_data = (EntryWidgets *)user_data;
        login_button_clicked(GTK_WIDGET(callback_data->password_entry), callback_data);
        return TRUE;
    }

    return FALSE;
}

void on_check_button_toggled(GtkToggleButton *togglebutton, gpointer data) {
    gboolean active = gtk_toggle_button_get_active(togglebutton);
    if (active) {
        userdata.button_recognize = TRUE;
        g_print("Check button is checked\n");
    }
    else {
        userdata.button_recognize = FALSE;
        g_print("Check button is unchecked\n");
    }
}

void draw_login(void) {
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
    g_signal_connect(signup_link, "activate-link", G_CALLBACK(go_to_signup), NULL);
    gtk_box_pack_start(GTK_BOX(login_vbox), signup_link, FALSE, FALSE, 0);

    GtkWidget *username_entry_login = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry_login), "Username");
    gtk_box_pack_start(GTK_BOX(login_vbox), username_entry_login, FALSE, FALSE, 0);

    GtkWidget *password_entry_login = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry_login), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(password_entry_login), FALSE);
    gtk_box_pack_start(GTK_BOX(login_vbox), password_entry_login, FALSE, FALSE, 0);

    GtkWidget *check_button = gtk_check_button_new_with_label("Remember me");
    gtk_box_pack_start(GTK_BOX(login_vbox), check_button, FALSE, FALSE, 0);

    EntryWidgets *entries = g_new(EntryWidgets, 1);
    entries->username_entry = username_entry_login;
    entries->password_entry = password_entry_login;

    GtkWidget *login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(login_button_clicked), entries);
    g_signal_connect(G_OBJECT(password_entry_login), "key-press-event", G_CALLBACK(on_entry_key_press), entries);
    g_signal_connect(G_OBJECT(check_button), "toggled", G_CALLBACK(on_check_button_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(login_vbox), login_button, FALSE, FALSE, 0);
}
