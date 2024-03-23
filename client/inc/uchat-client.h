#ifndef UCHAT_CLIENT_H
#define UCHAT_CLIENT_H

// Graphic elements
#define DARK_GRAY 0.23
#define LIGHT_GRAY 0.39
#define LIGHTER_GRAY 0.75

#define MAIN_WINDOW_HEIGHT 900
#define MAIN_WINDOW_WIDTH 1500

#define MAX_MESSAGES 1000 // Maximum number of messages
#define MAX_CHATTERS 50 // Maximum number of chatters

#include <gtk/gtk.h>
// #include <gdk-pixbuf-core.h>
#include <stdio.h>
#include <stdbool.h>
#include <../../libmx/inc/libmx.h>

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

// Xyeta widgets
GtkWidget *create_penis(void);
void fill_data(void);
bool is_chatters_empty(void);
bool is_messages_empty(void);


#endif
