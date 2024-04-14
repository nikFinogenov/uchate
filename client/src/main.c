#include "uchat-client.h"
#include <glib.h>
// void log_handler(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data) {
//     // Check if the log level indicates a critical warning
//     if (log_level & (G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_ERROR | G_LOG_LEVEL_WARNING)) {
//         // Output the log message
//         g_log_default_handler(log_domain, log_level, message, user_data);

//         // If you want to break on the critical warning, you can use abort() here
//         // abort();
//     }
// }

int main(int argc, char *argv[]) {
    if (argc != 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    sockfd = -1;
    sock_for_chats = -1;
    argv_ptr = argv;
    // GtkCssProvider *cssProvider = gtk_css_provider_new();
    // gtk_css_provider_load_from_path(cssProvider, "client/style.css", NULL);
    // gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
    // GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_init(&argc, &argv);
    // g_log_set_handler(NULL, G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION, log_handler, NULL);
    
    fill_data();

    // Get screen width and height
    GdkScreen *screen = gdk_screen_get_default();
    gint screen_width = gdk_screen_get_width(screen);
    gint screen_height = gdk_screen_get_height(screen);


    // Draw windows
    draw_login();
    draw_singup();
    
    // read_json_from_file("client/client-data/login_info.json", &userdata);
    // if()
    read_txt_from_file(login_info, &userdata);

    char *avatar_path = (char *)malloc(strlen(AVATAR_FOLDER) + strlen(user.username) + strlen("_avatar.png") + 1);

    if (userdata.button_recognize) {
        char **status_response = get_user_status(userdata.username);
        char *tok = strtok(status_response, "\n");
        user.status = strdup(tok);

        if (strcmp(user.status, "online") == 0){
            GtkWidget *dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error: User is already logged in\n");
            gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
            gtk_dialog_run(GTK_DIALOG(dialog));
            gtk_widget_destroy(dialog);
        } else {
            char **response = check_login_data(userdata.username, userdata.password);
            char *token = strtok(response, "\n");
            user.username = strdup(token);
            // token = strtok(NULL, "\n");
            // g_print("passs -> %s\n", token);
            token = strtok(NULL, "\n");
            user.name = strdup(token);
            token = strtok(NULL, "\n");
            user.surname = strdup(token);
            get_and_save_avatar_to_file(user.username);
            sprintf(avatar_path, "%s%s_avatar.png", AVATAR_FOLDER, user.username);
            g_print("%s\n", avatar_path);
            user.avatar = gdk_pixbuf_new_from_file(avatar_path, NULL);
            remove(avatar_path);
            free(avatar_path);
            update_user_status("online", user.username);
            load_chats(user.username);
            load_message(user.username);
            draw_user_window();
            show_user_window();
        }
    } else {
        show_login();
    }

    // printf("Username: %s\n", userdata.username);
    // printf("Password: %s\n", userdata.password);
    // printf("Button Recognize: %s\n", userdata.button_recognize ? "true" : "false");
    // // Set background color for both windows
    // GdkRGBA color;
    // gdk_rgba_parse(&color, "#171717");
    // gtk_widget_override_background_color(login_window, GTK_STATE_FLAG_NORMAL, &color);
    // gtk_widget_override_background_color(signup_window, GTK_STATE_FLAG_NORMAL, &color);
    //
    gtk_main();

    return 0;
}
