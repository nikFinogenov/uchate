#ifndef UCHAT_CLIENT_H
#define UCHAT_CLIENT_H

#define DARK_GRAY 0.23
#define LIGHT_GRAY 0.39
#define LIGHTER_GRAY 0.75
#define NORM_CVET &(GdkRGBA){255 / 255.0,  182 / 255.0, 193 / 255.0, 1.0}
#define RED_CVET &(GdkRGBA){255 / 255.0,  0, 0, 1.0}

#define MAIN_WINDOW_HEIGHT 900
#define MAIN_WINDOW_WIDTH 1500

#define MAX_MESSAGES 1000
#define MAX_CHATTERS 50
#define DEFAULT_MESSAGE_SIZE 1024
#define MAX_LINE_LENGTH 70
#define MAX_PATH_LENGTH 256
#define AVATAR_FOLDER "client/avatars/"
#define MAX_BUFFER_SIZE 1024

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
extern GdkPixbuf *temp_avatar;

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
    char* status;
    GdkPixbuf *avatar;
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
extern GtkWidget *chat_box;
extern GtkWidget *empty_chat;
extern GtkWidget *chats_box;
extern GtkWidget *settings_box;
extern GtkWidget *account_settings;
extern GtkWidget *user_info_box;
extern GtkWidget *scrollable_window;
extern GtkWidget *scrollable_window2;
GdkRGBA black;
GdkRGBA dark_purple;
GdkRGBA not_too_dark_purple;
GdkRGBA light_purple;
GdkRGBA vrode_norm_purple;

extern gint screen_width;
extern gint screen_height;

extern int chatters_count;
extern int messages_count[MAX_CHATTERS];
extern char *login_info;
char **argv_ptr;
int sockfd;
int sock_for_chats;

void draw_login(void);
void show_login(void);
void go_to_signup(void);

void draw_singup(void);
void show_signup(void);
void go_to_login(void);

void draw_user_window(void);
void show_user_window(void);
void draw_user_info_box(GtkWidget *user_info_box);
void draw_account_settings_box();
void draw_image_for_chat_box(GtkWidget *widget, cairo_t *cr, GdkPixbuf *data);

void user_populate_scrollable_window(GtkWidget *scrollable_window);
void user_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter);
void message_populate_scrollable_window(GtkWidget *scrollable_window);
void message_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter);
//utils
GdkPixbuf *file_to_pixbuf(const gchar *filename);
void draw_image(GtkWidget *widget, cairo_t *cr, GdkPixbuf *data);
void set_widget_height(GtkWidget *widget, int height);
void create_txt_with_data(const char *filename, const char *username, const char *password, bool button_recognize);
void read_txt_from_file(const char *filename, t_user_data_s *userdata);
void parse_txt_buffer(const char *buffer, t_user_data_s *userdata);
void update_user_line(const char *filename, const char *new_line);
void delete_more_than_three_lines(const char *filename);
void wrap_text(char *text);

//Server stuff
int connect_to_server(int *sock);
char **send_sign_up_data(char *first_name, char *last_name, char *username, char *password, char *status);
char **check_login_data(char *username, char* password);
char **get_chatter_data(char *username);
char **send_new_chat_data(char *username1, char* username2);
char **get_chats_data(char *username);
char **add_new_message(char *username_1, char *username_2, char* text, char* time, char* type);
char **update_user_info(char *changed_username, char * name, char *surname, char *desc, char *username);
char **chatter_delete(char *username_1, char *username_2);
char **delete_message_data(int id);
char **update_message_info(int id, char *new_text);
char **get_messages_data(char *username);
char **get_chat_messages(char *username_1, char *username_2);
char **get_mess_chat_amount(char *username_1, char *username_2);
char **update_user_info(char *changed_username, char *name, char *surname, char *desc, char *username);
void get_and_save_avatar_to_file(char *username);
void update_avatar(char *path, char *username);
char **update_user_status(char *status, char *username);
char **get_user_status(char *username);


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
void load_message(char *username);
void start_message_checker(char *username);
void stop_message_checker(void);
void clear_message(t_message_s *message);

char* get_random_joke();
void clear_all(void);

#endif
