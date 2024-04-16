#include "uchat-client.h"

GtkWidget *error_poppi_upe;

void on_close_button_clicked(GtkWidget *widget, gpointer data) {
    gtk_widget_destroy(error_poppi_upe);
}

void *show_error(void) {
    if (!g_main_context_default() || !g_main_context_is_owner(g_main_context_default())) {

        return NULL;
    }
    
    GtkWidget *parent_window = NULL;
    if (gtk_widget_get_visible(user_window)) {
        parent_window = user_window;
    } else if (gtk_widget_get_visible(login_window)) {
        parent_window = login_window;
    }
    else if(gtk_widget_get_visible(signup_window)) parent_window = signup_window;

    error_poppi_upe = gtk_dialog_new_with_buttons("Error", 
                                                           GTK_WINDOW(parent_window),
                                                           GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                           NULL, GTK_RESPONSE_NONE, NULL);
    gtk_window_set_position(GTK_WINDOW(error_poppi_upe), GTK_WIN_POS_CENTER_ON_PARENT);

    gtk_window_set_default_size(GTK_WINDOW(error_poppi_upe), 200, 100);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(error_poppi_upe));

    GtkWidget *label = gtk_label_new("Connection lost");
    gtk_container_add(GTK_CONTAINER(content_area), label);

    GtkWidget *close = gtk_button_new_with_label("Close");
    gtk_container_add(GTK_CONTAINER(content_area), close);
    g_signal_connect(close, "clicked", G_CALLBACK(on_close_button_clicked), NULL);

    gtk_widget_set_vexpand(close, TRUE);
    gtk_widget_set_vexpand(label, TRUE);
    gtk_widget_set_valign(close, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);

    gtk_widget_set_halign(content_area, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(content_area, GTK_ALIGN_CENTER);

    gtk_widget_show_all(error_poppi_upe);
    
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

        if (g_main_context_default() && g_main_context_is_owner(g_main_context_default())) {
            show_error();
        } else {
            pthread_t thread_id;
            pthread_create(&thread_id, NULL, show_error, NULL);
        }

        return -1;
    }
    return 0;
}

char **send_sign_up_data(char *first_name, char *last_name, char *username, char *password, char* status) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/user/add\n%s\n%s\n%s\n%s\n%s\n", username, password, first_name, last_name, status);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");

    return recvBuffer;
}

char **check_login_data(char *username, char* password) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/user/check-login-data\n%s\n%s\n", username, password);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

char **get_chatter_data(char *username) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/user/get-chatter\n%s\n", username);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

char **send_new_chat_data(char *username1, char* username2) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/chat/add\n%s\n%s\n", username1, username2);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");

    return recvBuffer;
}

char **get_chats_data(char *username) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/chat/get\n%s\n", username);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);


    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

char **update_user_info(char *changed_username, char *name, char *surname, char *desc, char *username) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/user/update\n%s\n%s\n%s\n%s\n%s\n", changed_username, name, surname, desc, username);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);


    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

char **update_message_info(int id, char *new_text) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/messages/update\n%s\n%s\n", mx_itoa(id), new_text);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);


    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

char **add_new_message(char *username_1, char *username_2, char* text, char* time, char* type) {
    if (sockfd == -1) connect_to_server(&sockfd);  
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/messages/add\n%s\n%s\n%s\n%s\n%s\n", username_1, username_2, text, time, type);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    
    if(sockfd == -1) sprintf(recvBuffer, "-1488");
    return recvBuffer;
}

char **delete_message_data(int id) {
    if (sockfd == -1) connect_to_server(&sockfd);  
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/messages/delete\n%s", mx_itoa(id));
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

char **chatter_delete(char *username_1, char *username_2) {
    if (sockfd == -1) connect_to_server(&sockfd);  
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/chat/delete\n%s\n%s\n", username_1, username_2);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);


    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

char **get_mess_chat_amount(char *username_1, char *username_2) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/message/amount\n%s\n%s\n", username_1, username_2);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);


    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

char **get_chat_messages(char *username_1, char *username_2) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/messages/get\n%s\n%s\n", username_1, username_2);
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;

        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

void get_and_save_avatar_to_file(char *username) {
    if (connect_to_server(&sockfd) == -1) {
        fprintf(stderr, "Error connecting to server\n");
        return;
    }

    char sendBuffer[MAX_BUFFER_SIZE];
    snprintf(sendBuffer, sizeof(sendBuffer), "/user/get-avatar\n%s\n", username);

    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("Error writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        close(sockfd);
        return;
    }

    char filename[MAX_BUFFER_SIZE];
    snprintf(filename, sizeof(filename), "%s%s_avatar.png", AVATAR_FOLDER, username);
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Error creating file");
        close(sockfd);
        return;
    }

    int file_size;
    if (recv(sockfd, &file_size, sizeof(file_size), 0) == -1) {
        perror("Error receiving data size through socket");
        fclose(file);
        close(sockfd);
        return;
    }

    char *blob_data = (char *)malloc(file_size);
    if (blob_data == NULL) {
        perror("Error allocating memory for blob data");
        fclose(file);
        close(sockfd);
        return;
    }

    ssize_t total_bytes_received = 0;
    while (total_bytes_received < file_size) {
        ssize_t bytes_received = recv(sockfd, blob_data + total_bytes_received, file_size - total_bytes_received, 0);
        if (bytes_received == -1) {
            perror("Error receiving data through socket");
            free(blob_data);
            fclose(file);
            close(sockfd);
            return;
        }
        total_bytes_received += bytes_received;
    }

    fwrite(blob_data, sizeof(char), file_size, file);

    free(blob_data);
    
    fclose(file);
}

void update_avatar(char *path, char *username) {
    if (connect_to_server(&sockfd) == -1) {
        fprintf(stderr, "Error connecting to server\n");
        return;
    }
    char sendBuffer[DEFAULT_MESSAGE_SIZE];
    memset(sendBuffer, 0, DEFAULT_MESSAGE_SIZE);
    sprintf(sendBuffer, "/user/update-avatar\n%s\n%s\n", path, username);

    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    memset(recvBuffer, 0, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

char **get_user_status(char *username) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/user/get-status\n%s\n", username);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}

char **update_user_status(char *status, char *username) {
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/user/update-status\n%s\n%s\n", status, username);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);


    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }

    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL);
        sockfd = -1;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, show_error, NULL); 
        sockfd = -1;
    }
    if(sockfd == -1) sprintf(recvBuffer, "1488");
    return recvBuffer;
}
