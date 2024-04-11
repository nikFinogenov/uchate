#include "uchat-client.h"
GtkWidget *error_poppi_upe;

// TODO
// static char **argv_ptr = {"127.0.0.1", "8888"};
// char *host_name = "127.0.0.1";
// int port = 7777;

// static int sockfd = -1;
void on_close_button_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_destroy(error_poppi_upe);
}
void *show_error(void) {
    if (!g_main_context_default() || !g_main_context_is_owner(g_main_context_default())) {
        // g_print("ya ya ya pidoras\n");
        // g_warning("show_error should only be called from the main thread");
        return NULL;
    }
    
    // char *text = "Connection lost\nTry again later";
    // Determine the parent window
    GtkWidget *parent_window = NULL;
    if (gtk_widget_get_visible(user_window)) {
        parent_window = user_window;
    } else if (gtk_widget_get_visible(login_window)) {
        parent_window = login_window;
    }
    else if(gtk_widget_get_visible(signup_window)) parent_window = signup_window;

    // Create a pop-up dialog
    error_poppi_upe = gtk_dialog_new_with_buttons("Error", 
                                                           GTK_WINDOW(parent_window),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           NULL);
    gtk_window_set_position(GTK_WINDOW(error_poppi_upe), GTK_WIN_POS_CENTER_ON_PARENT);

    gtk_window_set_default_size(GTK_WINDOW(error_poppi_upe), 200, 100);

    // Add some content to the dialog
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(error_poppi_upe));

    GtkWidget *label = gtk_label_new("Connection lost");
    gtk_container_add(GTK_CONTAINER(content_area), label);

    GtkWidget *close = gtk_button_new_with_label("Close");
    gtk_container_add(GTK_CONTAINER(content_area), close);
    g_signal_connect(close, "clicked", G_CALLBACK(on_close_button_clicked), NULL);

    // Center the widgets vertically
    gtk_widget_set_vexpand(close, TRUE);
    gtk_widget_set_vexpand(label, TRUE);
    gtk_widget_set_valign(close, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

    // Center the content area
    gtk_widget_set_halign(content_area, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(content_area, GTK_ALIGN_CENTER);

    gtk_widget_show_all(error_poppi_upe);
    
    // Connect signal handler to close the dialog when the close button is clicked
    g_signal_connect_swapped(error_poppi_upe, "response", G_CALLBACK(gtk_widget_destroy), error_poppi_upe);
}

int connect_to_server(int *sock) {
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = atoi(argv_ptr[2]);
    *sock = socket(AF_INET, SOCK_STREAM, 0);

    if (*sock < 0) {
        perror("ERROR opening socket");
    }

    server = gethostbyname(argv_ptr[1]);
    
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        close(*sock);
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(*sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting to the server!");
        close(*sock);

        // Call show_error only from the main thread
        if (g_main_context_default() && g_main_context_is_owner(g_main_context_default())) {
            // Run show_error directly since we're in the main thread
            // char *err_msg = "Connection lost\nTry again later";
            show_error();
        } else {
            // If not in the main thread, create a new thread to run show_error
            pthread_t thread_id;
            // char *err_msg = "";
            pthread_create(&thread_id, NULL, show_error, NULL);
        }

        return -1;
    }
    return 0;
}

char **send_sign_up_data(char *first_name, char *last_name, char *username, char *password) {
    // Connect to the server if not yet
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    //sprintf(sendBuffer, "/chat/add\n%s\n%s\n%s\n", id1, id2, date);
    //sprintf(sendBuffer, "/messages/add\n%s\n%s\n%s\n%s\n", chat_id, text, type, status);
    sprintf(sendBuffer, "/user/add\n%s\n%s\n%s\n%s\n", username, password, first_name, last_name);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        if(error_revealer == NULL) pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");

    // char **user_recv_data = mx_strsplit(recvBuffer, '\n');
    return recvBuffer;
}

char **check_login_data(char *username, char* password) {
    // Connect to the server if not yet
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    //sprintf(sendBuffer, "/chat/add\n%s\n%s\n%s\n", id1, id2, date);
    //sprintf(sendBuffer, "/messages/add\n%s\n%s\n%s\n%s\n", chat_id, text, type, status);
    sprintf(sendBuffer, "/user/check-login-data\n%s\n%s\n", username, password);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);


    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        if(error_revealer == NULL) pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    // char **user_recv_data = mx_strsplit(recvBuffer, '\n');
    return recvBuffer;
}

char **get_chatter_data(char *username) {
    // Connect to the server if not yet
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    //sprintf(sendBuffer, "/chat/add\n%s\n%s\n%s\n", id1, id2, date);
    //sprintf(sendBuffer, "/messages/add\n%s\n%s\n%s\n%s\n", chat_id, text, type, status);
    sprintf(sendBuffer, "/user/get-chatter\n%s\n", username);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);


    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    // char **user_recv_data = mx_strsplit(recvBuffer, '\n');
    return recvBuffer;
}

char **send_new_chat_data(char *username1, char* username2) {
    // g_print("%s\n%s\n", username1, username2);
    // Connect to the server if not yet
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    //sprintf(sendBuffer, "/chat/add\n%s\n%s\n%s\n", id1, id2, date);
    //sprintf(sendBuffer, "/messages/add\n%s\n%s\n%s\n%s\n", chat_id, text, type, status);
    sprintf(sendBuffer, "/chat/add\n%s\n%s\n", username1, username2);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        if(error_revealer == NULL) pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");

    // char **user_recv_data = mx_strsplit(recvBuffer, '\n');
    return recvBuffer;
}

char **get_chats_data(char *username) {
        // Connect to the server if not yet
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    //sprintf(sendBuffer, "/chat/add\n%s\n%s\n%s\n", id1, id2, date);
    //sprintf(sendBuffer, "/messages/add\n%s\n%s\n%s\n%s\n", chat_id, text, type, status);
    sprintf(sendBuffer, "/chat/get\n%s\n", username);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);


    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        // char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    // char **user_recv_data = mx_strsplit(recvBuffer, '\n');
    return recvBuffer;
}

// static void *check_last_room(void *data) {
//     mx_connect_to_server(&sock_for_chats);
//     while (true) {
//         if (sock_for_chats == -1) {
//             mx_connect_to_server(&sock_for_chats);
//             continue;
//         }
        // t_chats_list *node = chats_list_head;
        // int last_uid = 0;

        // GList *children = gtk_container_get_children(GTK_CONTAINER(chats_list));
        // while (true) {
        //     if (node->room == g_list_nth_data(children, 0)) {
        //         last_uid = node->uid;
        //         break;
        //     }
        //     else
        //         node = node->next;
        // }
        // g_list_free(children);

        
    //     char sendBuff[256];
    //     bzero(sendBuff, 256);
    //     sprintf(sendBuff, "CheckLastRoom\n%d\n%d", t_user.id, (int)last_uid);

    //     if(send(sock_for_rooms, sendBuff, 256, 0) == -1){
    //         pthread_t thread_id;
    //         char *err_msg = "Connection lost\nTry again later";
    //         pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
    //         sock_for_rooms = -1;
    //         return NULL;
    //     }
        

    //     int serv_last_uid = 0;

    //     if(recv(sock_for_rooms, &serv_last_uid, sizeof(int), 0) == 0){
    //         pthread_t thread_id;
    //         char *err_msg = "Connection lost\nTry again later";
    //         pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
    //         sock_for_rooms = -1;
    //         return NULL;
    //     }

    //     if (serv_last_uid != last_uid) {
    //         node = chats_list_head;
    //         while (node != NULL) {
    //             if (node->uid == serv_last_uid) {
    //                 gtk_box_reorder_child(GTK_BOX(chats_list), node->room, 0);
    //                 gtk_widget_set_state_flags(GTK_WIDGET(node->room), GTK_STATE_FLAG_LINK, FALSE);
    //                 break;
    //             }
    //             else
    //                 node = node->next;
    //         }
    //         if (node == NULL) {
    //             gtk_box_pack_start(GTK_BOX(chats_list), 
    //                 mx_create_room((unsigned int)serv_last_uid, L_FIELD_WIDTH, room_click), FALSE, FALSE, 0);
    //             node = chats_list_head;
    //             while (node->uid != serv_last_uid)
    //                 node = node->next;
    //             gtk_box_reorder_child(GTK_BOX(chats_list), node->room, 0);
    //             gtk_widget_set_state_flags(GTK_WIDGET(node->room), GTK_STATE_FLAG_LINK, TRUE);
    //             gtk_widget_show_all(GTK_WIDGET(node->room));
    //         }
    //     }
    //     usleep(500000);
    // }

    // return NULL;
// }


// void mx_configure_chats_list(void) {
//     if (sockfd == -1){
//         mx_connect_to_server(&sockfd);
//         //return 1;
//     }
//     // chats_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
//     // gtk_fixed_put(GTK_FIXED(chat_area), chats_list, 0, 105);

//     char sendBuffer[1024];
//     bzero(sendBuffer, 1024);
//     // sprintf(sendBuffer, "GetUsersArr\n%d", t_user.id);

//     int error = 0;
//     socklen_t len = sizeof (error);
//     int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);
//     if (retval != 0) {
//         fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
//         sockfd = -1;
//         return;
//     }
//     if (error != 0) {
//         fprintf(stderr, "socket error: %s\n", strerror(error));
//         sockfd = -1;
//          return;
//     }

//     if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) == -1) {
//         perror("ERROR writing to socket");
//         pthread_t thread_id;
//         // char *err_msg = "Connection lost\nTry again later";
//         pthread_create(&thread_id, NULL, show_error, NULL); 
//         sockfd = -1;
//         return;
//     }
    
//     int rooms_uids_len = 0;
    
//     if(recv(sockfd, &rooms_uids_len, sizeof(int), 0) == 0){
//         pthread_t thread_id;
//         // char *err_msg = "Connection lost\nTry again later";
//         pthread_create(&thread_id, NULL, show_error, NULL); 
//         sockfd = -1;
//         return;
//     }

//     unsigned int *rooms_uids = (unsigned int *)malloc(rooms_uids_len);
//     for (int i = 0; i < rooms_uids_len; i++)
//         if(recv(sockfd, &rooms_uids[i], sizeof(unsigned int), 0) == 0){
//             pthread_t thread_id;
//             // char *err_msg = "Connection lost\nTry again later";
//             pthread_create(&thread_id, NULL, show_error,NULL ); 
//             sockfd = -1;
//             return;
//         }
    
//     for (int i = 0; i < rooms_uids_len; i++)
//         gtk_box_pack_start(GTK_BOX(chats_list), 
//             mx_create_room(rooms_uids[i], L_FIELD_WIDTH, room_click), FALSE, FALSE, 0);
    
//     free(rooms_uids);

//     pthread_create(&check_last_room_id, NULL, check_last_room, NULL);
// }

