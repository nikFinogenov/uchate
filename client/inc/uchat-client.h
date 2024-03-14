#ifndef UCHAT_CLIENT_H
#define UCHAT_CLIENT_H

// Graphic elements
#define DARK_GRAY 0.23
#define LIGHT_GRAY 0.39

#define MAIN_WINDOW_HEIGHT 900
#define MAIN_WINDOW_WIDTH 1500

#include <gtk/gtk.h>
// #include <gdk-pixbuf-core.h>
#include <stdio.h>
#include <stdbool.h>
#include <../../libmx/inc/libmx.h>

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


// Xyeta widgets
GtkWidget *create_penis(void);


#endif
