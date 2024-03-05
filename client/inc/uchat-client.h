#ifndef UCHAT_CLIENT_H
#define UCHAT_CLIENT_H

// Graphic elements
#define DARK_GRAY 0.23
#define LIGHT_GRAY 0.39

#define MAIN_WINDOW_HEIGHT 900
#define MAIN_WINDOW_WIDTH 1500

#include <gtk/gtk.h>
#include <stdio.h>

// Login side
void draw_login();
void show_login();
void go_to_signup();

// Sign up side
void draw_singup();
void show_signup();
void go_to_login();

// User window
void draw_user_window();
void show_user_window();

#endif
