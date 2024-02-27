#ifndef UCHAT_H
#define UCHAT_H

#define BG_COLOR "#171717"

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

#endif
