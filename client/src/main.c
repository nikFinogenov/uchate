#include "uchat-client.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    sockfd = -1;
    sock_for_chats = -1;
    argv_ptr = argv;

    gtk_init(&argc, &argv);
    
    fill_data();

    GdkScreen *screen = gdk_screen_get_default();
    gint screen_width = gdk_screen_get_width(screen);
    gint screen_height = gdk_screen_get_height(screen);

    draw_login();
    draw_singup();
    
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
            g_signal_connect(GTK_WINDOW(dialog), "destroy", G_CALLBACK(exit), NULL);
            gtk_widget_destroy(dialog);
        } else {
            char **response = check_login_data(userdata.username, userdata.password);
            char *token = strtok(response, "\n");
            user.username = strdup(token);
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
            start_chat_checker(user.username);
        }
    } else {
        show_login();
    }

    gtk_main();

    return 0;
}
