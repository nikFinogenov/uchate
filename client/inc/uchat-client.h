#ifndef UCHAT_CLIENT_H
#define UCHAT_CLIENT_H

// Graphic elements
#define DARK_GRAY 0.23
#define LIGHT_GRAY 0.39

#define MAIN_WINDOW_HEIGHT 900
#define MAIN_WINDOW_WIDTH 1500

#define DEFAULT_MESSAGE_SIZE 1024

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>
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

// Login side
// void draw_login(GtkWidget *window);
void draw_login(void);
void show_login(void);
void go_to_signup(void);

// Sign up side
void draw_singup(void);
void show_signup(void);
void go_to_login(void);

// User window
void draw_user_window(void);
void show_user_window(void);

void user_populate_scrollable_window(GtkWidget *scrollable_window);
void message_populate_scrollable_window(GtkWidget *scrollable_window);


// Server stuff
int connect_to_server(int *sock);
char **send_sign_up_data(char *username, char *password);
char *check_login_data(char *username, char* password);



// Xyeta widgets, Podderjivau
GtkWidget *create_penis(void);


#endif
