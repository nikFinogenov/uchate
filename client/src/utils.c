#include "uchat-client.h"

GtkWidget *edit_pop_up;
pthread_t message_checker_thread;

typedef struct {
    int index;
    GtkWidget *mess_box;
} ButtonPressEventUserData;

t_selected_s selected_user = {
    .index = -1,
    .box = NULL
};

typedef struct {
    GtkWidget *edit_enter;
    int index_of_msg;
} InfoWidgets;

static void refresh_user_box() {
    gtk_container_foreach(GTK_CONTAINER(user_info_box), (GtkCallback)gtk_widget_destroy, NULL);

    draw_user_info_box(user_info_box);
    gtk_widget_show_all(user_info_box);
}

static void delete_message(GtkWidget *widget, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    if (index < 0 || index >= messages_count[selected_user.index]) {
        return;
    }
    char** response = delete_message_data(messages[selected_user.index][index].id);
    if(mx_strcmp((char *)response, "1") == 0) {
        g_print("Sadly, ended with error:(\n");
        return;
    }
    if(mx_strcmp((char *)response, "500") == 0) {
        g_print("Server offline\n");
        return;
    }
    free(messages[selected_user.index][index].text);
    free(messages[selected_user.index][index].time);
    for (int i = index; i < messages_count[selected_user.index] - 1; ++i) {
        messages[selected_user.index][i] = messages[selected_user.index][i + 1];
    }

    messages_count[selected_user.index]--;

    memset(&messages[selected_user.index][messages_count[selected_user.index]], 0, sizeof(t_message_s));

    refresh_scrollable_window2(scrollable_window2);
    update_reload_status("true", user.username, chatters[selected_user.index].username); 
}

static void edit_message_button_clicked (GtkWidget *widget, gpointer user_data) {
    InfoWidgets *data = (InfoWidgets *)user_data;
    const gchar *data_edit = gtk_entry_get_text(GTK_ENTRY(data->edit_enter));

    char *parsed_edit = (char*)data_edit;
    if(strlen(parsed_edit) > 512) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, 0, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "Error: Message is too long\n512 symbols max, buy premium for more");
        gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    wrap_text(parsed_edit);
    char** response = update_message_info(messages[selected_user.index][data->index_of_msg].id, parsed_edit);
    if(mx_strcmp((char *)response, "1") == 0) {
        g_print("Sadly, ended with error:(\n");
        return;
    }
    if(mx_strcmp((char *)response, "500") == 0) {
        g_print("Server offline\n");
        return;
    }
    
    if (strlen(parsed_edit) > DEFAULT_MESSAGE_SIZE || strlen(parsed_edit) == 0) NULL;
    else messages[selected_user.index][data->index_of_msg].text = mx_strdup(parsed_edit);
    gtk_widget_destroy(edit_pop_up);
    refresh_scrollable_window2(scrollable_window2);
    update_reload_status("true", user.username, chatters[selected_user.index].username);  
}

static void edit_message(GtkWidget *widget, gpointer data) {
    int index = GPOINTER_TO_INT(data);

    if (index < 0 || index >= messages_count[selected_user.index]) {
       
        return;
    }

    
    g_print("Edit message clicked\n");
    edit_pop_up = gtk_dialog_new_with_buttons("Edit Message", GTK_WINDOW(user_window),
                                                    GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    NULL, GTK_RESPONSE_NONE, NULL);

    gtk_window_set_default_size(GTK_WINDOW(edit_pop_up), 500, 150);
    gtk_widget_override_background_color(edit_pop_up, GTK_STATE_FLAG_NORMAL, &black);
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(edit_pop_up));

    GtkWidget *edit_entry =  gtk_entry_new();

    gtk_entry_set_placeholder_text(GTK_ENTRY(edit_entry), "Enter edited message...");
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_entry_set_text(GTK_ENTRY(edit_entry), messages[selected_user.index][index].text);
    gtk_box_pack_start(GTK_BOX(box), edit_entry, TRUE, FALSE, 0);
    

    GtkWidget *edit_button = gtk_button_new_with_label("Edit");
    gtk_box_pack_start(GTK_BOX(box), edit_button, FALSE, FALSE, 0);

    InfoWidgets *entry = g_new(InfoWidgets, 1);
    entry->edit_enter = edit_entry;
    entry->index_of_msg = index;

    g_signal_connect(edit_button, "clicked", G_CALLBACK(edit_message_button_clicked), entry);

    gtk_widget_set_margin_top(edit_button, 10);
    gtk_container_add(GTK_CONTAINER(content_area), box);
    gtk_widget_set_size_request(box, 400, 100);

    gtk_widget_set_halign(content_area, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(content_area, GTK_ALIGN_CENTER);

    gtk_widget_show_all(edit_pop_up);

    g_signal_connect_swapped(edit_pop_up, "response", G_CALLBACK(gtk_widget_destroy), edit_pop_up);
}

static void delete_chatter(GtkWidget *widget, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    if (index < 0 || index >= chatters_count) {
        return;
    }

    char **response = chatter_delete(user.username, chatters[index].username);
    if(mx_strcmp((char *)response, "1") == 0) {
        g_print("Sadly, ended with error:(\n");
        return;
    }
    if(mx_strcmp((char *)response, "500") == 0) {
        g_print("Server offline\n");
        return;
    }

    free(chatters[index].name);
    free(chatters[index].surname);
    free(chatters[index].username);
    free(chatters[index].lastmsg);
    if (chatters[index].avatar != NULL) {
        g_object_unref(chatters[index].avatar);
    }
    for(int i = 0; i < messages_count[index]; i++) {
        free(messages[i][index].text);
        free(messages[i][index].time);
    }
    messages_count[index] = 0;
    
    for (int i = index; i < chatters_count - 1; ++i) {
        chatters[i] = chatters[i + 1];
        messages[i] = messages[i + 1];
        messages_count[i] = messages_count[i + 1];
    }
    chatters_count--;
    memset(&chatters[chatters_count], 0, sizeof(t_chatter_s));

    if (selected_user.index == index) {
        selected_user.box = NULL;
        selected_user.index = -1;
        gtk_widget_hide(chat_box);
        gtk_widget_show(empty_chat);
    }
    else if(selected_user.index > index){
        selected_user.index--;
    }

    if (strcmp((char *)response, "1") == 0) {
        g_print("There was a problem deleting a user...\n");
    } else g_print("pracue\n");
    
    refresh_scrollable_window(scrollable_window);
    if(chatters_count == 0) {
        gtk_widget_show(add_new_chat_when_no_chats);
        gtk_label_set_text(GTK_LABEL(empty_chat), "[ Add your first chat! ]");
    }
}

static GdkPixbuf *resize_img(GdkPixbuf *pixbuf, int w, int h) {
    int width = gdk_pixbuf_get_width(GDK_PIXBUF(pixbuf));
    int height = gdk_pixbuf_get_height(GDK_PIXBUF(pixbuf));

    if (width <= w && height <= h) return pixbuf;
    else {
        int source_width = width;
        int source_height = height;
        if (width > height) {
            width = w;
            height = (w * source_height) / source_width;
            if (height > h) {
                source_width = width;
                source_height = height;
                height = h;
                width = (h * source_width) / source_height;
            }
        }
        else {
            height = h;
            width = (h * source_width) / source_height;
            if (width > w) {
                source_width = width;
                source_height = height;
                width = w;
                height = (w * source_height) / source_width;
            }
        }
    }

    GdkPixbuf *result = gdk_pixbuf_scale_simple(GDK_PIXBUF(pixbuf), 
        width, height, GDK_INTERP_BILINEAR);
    g_object_unref(pixbuf);
    return result;
}

static void create_tools_menu(GdkEvent *event, int index) {
    GtkWidget *menu;
    GtkWidget *menu_item;
    
    menu = gtk_menu_new();

    menu_item = gtk_menu_item_new_with_label("Edit");
    g_signal_connect(menu_item, "activate", G_CALLBACK(edit_message), GINT_TO_POINTER(index));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    menu_item = gtk_menu_item_new_with_label("Delete");
    g_signal_connect(menu_item, "activate", G_CALLBACK(delete_message), GINT_TO_POINTER(index));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
    gtk_widget_show_all(menu);

    gtk_menu_popup_at_pointer(GTK_MENU(menu), event);
}

static void create_chatter_menu(GdkEvent *event, int index) {
    GtkWidget *menu;
    GtkWidget *menu_item;
    
    menu = gtk_menu_new();

    menu_item = gtk_menu_item_new_with_label("Delete");
    g_signal_connect(menu_item, "activate", G_CALLBACK(delete_chatter), GINT_TO_POINTER(index));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    gtk_widget_show_all(menu);

    gtk_menu_popup_at_pointer(GTK_MENU(menu), event);
}

static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    GtkWidget *event_box = GTK_WIDGET(widget);
    GtkWidget *mess_box = gtk_bin_get_child(GTK_BIN(event_box));
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_SECONDARY) {
        GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(widget)));
        if (parent_window != NULL) {
            gint x, y;
            gtk_window_get_position(parent_window, &x, &y);
            GtkAllocation allocation;
            gtk_widget_get_allocation(GTK_WIDGET(mess_box), &allocation);
            create_tools_menu((GdkEvent *)event, index);
            return TRUE;
        }
    }
    return FALSE;
}

static GtkWidget *create_user_box(char* tag, char* last_msg, char* input_image_file) {
    GtkWidget *event_box = gtk_event_box_new();
    gtk_widget_set_name(event_box, "clickable-user-box");

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(input_image_file, NULL);
    GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(pixbuf);
    prev_pixbuf = resize_img(prev_pixbuf, 64, 64);
    set_widget_height(box, 70);
    
    GtkWidget *image = gtk_drawing_area_new();
    gtk_widget_set_halign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_valign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(image), gdk_pixbuf_get_width(GDK_PIXBUF(prev_pixbuf)) + 3, gdk_pixbuf_get_height(GDK_PIXBUF(prev_pixbuf)) + 3);
    g_signal_connect(G_OBJECT(image), "draw", G_CALLBACK(draw_image), prev_pixbuf);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 15);

    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    GtkWidget *user_tag = gtk_label_new(tag);
    gtk_widget_set_name(user_tag, "user-tag");

    GtkWidget *last_mess = gtk_label_new(last_msg);
    gtk_widget_set_name(last_mess, "last-message");

    gtk_box_pack_start(GTK_BOX(info_box), user_tag, FALSE, FALSE, 7);
    gtk_box_pack_start(GTK_BOX(info_box), last_mess, FALSE, FALSE, 0);
    gtk_widget_set_halign(GTK_WIDGET(user_tag), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(last_mess), GTK_ALIGN_START);

    gtk_box_pack_start(GTK_BOX(box), info_box, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(event_box), box);
    return event_box;
}

static GtkWidget *create_message_box(t_message_s *message) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(box, 10);

    GtkWidget *message_box = gtk_grid_new(); 
    gtk_grid_set_row_homogeneous(GTK_GRID(message_box), FALSE); 
    gtk_grid_set_column_homogeneous(GTK_GRID(message_box), FALSE); 
    gtk_widget_set_name(message_box, "message-text");

    if(mx_strcmp(message->text, ":ucode") == 0) {
        mx_b64_decodef("code.txt", "server/source/tmp.png");
        GdkPixbuf *ucode_pixbuf = gdk_pixbuf_new_from_file("server/source/tmp.png", NULL);
        if (ucode_pixbuf != NULL) {
            GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(ucode_pixbuf);
            prev_pixbuf = resize_img(prev_pixbuf, 128, 128);
            GtkWidget *ucode_image = gtk_image_new_from_pixbuf(prev_pixbuf);
            gtk_grid_attach(GTK_GRID(message_box), ucode_image, 0, 0, 1, 1); 

            g_object_unref(ucode_pixbuf);
        } else {
            GtkWidget *error_label = gtk_label_new("Error: Failed to load image");
            gtk_grid_attach(GTK_GRID(message_box), error_label, 0, 0, 1, 1); 
        }
        remove("server/source/tmp.png");
    }
    else if(mx_strcmp(message->text, ":like") == 0) { 
        GdkPixbuf *ucode_pixbuf = gdk_pixbuf_new_from_file("client/img/thumbs.png", NULL);
        if (ucode_pixbuf != NULL) {
            GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(ucode_pixbuf);
            prev_pixbuf = resize_img(prev_pixbuf, 32, 32);
            GtkWidget *ucode_image = gtk_image_new_from_pixbuf(prev_pixbuf);
            gtk_grid_attach(GTK_GRID(message_box), ucode_image, 0, 0, 1, 1); 

            g_object_unref(ucode_pixbuf);
        } 
    }
    else if(mx_strcmp(message->text, ":love") == 0) {
        GdkPixbuf *ucode_pixbuf = gdk_pixbuf_new_from_file("client/img/pink.png", NULL);
        if (ucode_pixbuf != NULL) {
            GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(ucode_pixbuf);
            prev_pixbuf = resize_img(prev_pixbuf, 32, 32);
            GtkWidget *ucode_image = gtk_image_new_from_pixbuf(prev_pixbuf);
            gtk_grid_attach(GTK_GRID(message_box), ucode_image, 0, 0, 1, 1); 

            g_object_unref(ucode_pixbuf);
        } 
     }
    else if(mx_strcmp(message->text, ":stone") == 0) {
        GdkPixbuf *ucode_pixbuf = gdk_pixbuf_new_from_file("client/img/moai.png", NULL);
        if (ucode_pixbuf != NULL) {
            GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(ucode_pixbuf);
            prev_pixbuf = resize_img(prev_pixbuf, 32, 32);
            GtkWidget *ucode_image = gtk_image_new_from_pixbuf(prev_pixbuf);
            gtk_grid_attach(GTK_GRID(message_box), ucode_image, 0, 0, 1, 1); 

            g_object_unref(ucode_pixbuf);
        } 
     }
    else if(mx_strcmp(message->text, ":lol") == 0) {
        GdkPixbuf *ucode_pixbuf = gdk_pixbuf_new_from_file("client/img/rolling.png", NULL);
        if (ucode_pixbuf != NULL) {
            GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(ucode_pixbuf);
            prev_pixbuf = resize_img(prev_pixbuf, 32, 32);
            GtkWidget *ucode_image = gtk_image_new_from_pixbuf(prev_pixbuf);
            gtk_grid_attach(GTK_GRID(message_box), ucode_image, 0, 0, 1, 1); 

            g_object_unref(ucode_pixbuf);
        } 
     }
    else if(mx_strcmp(message->text, ":sad") == 0) { 
        GdkPixbuf *ucode_pixbuf = gdk_pixbuf_new_from_file("client/img/sad.png", NULL);
        if (ucode_pixbuf != NULL) {
            GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(ucode_pixbuf);
            prev_pixbuf = resize_img(prev_pixbuf, 32, 32);
            GtkWidget *ucode_image = gtk_image_new_from_pixbuf(prev_pixbuf);
            gtk_grid_attach(GTK_GRID(message_box), ucode_image, 0, 0, 1, 1); 

            g_object_unref(ucode_pixbuf);
        } 
    }
    else {
        GtkWidget *text_label = gtk_label_new(message->text);
        gtk_grid_attach(GTK_GRID(message_box), text_label, 0, 0, 1, 1);
    }
    GtkWidget *time_label = gtk_label_new(message->time);
    gtk_widget_set_name(time_label, "message-time");
    gtk_widget_set_halign(time_label, GTK_ALIGN_END);
    gtk_widget_set_valign(time_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(message_box), time_label, 1, 1, 1, 1);

    if(message->is_user) {
        gtk_box_pack_end(GTK_BOX(box), message_box, FALSE, TRUE, 0);
    }
    else gtk_box_pack_start(GTK_BOX(box), message_box, FALSE, TRUE, 0);
    
    return box;
}

gboolean user_box_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    int index = GPOINTER_TO_INT(user_data);
    if (event->type == GDK_BUTTON_PRESS) {
        if (event->button == GDK_BUTTON_SECONDARY) {
            create_chatter_menu((GdkEvent *)event, index);
            return TRUE;
        } else if (event->button == GDK_BUTTON_PRIMARY) {
            if (selected_user.box != NULL && selected_user.index != index) {
                gtk_widget_override_background_color(selected_user.box, GTK_STATE_FLAG_NORMAL, &ny_takoy_purple); 
            }
            gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &vrode_norm_purple); 
            selected_user.box = widget;
            selected_user.index = index;

            gtk_widget_hide(empty_chat);
            refresh_user_box();
            refresh_scrollable_window2(scrollable_window2);
            gtk_widget_show(scrollable_window2);

            gtk_widget_show_all(chat_box);

            return FALSE;
        }
    }
    return FALSE;
}

char* get_random_joke() {
    srand(time(NULL));
    int random_index = rand() % 30;

    return strdup(jokes[random_index]);
}

GdkPixbuf *file_to_pixbuf(const gchar *filename) {
    GdkPixbuf *pixbuf;
    GError *error = NULL;
    pixbuf = gdk_pixbuf_new_from_file(filename, &error);
    
    if (!pixbuf) {
        fprintf(stderr, "%s\n", error->message);
        g_error_free(error);
    }
    
    return pixbuf;
}

void draw_image(GtkWidget *widget, cairo_t *cr, GdkPixbuf *data) {
    GdkPixbuf *pixbuf = (GdkPixbuf *)data;
    gint width = gdk_pixbuf_get_width(pixbuf);
    gint height = gdk_pixbuf_get_height(pixbuf);
    gint radius = MIN(width, height) / 2;

    gint x = (gtk_widget_get_allocated_width(widget) - width) / 2;
    gint y = (gtk_widget_get_allocated_height(widget) - height) / 2;

    cairo_arc(cr, x + width / 2, y + height / 2, radius, 0, 2 * G_PI);
    cairo_clip(cr);

    gdk_cairo_set_source_pixbuf(cr, pixbuf, x, y);
    cairo_paint(cr);
}

void draw_image_for_chat_box(GtkWidget *widget, cairo_t *cr, GdkPixbuf *data){
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    int img_w = alloc.width;
    int img_h = alloc.height;

    double corner_radius = 12.0;
    double degrees = G_PI / 180.0;
    double radius = corner_radius;

    cairo_move_to(cr, radius, 0);
    cairo_arc(cr, img_w - radius, radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc(cr, img_w - radius, img_h - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc(cr, radius, img_h - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, radius, radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(cr);

    gdk_cairo_set_source_pixbuf(cr, data, 0, 0);
    cairo_fill(cr);
}

void set_widget_height(GtkWidget *widget, int height) {
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    alloc.height = height;
    gtk_widget_set_size_request(widget, alloc.width, height);
}
void user_populate_scrollable_window(GtkWidget *scrollable_window) {
    GtkWidget *user_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_add(GTK_CONTAINER(scrollable_window), user_list);
    char *avatar_path;
    
    if (chatters != NULL) {
        for (int i = 0; i < chatters_count; i++) {
            avatar_path = (char *)malloc(strlen(AVATAR_FOLDER) + strlen(chatters[i].username) + strlen("_avatar.png") + 1);
            get_and_save_avatar_to_file(chatters[i].username);
            sprintf(avatar_path, "%s%s_avatar.png", AVATAR_FOLDER, chatters[i].username);
            GtkWidget *user_box = create_user_box(chatters[i].username, chatters[i].lastmsg, avatar_path);
            gtk_widget_set_name(user_box, "user-box");
            if(i == selected_user.index) {
                selected_user.box = user_box;
                gtk_widget_override_background_color(user_box, GTK_STATE_FLAG_NORMAL, &vrode_norm_purple); 
            }
            g_signal_connect(user_box, "button-press-event", G_CALLBACK(user_box_clicked), GINT_TO_POINTER(i));

            gtk_box_pack_start(GTK_BOX(user_list), user_box, FALSE, FALSE, 0);
            remove(avatar_path);
            free(avatar_path);
        }
    }
}

void user_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter) {
    GtkWidget *user_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_container_add(GTK_CONTAINER(scrollable_window), user_list);
    char *avatar_path;
    if (chatters != NULL) {
        for (int i = 0; i < chatters_count; i++) {
            if(is_in_format(chatters[i].username, filter) || is_in_format(chatters[i].name, filter)) {
                avatar_path = (char *)malloc(strlen(AVATAR_FOLDER) + strlen(chatters[i].username) + strlen("_avatar.png") + 1);
                get_and_save_avatar_to_file(chatters[i].username);
                sprintf(avatar_path, "%s%s_avatar.png", AVATAR_FOLDER, chatters[i].username);
                GtkWidget *user_box = create_user_box(chatters[i].username, chatters[i].lastmsg, avatar_path);
                gtk_widget_set_name(user_box, "user-box");
                if(i == selected_user.index) {
                    selected_user.box = user_box;
                    gtk_widget_override_background_color(user_box, GTK_STATE_FLAG_NORMAL, &vrode_norm_purple); 
                }
                g_signal_connect(user_box, "button-press-event", G_CALLBACK(user_box_clicked), GINT_TO_POINTER(i));

                gtk_box_pack_start(GTK_BOX(user_list), user_box, FALSE, FALSE, 0);
                remove(avatar_path);
                free(avatar_path);
            }
        }
    }
}

void message_populate_scrollable_window(GtkWidget *scrollable_window) {
    GtkWidget *mess_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), mess_list);

    if (messages != NULL && selected_user.index != -1) {
        for (int i = 0; i < messages_count[selected_user.index]; i++) {
            GtkWidget *event_box = gtk_event_box_new();
            gtk_container_add(GTK_CONTAINER(mess_list), event_box);
            GtkWidget *mess_box = create_message_box(&messages[selected_user.index][i]);
            gtk_container_add(GTK_CONTAINER(event_box), mess_box);

            if(messages[selected_user.index][i].is_user) g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_button_press), GINT_TO_POINTER(i));
        }
    }
}

void message_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter) {
    GtkWidget *mess_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), mess_list);

    if (messages != NULL) {
        for (int i = 0; i < messages_count[selected_user.index]; i++) {
            if(is_in_format(messages[selected_user.index][i].text, filter)){
                GtkWidget *event_box = gtk_event_box_new();
                gtk_container_add(GTK_CONTAINER(mess_list), event_box);
                GtkWidget *mess_box = create_message_box(&messages[selected_user.index][i]);
                gtk_container_add(GTK_CONTAINER(event_box), mess_box);
                if(messages[selected_user.index][i].is_user) g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_button_press), GINT_TO_POINTER(i));
            }
        }
    }
}

char* format_last_msg(char* text) {
        int len = strlen(text);
    int max_length = 25;

    char* formatted_text = (char*)malloc((max_length + 4) * sizeof(char));
    if (formatted_text == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    strncpy(formatted_text, text, max_length);

    formatted_text[max_length] = '\0';

    if (len > max_length) {
        strcat(formatted_text, "...");
    }

    return formatted_text;
}

gboolean is_in_format(char* text, char* format) {
    size_t text_len = strlen(text);
    size_t format_len = strlen(format);
    
    if (format_len > text_len) {
        return FALSE;
    }
    

    for (size_t i = 0; i <= text_len - format_len; ++i) {
        if (strncmp(text + i, format, format_len) == 0) {
            return TRUE;
        }
    }
    
    return FALSE;
}

void create_txt_with_data(const char *filename, const char *username, const char *password, bool button_recognize) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Unable to open file %s for writing.\n", filename);
        return;
    }

    fprintf(file, "%s\n%s\n%s\n", username, password, (button_recognize ? "button_recognize" : ""));
    fclose(file);
}

void read_txt_from_file(const char *filename, t_user_data_s *userdata) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file %s for reading.\n", filename);
        return;
    }

    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    userdata->username = strdup(buffer);
    if (!userdata->username) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        return;
    }

    fgets(buffer, sizeof(buffer), file);
    userdata->password = strdup(buffer);
    if (!userdata->password) {
        printf("Error: Memory allocation failed.\n");
        fclose(file);
        free(userdata->username);
        return;
    }

    fgets(buffer, sizeof(buffer), file);
    userdata->button_recognize = (strcmp(buffer, "button_recognize\n") == 0);
    fclose(file);
}

void update_user_line(const char *filename, const char *new_line) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        perror("Ошибка при открытии файла");
        return;
    }

    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) == NULL) {
        perror("Ошибка при чтении файла");
        fclose(file);
        return;
    }

    long offset = ftell(file);
    FILE *temp_file = tmpfile();
    if (temp_file == NULL) {
        perror("Ошибка при создании временного файла");
        fclose(file);
        return;
    }
    int line_count = 1;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        if (line_count <= 3) {
            fputs(buffer, temp_file);
        }
        line_count++;
    }

    rewind(file);

    fputs(new_line, file);
    fputs("\n", file);

    rewind(temp_file);
    while (fgets(buffer, sizeof(buffer), temp_file) != NULL) {
        fputs(buffer, file);
    }

    fclose(file);
    fclose(temp_file);
}

void delete_more_than_three_lines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Не удалось открыть файл.\n");
        return;
    }

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL) {
        printf("Не удалось создать временный файл.\n");
        fclose(file);
        return;
    }

    char line[256];
    int lineCount = 0;
    while (fgets(line, sizeof(line), file) && lineCount < 3) {
        fputs(line, tempFile);
        lineCount++;
    }

    fclose(file);
    remove(filename);

    rename("temp.txt", filename);

    fclose(tempFile);
}

void parse_txt_buffer(const char *buffer, t_user_data_s *userdata) {
    char *token;
    int count = 0;
    char *lines[3];

    token = strtok((char *)buffer, "\n");
    while (token != NULL && count < 3) {
        lines[count++] = token;
        token = strtok(NULL, "\n");
    }

    if (count != 3) {
        fprintf(stderr, "Error: Invalid format in txt buffer\n");
        return;
    }

    strncpy(userdata->username, lines[0], sizeof((char *)userdata->username));
    strncpy(userdata->password, lines[1], sizeof((char *)userdata->password));
    if (strcmp(lines[2], "button_recognize") == 0) {
        userdata->button_recognize = true;
    } else {
        userdata->button_recognize = false;
    }
}

int server_chats_quantity(char *username) {
    char **response = get_chats_data(username);
    if (strcmp((char *)response, "1") == 0) {
        g_print("Sadly, ended with error:(\n");
        return chatters_count;
    }
    if (strcmp((char *)response, "500") == 0) {
        g_print("Server offline\n");
        return -1;
    }
    char **tokens = mx_strsplit((char *)response, '\n');
    return(mx_get_length(tokens));
}

void load_chats(char *username) {
    char **response = get_chats_data(username);
    if (strcmp((char *)response, "1") == 0) {
        g_print("Sadly, ended with error:(\n");
        return;
    }
    if (strcmp((char *)response, "500") == 0) {
        g_print("Server offline\n");
        return;
    }
    char **tokens = mx_strsplit((char *)response, '\n');
    for(int i = 0; i < mx_get_length(tokens); i++) {
        char **response2 = get_chatter_data(tokens[i]);
        if (strcmp((char *)response2, "1") == 0) {
            g_print("couldn't be found1\n");
            return;
        }
        if (strcmp((char *)response2, "500") == 0) {
            g_print("Server offline\n");
            return;
        }
        char *token2 = strtok((char *)response2, "\n");
        char *username = strdup(token2);
        token2 = strtok(NULL, "\n");
        char *name = strdup(token2);
        token2 = strtok(NULL, "\n");
        char *surname = strdup(token2);
        add_chatter(name, surname, username, "No messages yet", NULL);
    }
}

void load_message(char *username) {
    char **response = get_chats_data(username);
    if (strcmp((char *)response, "1") == 0) {
        g_print("Sadly, ended with error:(\n");
        return;
    }
    if (strcmp((char *)response, "500") == 0) {
        g_print("Server offline\n");
        return;
    }
    char **tokens = mx_strsplit((char *)response, '\n');
    for(int i = 0; i < mx_get_length(tokens); i++) {
        char **response2 = get_chat_messages(user.username, tokens[i]);
        if (strcmp((char *)response2, "1") == 0) {
            g_print("Sadly, ended with error:(\n");
            return;
        }
        if (strcmp((char *)response2, "500") == 0) {
            g_print("Server offline\n");
            return;
        }
        if (strcmp((char *)response2, "0") == 0) {
            g_print("0 messages\n");
            continue;
        }
        char **tokens2 = mx_strsplit((char *)response2, '\n');
        int mess_count_local = mx_get_length(tokens2) / 5;
        for(int j = 0; j < mess_count_local; j++) {
            char *id = tokens2[j * 5];
            char *chat_id = tokens2[j * 5 + 1];
            char *text = tokens2[j * 5 + 2];
            char *type = tokens2[j * 5 + 3];
            char *time = tokens2[j * 5 + 4];
            add_message(mx_atoi(id), i, text, time, (mx_strcmp(user.username, type) == 0) ? true : false);
        }
        chatters[i].lastmsg = mx_strdup(messages[i][mess_count_local - 1].text);
    }
}

void load_chatter_message(char *chattername) {
        char **response2 = get_chat_messages(user.username, chattername);
        if (strcmp((char *)response2, "1") == 0) {
            g_print("Sadly, ended with error:(\n");
            return;
        }
        if (strcmp((char *)response2, "500") == 0) {
            g_print("Server offline\n");
            return;
        }
        if (strcmp((char *)response2, "0") == 0) {
            return;
        }
        char **tokens2 = mx_strsplit((char *)response2, '\n');
        int mess_count_ahuet = mx_get_length(tokens2) / 5;
        char *token2 = strtok((char *)response2, "\n");
        for(int j = 0; j < mx_get_length(tokens2); j++) {
            char *id = tokens2[j];
            j++;
            char *chat_id = tokens2[j];
            j++;
            char *text = tokens2[j];
            j++;
            char *type = tokens2[j];
            j++;
            char *time = tokens2[j];
            add_message(mx_atoi(id), selected_user.index, text, time, (mx_strcmp(user.username, type) == 0) ? true : false);
        }
}

void reload_chats(char *username) {
    char **response = get_chats_data(username);
    if (strcmp((char *)response, "1") == 0) {
        g_print("Sadly, ended with error:(\n");
        return;
    }
    if (strcmp((char *)response, "500") == 0) {
        g_print("Server offline\n");
        return;
    }
    for (int i = 0; i < chatters_count; i++) {
        free(chatters[i].name);
        free(chatters[i].surname);
        free(chatters[i].username);
        free(chatters[i].lastmsg);
        if (chatters[i].avatar != NULL) {
            g_object_unref(chatters[i].avatar);
        }
    }
    free(chatters);
    chatters = NULL;
    chatters_count = 0;

    char **tokens = mx_strsplit((char *)response, '\n');
    for(int i = 0; i < mx_get_length(tokens); i++) {
        char **response2 = get_chatter_data(tokens[i]);
        if (strcmp((char *)response2, "1") == 0) {
            g_print("Sadly, ended with error:(\n");
            return;
        }
        if (strcmp((char *)response2, "500") == 0) {
            g_print("Server offline\n");
            return;
        }
        char *token2 = strtok((char *)response2, "\n");
        char *username = strdup(token2);
        token2 = strtok(NULL, "\n");
        char *name = strdup(token2);
        token2 = strtok(NULL, "\n");
        char *surname = strdup(token2);
        char *avatar_path = (char *)malloc(strlen(AVATAR_FOLDER) + strlen(username) + strlen("_avatar.png") + 1);
        get_and_save_avatar_to_file(username);
        sprintf(avatar_path, "%s%s_avatar.png", AVATAR_FOLDER, username);
        GdkPixbuf *avatar_reload = gdk_pixbuf_new_from_file(avatar_path, NULL);
        GdkPixbuf *pixbuf = gdk_pixbuf_scale_simple(avatar_reload, 64, 64, GDK_INTERP_BILINEAR);
        t_chatter_s new_chatter = {
            .name = mx_strdup(name),
            .surname = mx_strdup(surname),
            .username = mx_strdup(username),
            .lastmsg = mx_strdup("No messages yet"),
            .avatar = gdk_pixbuf_copy(pixbuf)
        };
        remove(avatar_path);
        free(avatar_path);
        chatters = realloc(chatters, sizeof(t_chatter_s) * (chatters_count + 1));
        chatters[chatters_count] = new_chatter;
        chatters_count++;
    }
}

void reload_messages(char *username) {
    char **response = get_chats_data(username);
    if (strcmp((char *)response, "1") == 0) {
        g_print("Sadly, ended with error:(\n");
        return;
    }
    if (strcmp((char *)response, "500") == 0) {
        g_print("Server offline\n");
        return;
    }
    for (int i = 0; i < MAX_CHATTERS; ++i) {
        for(int j = 0; j < messages_count[i]; j++) clear_message(&messages[i][j]);
    }

    for(int i = 0; i < MAX_CHATTERS; i++) messages_count[i] = 0;
    free(messages);
    messages = NULL;
    messages = malloc(MAX_CHATTERS * sizeof(t_message_s *));
    for (int i = 0; i < MAX_CHATTERS; i++) {
        messages[i] = malloc(MAX_MESSAGES * sizeof(t_message_s));
        messages_count[i] = 0;
    }

    char **tokens = mx_strsplit((char *)response, '\n');
    for (int i = 0; i < mx_get_length(tokens); i++) {

        char **response2 = get_chat_messages(username, tokens[i]);
        if (strcmp((char *)response2, "1") == 0) {
            g_print("Sadly, ended with error:(\n");
            continue;
        }
        if (strcmp((char *)response2, "500") == 0) {
            g_print("Server offline\n");
            return;
        }

        char **tokens2 = mx_strsplit((char *)response2, '\n');
        int mess_count_ahuet = mx_get_length(tokens2) / 5;

        char *token2 = strtok((char *)response2, "\n");

        for (int j = 0; j < mess_count_ahuet; j++) {
            char *id = token2;
            token2 = strtok(NULL, "\n");
            char *chat_id = token2;
            token2 = strtok(NULL, "\n");
            char *text = token2;
            token2 = strtok(NULL, "\n");
            char *type = token2;
            token2 = strtok(NULL, "\n");
            char *time = token2;
            token2 = strtok(NULL, "\n");
            t_message_s new_message = {
                .id = atoi(id),
                .text = strdup(text),
                .time = strdup(time),
                .is_user = (strcmp(username, type) == 0) ? true : false
            };
            messages[i][j] = new_message;
            messages_count[i]++;
        }

    }
}

int server_messages_quantity(char *username) {
    int total_local_messages_amount = 0;
    int total_messages = 0;
    for(int i = 0; i < MAX_CHATTERS; i++) {
        total_local_messages_amount += messages_count[i];
    }
    char **response = get_chats_data(username);
    if (strcmp((char *)response, "1") == 0) {
        g_print("Sadly, ended with error:(\n");
        return total_local_messages_amount;
    }
    if (strcmp((char *)response, "500") == 0) {
        g_print("Server offline\n");
        return total_local_messages_amount;
    }

    char **tokens = mx_strsplit((char *)response, '\n');
    for (int i = 0; i < mx_get_length(tokens); i++) {
        char **response2 = get_mess_chat_amount(username, tokens[i]);
        if (strcmp((char *)response2, "1") == 0) {
            g_print("Sadly, ended with error:(\n");
            return total_local_messages_amount;
        }
        if (strcmp((char *)response2, "500") == 0) {
            g_print("Server offline\n");
            return total_local_messages_amount;
        }
        total_messages += mx_atoi((char *)response2);
    }

    return total_messages;
}

gboolean check_messages_async(gpointer data) {
    char *username = (char *)data;
    
    int server_chats_amount = server_chats_quantity(username);
    if (server_chats_amount == -1) {
        return TRUE; 
    }else if (server_chats_amount != chatters_count) {
            clear_chats();
            chatters_count = 0; 
            if (chatters != NULL) {
                free(chatters);
                chatters = NULL;
            }
            chatters = malloc(MAX_CHATTERS * sizeof(t_chatter_s));
            for(int i = 0; i < MAX_CHATTERS; i++) {
                chatters[i].username = NULL;
            }
            load_chats(username);
            gtk_widget_hide(add_new_chat_when_no_chats);
            refresh_scrollable_window(scrollable_window);
    }
    if(selected_user.index != -1) {
            char **reload_response = get_reload_status(user.username, chatters[selected_user.index].username);
            char *tok = strtok((char *)reload_response, "\n");
            char *reload = mx_strdup(tok);
            int server_message_amount = mx_atoi((char *)get_mess_chat_amount(username, chatters[selected_user.index].username));
            int server_message_lastid = mx_atoi((char *)get_mess_chat_last_id(username, chatters[selected_user.index].username));
            int total_local_messages_amount = 0;
            for(int i = 0; i < MAX_CHATTERS; i++) {
                total_local_messages_amount += messages_count[i];
            }
            if(mx_strcmp(reload, "true") == 0) {
                for(int i = 0; i < chatters_count; i++) {
                    for(int j = 0; j < messages_count[i]; j++) {
                        free(messages[i][j].text);
                        free(messages[i][j].time);
                    }
                    free(messages[i]);
                }
                free(messages);
                messages = NULL;
                messages = malloc(MAX_CHATTERS * sizeof(t_message_s*));
                if (messages == NULL) {
                    fprintf(stderr, "Error: Memory allocation failed for messages array.\n");
                    exit(EXIT_FAILURE);
                }
                for(int i = 0; i < MAX_CHATTERS; i++) {
                    messages[i] = malloc(MAX_MESSAGES * sizeof(t_message_s));
                    if (messages[i] == NULL) {
                        fprintf(stderr, "Error: Memory allocation failed for messages.\n");
                        exit(EXIT_FAILURE);
                    }
                    for(int j = 0; j < MAX_MESSAGES; j++) messages[i][j].text = NULL;
                }
                for(int i = 0; i < MAX_CHATTERS; i++) messages_count[i] = 0;
                load_message(username);

                refresh_scrollable_window2(scrollable_window2);
                // if() {
                    chatters[selected_user.index].lastmsg = format_last_msg((char *)messages[selected_user.index][messages_count[selected_user.index] - 1].text);
                    refresh_scrollable_window(scrollable_window);
                // }
                update_reload_status("false", username, chatters[selected_user.index].username); 
            }
            if(server_message_lastid != 0 && server_message_lastid != messages[selected_user.index][messages_count[selected_user.index] - 1].id) {
                g_print("%d - %d\n", server_message_lastid, messages[selected_user.index][messages_count[selected_user.index] - 1].id);
                char **response2 = get_mess_chat_last_text(mx_itoa(server_message_lastid));
                if(mx_strcmp((char *)response2, "1") == 0) {
                    g_print("Sadly, ended with error:(\n");
                    return TRUE;
                }
                if(mx_strcmp((char *)response2, "500") == 0) {
                    g_print("Server offline\n");
                    return TRUE;
                }
                char *token = strtok((char *)response2, "\n");
                char *new_text = strdup(token);
                token = strtok(NULL, "\n");
                char *new_time = strdup(token);

                add_message(server_message_lastid, selected_user.index, new_text, new_time, FALSE);
                refresh_scrollable_window2(scrollable_window2);
                chatters[selected_user.index].lastmsg = format_last_msg((char *)new_text);
                refresh_scrollable_window(scrollable_window);
            }
        }

    return TRUE;
}

bool chat_checker_thread_func(void *arg) {
    char *username = (char *)arg;
    int server_chats_amount = 0;
    while (1) {
        server_chats_amount = server_chats_quantity(username);
        if(server_chats_amount == -1) {
            continue;
        }
        
        if (server_chats_amount != chatters_count) {
            clear_chats();
            chatters_count = 0; 
            if (chatters != NULL) {
                free(chatters);
                chatters = NULL;
            }
            chatters = malloc(MAX_CHATTERS * sizeof(t_chatter_s));
            for(int i = 0; i < MAX_CHATTERS; i++) {
                chatters[i].username = NULL;
            }
            load_chats(username);
            refresh_scrollable_window(scrollable_window);
        }
        if(selected_user.index != -1) {
            int server_message_amount = mx_atoi((char *)get_mess_chat_amount(username, chatters[selected_user.index].username));
            int server_message_lastid = mx_atoi((char *)get_mess_chat_last_id(username, chatters[selected_user.index].username));
            int total_local_messages_amount = 0;
            for(int i = 0; i < MAX_CHATTERS; i++) {
                total_local_messages_amount += messages_count[i];
            }
            if(server_message_lastid != messages[selected_user.index][messages_count[selected_user.index] - 1].id) {
                char **response2 = get_mess_chat_last_text(mx_itoa(server_message_lastid));
                if(mx_strcmp((char *)response2, "1") == 0) {
                    g_print("Sadly, ended with error:(\n");
                    continue;
                }
                if(mx_strcmp((char *)response2, "500") == 0) {
                    g_print("Server offline\n");
                    continue;
                } 
                char *token = strtok((char *)response2, "\n");
                char *new_text = strdup(token);
                token = strtok(NULL, "\n");
                char *new_time = strdup(token);

                add_message(server_message_lastid, selected_user.index, new_text, new_time, FALSE);
                refresh_scrollable_window2(scrollable_window2);
                chatters[selected_user.index].lastmsg = format_last_msg((char *)new_text);
            }
        }
        sleep(1);
    }
    return true;
}

void start_chat_checker(char *username) {
    g_timeout_add_seconds(1, check_messages_async, username);
}

int stop_chat_checker(void) {
    return 0;
}

