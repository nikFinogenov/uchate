#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h> 
#include <time.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

// void pass_encrypt(char *password, char *key, char *encrypted) {
//     int i, keyLen, passLen;
//     keyLen = strlen(key);
//     passLen = strlen(password);

//     for (i = 0; i < passLen; i++) {
//         encrypted[i] = password[i] ^ key[i % keyLen];
//     }
//     encrypted[i] = '\0';
// }
void *mx_run_error_pop_up(void *vargp) {}
char **argv_ptr;
int sockfd = -1;
int mx_connect_to_server(int *sock) {
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
    
    if (connect(*sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        close(*sock);
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        char*  error_revealer = NULL;
        if (error_revealer == NULL)
            pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        return -1;
    }
    return 0;
}

void send_xyinya(void) {
    char* name = "Pidoras";
    char* surname = "Pidorovich";
    char* username = "p1d0r";
    char* password = "qwerty";

    if (sockfd == -1) mx_connect_to_server(&sockfd);
    
    char sendBuffer[1024];
    bzero(sendBuffer, 1024);
    sprintf(sendBuffer, "/user/add\n%s\n%s\n%s\n%s", username, password, name, surname);
    
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);
    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
        return;
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
         return;
    }
    
    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) < 0) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        sockfd = -1;
        return;
    }
}
int recive_xyinya(int id) {
if (sockfd == -1)  mx_connect_to_server(&sockfd);
    char sendBuffer[1025];
    bzero(sendBuffer, 1025);
    sprintf(sendBuffer, "/user/get\n%d", id);
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);
    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
        return 1;
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
        return 1;
    }

    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) == -1) {
        perror("ERROR writing to socket");
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        // if(error_revealer == NULL)
            // pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        sockfd = -1;
        return 1;
    }

    char recvBuffer[6000];
    bzero(recvBuffer, 6000);
    if (recv(sockfd, recvBuffer, 6000, 0) == 0) {
        perror("ERROR reading from socket");
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        // if(error_revealer == NULL)
            // pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
         sockfd = -1;
         return 1;
    }

    // char **user_recv_data = mx_strsplit(recvBuffer, '\n');
    write(1, recvBuffer, strlen(recvBuffer));
    return 0;
}
void update_xyunya(int id) {
    char* name = "Sidor";
    char* surname = "Pidor";
    char* desc = "qwerty";
    char* pass = "xyas";

    if (sockfd == -1) mx_connect_to_server(&sockfd);
    char sendBuffer[1025];
    bzero(sendBuffer, 1025);
    sprintf(sendBuffer, "/user/update\n%s\n%s\n%s\n%s\n%d", pass, name, surname, desc, id);
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);
    
    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
        // return 1;
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
        // return 1;
    }

    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) == -1) {
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        sockfd = -1;
        // return;
    }
}
void delete_xyinya(int id) {
if (sockfd == -1) mx_connect_to_server(&sockfd);
    char sendBuffer[1025];
    bzero(sendBuffer, 1025);
    sprintf(sendBuffer, "/user/delete\n%d",id);
    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &error, &len);
    
    if (retval != 0) {
        fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
        sockfd = -1;
        // return 1;
    }
    if (error != 0) {
        fprintf(stderr, "socket error: %s\n", strerror(error));
        sockfd = -1;
        // return 1;
    }

    if (send(sockfd, sendBuffer, strlen(sendBuffer), 0) == -1) {
        pthread_t thread_id;
        char *err_msg = "Connection lost\nTry again later";
        pthread_create(&thread_id, NULL, mx_run_error_pop_up, (void *)err_msg); 
        sockfd = -1;
        // return;
    }
}

int main(int argc, char *argv[]) {
    argv_ptr = argv;
    // send_xyinya();
    // update_xyunya(4);
    // close(sockfd);
    // if (mx_connect_to_server(&sockfd) != 0) {
    //     // Handle connection error
    //     return 1;
    // }
    // recive_xyinya(4);
    // close(sockfd);
    // if (mx_connect_to_server(&sockfd) != 0) {
    //     // Handle connection error
    //     return 1;
    // }
    delete_xyinya(4);
    // close(sockfd);
    // char *id = "1";
}
