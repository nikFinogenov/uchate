#ifndef UCHAT_CLIENT_H
#define UCHAT_CLIENT_H

// Graphic elements
#define DARK_GRAY 0.23
#define LIGHT_GRAY 0.39
#define LIGHTER_GRAY 0.75
#define NORM_CVET &(GdkRGBA){255 / 255.0,  182 / 255.0, 193 / 255.0, 1.0}

#define MAIN_WINDOW_HEIGHT 900
#define MAIN_WINDOW_WIDTH 1500

#define MAX_MESSAGES 1000 // Maximum number of messages
#define MAX_CHATTERS 50 // Maximum number of chatters
#define DEFAULT_MESSAGE_SIZE 1024

#include <gtk/gtk.h>
// #include <gdk-pixbuf-core.h>

#include "../../libmx/inc/libmx.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdbool.h>
#include "../../libmx/inc/libmx.h"
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

typedef struct {
    char* username;
    char* name;
    char* surname;
    char* desc;
    GdkPixbuf *avatar;
    // mx_read_photo_from_bd(t_user.id);
    // t_user.avatar = mx_get_pixbuf_with_size("client/img/tmp_avatar.png", 100, 100);
    // remove("client/img/tmp_avatar.png");
} t_user_s;

typedef struct {
    char* name;
    char* surname;
    char* username;
    char* lastmsg;
    GdkPixbuf *avatar;
} t_chatter_s;

typedef struct {
    char* text;
    bool is_user;
    //reaction
} t_message_s;

typedef struct {
    GtkWidget *box;
    int index; 
} t_selected_s;

extern char* default_img;
extern t_message_s** messages;
extern t_chatter_s* chatters;
extern t_user_s user;
extern t_selected_s selected_user;
// extern GtkWidget *scrollable_window2;
extern GtkWidget *chat_box;
extern GtkWidget *empty_chat;
extern GtkWidget *chats_box;
extern GtkWidget *settings_box;
extern GtkWidget *user_info_box;
extern GtkWidget *scrollable_window;
extern GtkWidget *scrollable_window2;

extern int chatters_count;
extern int messages_count[MAX_CHATTERS];

// typedef struct {
//     char* name;
//     char* surname;
//     GdkPixbuf *avatar;
// } t_chatter_s;


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
void draw_user_info_box(GtkWidget *user_info_box);

void user_populate_scrollable_window(GtkWidget *scrollable_window);
void message_populate_scrollable_window(GtkWidget *scrollable_window);

//utils
GdkPixbuf *file_to_pixbuf(const gchar *filename);
void draw_image(GtkWidget *widget, cairo_t *cr, GdkPixbuf *data);
void set_widget_height(GtkWidget *widget, int height);

// Server stuff
int connect_to_server(int *sock);
char **send_sign_up_data(char *first_name, char *last_name, char *username, char *password);
char **check_login_data(char *username, char* password);



// Xyeta widgets, Podderjivau
GtkWidget *create_penis(void);
void fill_data(void);
bool is_chatters_empty(void);
bool is_messages_empty(void);
gboolean user_box_clicked(GtkWidget *widget, GdkEvent *event, int index);
void refresh_scrollable_window(GtkWidget *scrollable_window);
void refresh_scrollable_window2(GtkWidget *scrollable_window);
char* format_last_msg(char* text);

#endif
