#include "server.h"

static void handle_request(char* request, char** data, int new_socket) {
    //utils-endpoint
    if (!mx_strcmp(request, "SearchInit")) mx_search_init(data, new_socket);
    else if (!mx_strcmp(request, "LoadMessages")) mx_load_messages(data, new_socket);
    else if (!mx_strcmp(request, "Authorization")) mx_authorization(data, new_socket);
    //user-endpoints
    else if (!mx_strcmp(request, "/user/add")) mx_add_user(data, new_socket);
    else if (!mx_strcmp(request, "/user/get")) mx_get_user(data, new_socket);
    else if (!mx_strcmp(request, "/user/get-avatar")) mx_get_user_avatar(data, new_socket);
    else if (!mx_strcmp(request, "/user/get-chatter")) mx_get_chatter(data, new_socket);
    else if (!mx_strcmp(request, "/user/update")) mx_update_user(data, new_socket);
    else if (!mx_strcmp(request, "/user/update-avatar")) mx_update_avatar(data, new_socket);
    else if (!mx_strcmp(request, "/user/delete")) mx_delete_user(data);
    else if (!mx_strcmp(request, "/user/check-login-data")) check_login_data(data, new_socket);
    //message-endpoints
    else if (!mx_strcmp(request, "/messages/add")) mx_add_message(data, new_socket);
    else if (!mx_strcmp(request, "/messages/get")) mx_get_message(data, new_socket);
    else if (!mx_strcmp(request, "/messages/update")) mx_update_message(data);
    else if (!mx_strcmp(request, "/messages/delete")) mx_delete_message(data);
    //chat-endpoints
    else if (!mx_strcmp(request, "/chat/add")) mx_create_chat(data, new_socket);
    else if (!mx_strcmp(request, "/chat/get")) mx_get_chat(data, new_socket);
    else if (!mx_strcmp(request, "/chat/update")) mx_update_chat(data);
    else if (!mx_strcmp(request, "/chat/delete")) mx_delete_chat(data);
    //xyeta-endpoints
    // else if (!mx_strcmp(request, "CheckLastRoom")) mx_check_last_room(data, new_socket);
    // else if (!mx_strcmp(request, "AddImageMessage")) message_img(data, new_socket);
    // else if (!mx_strcmp(request, "GetMessageImage")) mx_get_image_message(data, new_socket);
    // else if (!mx_strcmp(request, "LoadRoom")) mx_load_room(data, new_socket);
    // else if (!mx_strcmp(request, "/user/getsArr")) mx_get_users_arr(data, new_socket);
    // else if (!mx_strcmp(request, "SendRoomData")) mx_send_room_data(data, new_socket);
    // else if (!mx_strcmp(request, "GetAvatar")) mx_get_avatar(data, new_socket);
    // else if (!mx_strcmp(request, "UpdateAvatar")) mx_update_avatar(data, new_socket);
    // else if (!mx_strcmp(request, "UpdateLanguage")) mx_update_language(data);
    // else if (!mx_strcmp(request, "GetLanguage")) mx_get_language(data, new_socket);
    // else if (!mx_strcmp(request, "GetTheme")) mx_get_theme(data, new_socket);
    // else if (!mx_strcmp(request, "UpdateTheme")) mx_update_theme(data);
}

static void *handle_client(void *data) {
    int client_socket = (int)(size_t)data;

    while(1) {
        char recv_buff[6000];
        memset(recv_buff, 0, sizeof(recv_buff));

        int bytesRead = recv(client_socket, recv_buff, sizeof(recv_buff), 0);
        if (bytesRead > 0) {
            char **recvData = mx_strsplit(recv_buff, '\n');
            char *req = recvData[0];
            handle_request(req, recvData, client_socket);
            mx_del_strarr(&recvData);
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    db_init();
    if (argc != 2) {
        mx_printerr("usage: ./server [port]\n");
        exit(1);
    }

    int port = atoi(argv[1]);
    if (port <= 0) {
        mx_printerr("Invalid port number\n");
        exit(1);
    }
    pid_t pid = fork();
    if(pid < 0) {
        mx_printerr("error: deamon launch failed\nforking error\n");
        exit(1);
    }
    if (pid > 0) exit(0);
    else {
        printf("Server OK\nPID -> %d\n", getpid());
        umask(0);     
        close(1);  
        close(2);
        close(3);    
        setsid();   
    }
    int listening_socket = socket_init(port);
    listen(listening_socket, SOMAXCONN);

    while(true) {
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);
        int newsocketfd = accept(listening_socket, (struct sockaddr *)&client, &client_len);

        if (newsocketfd < 0) continue;

        pthread_t thread_id;
        pthread_create(&thread_id, NULL, handle_client, (void *)(size_t)newsocketfd); 
    }

    return 0;
}
