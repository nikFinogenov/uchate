#include "uchat-client.h"

// TODO
// static char **argv_ptr = {"127.0.0.1", "8888"};
char *host_name = "127.0.0.1";
int port = 7777;

static int sockfd = -1;

int connect_to_server(int *sock) {
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = port;
    *sock = socket(AF_INET, SOCK_STREAM, 0);

    if (*sock < 0) {
        perror("ERROR opening socket");
    }

    server = gethostbyname(host_name);
    
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        close(*sock);
        exit(0);
    }

    
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(*sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        perror("ERROR connecting to the server!");
        close(*sock);
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        char*  error_revealer = NULL;
        if (error_revealer == NULL)
            // pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        return -1;
    }
    return 0;
}

char **send_sign_up_data(char *username, char *password) {
    // Connect to the server if not yet
    if (sockfd == -1) connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    //sprintf(sendBuffer, "/chat/add\n%s\n%s\n%s\n", id1, id2, date);
    //sprintf(sendBuffer, "/messages/add\n%s\n%s\n%s\n%s\n", chat_id, text, type, status);
    sprintf(sendBuffer, "/user/add\n%s\n%s\n", username, password);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);

    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
        return "Go fuck yourself once!";
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
         return "Go fuck yourself twice!";
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        //pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        sockfd = -1;
        return "Go fuck yourself drice!";
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        // if(error_revealer == NULL)
        // pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        sockfd = -1;
        return "Go fuck yourself fierce!";
    }

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
        return "Go fuck yourself once!";
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
         return "Go fuck yourself twice!";
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        //pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        sockfd = -1;
        return "Go fuck yourself drice!";
    }

    char recvBuffer[DEFAULT_MESSAGE_SIZE];
    bzero(recvBuffer, DEFAULT_MESSAGE_SIZE);

    if (recv(sockfd, recvBuffer, DEFAULT_MESSAGE_SIZE, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        // if(error_revealer == NULL)
        // pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        sockfd = -1;
        return "Go fuck yourself fierce!";
    }


    // char **user_recv_data = mx_strsplit(recvBuffer, '\n');
    return recvBuffer;
}

