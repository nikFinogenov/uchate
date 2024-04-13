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
#define MAX_LINE_LENGTH 70

#define MAX_JOKE_LENGTH 200

#include <gtk/gtk.h>
#include "../../libmx/inc/libmx.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <sqlite3.h>
#include <signal.h> 
#include <time.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <locale.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>

extern const char *jokes[];

typedef struct {
    char *username;
    char *password;
    bool button_recognize;
} t_user_data_s;

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
    int id;
    char* text;
    char* time;
    bool is_user;
    //reaction
} t_message_s;

typedef struct {
    GtkWidget *box;
    int index; 
} t_selected_s;

extern GtkWidget *signup_window;
extern GtkWidget *user_window;
extern GtkWidget *login_window;
extern char* default_img;
extern t_message_s** messages;
extern t_chatter_s* chatters;
extern t_user_s user;
extern t_user_data_s userdata;
extern t_selected_s selected_user;
// extern GtkWidget *scrollable_window2;
extern GtkWidget *chat_box;
extern GtkWidget *empty_chat;
extern GtkWidget *chats_box;
extern GtkWidget *settings_box;
extern GtkWidget *account_settings;
extern GtkWidget *user_info_box;
extern GtkWidget *scrollable_window;
extern GtkWidget *scrollable_window2;
extern GtkWidget *error_revealer;

extern int chatters_count;
extern int messages_count[MAX_CHATTERS];
extern char *login_info;
char **argv_ptr;
int sockfd;
int sock_for_chats;

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
void user_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter);
void message_populate_scrollable_window(GtkWidget *scrollable_window);
void message_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter);
//utils
GdkPixbuf *file_to_pixbuf(const gchar *filename);
void draw_image(GtkWidget *widget, cairo_t *cr, GdkPixbuf *data);
void set_widget_height(GtkWidget *widget, int height);
// void parse_json_buffer(const char *buffer, long buffer_size, t_user_data_s *userdata);
// void read_json_from_file(const char *filename, t_user_data_s *userdata);
void create_txt_with_data(const char *filename, const char *username, const char *password, bool button_recognize);
void read_txt_from_file(const char *filename, t_user_data_s *userdata);
void parse_txt_buffer(const char *buffer, t_user_data_s *userdata);
void update_user_line(const char *filename, const char *new_line);
void dimas_gandon(const char *filename);
void wrap_text(char *text);

// Server stuff
int connect_to_server(int *sock);
char **send_sign_up_data(char *first_name, char *last_name, char *username, char *password);
char **check_login_data(char *username, char* password);
char **get_chatter_data(char *username);
char **send_new_chat_data(char *username1, char* username2);
char **get_chats_data(char *username);
char **add_new_message(char *username_1, char *username_2, char* text, char* time);
char **update_user_info(char *changed_username, char * name, char *surname, char *desc, char *username);
char **chatter_delete(char *username_1, char *username_2);
char **delete_message_data(int id);
char **update_message_info(int id, char *new_text);
// char **get_user_data(char *username);


// Xyeta widgets, Podderjivau
GtkWidget *create_penis(void);
void fill_data(void);
bool is_chatters_empty(void);
bool is_messages_empty(void);
gboolean user_box_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void refresh_scrollable_window(GtkWidget *scrollable_window);
void refresh_scrollable_window2(GtkWidget *scrollable_window);
char* format_last_msg(char* text);
gboolean is_in_format(char* text, char* format);
void load_chats(char *username);
void reload_chats(char *username);
void start_chat_checker(char *username);
void stop_chat_checker(void);

// Dad jokes
char* get_random_joke();
void clear_all(void);

#endif
