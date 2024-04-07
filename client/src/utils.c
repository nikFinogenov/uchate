#include "uchat-client.h"
#include <math.h>

typedef struct {
    int index;
    GtkWidget *mess_box;
} ButtonPressEventUserData;
// const char *input_image_file = "client/img/simple.png"; // Input image file path
t_selected_s selected_user = {
    .index = -1,
    .box = NULL
};
// GtkWidget *selected_user = NULL;
void refresh_user_box() {
    // Clear the current contents of the chat_box
    gtk_container_foreach(GTK_CONTAINER(user_info_box), (GtkCallback)gtk_widget_destroy, NULL);

    // Refreshed content has been added to the chat_box, so show it
    draw_user_info_box(user_info_box);
    gtk_widget_show_all(user_info_box);
}

static void delete_message(GtkWidget *widget, gpointer data) {
    int index = GPOINTER_TO_INT(data);
    g_print("total gays: %d\n", messages_count[selected_user.index]);
    g_print("gay3 is here-> %d\n", index);
    if (index < 0 || index >= messages_count[selected_user.index]) {
        // Некорректный индекс
        return;
    }

    // Освобождаем память для удаляемого элемента
    free(messages[selected_user.index][index].text);
    free(messages[selected_user.index][index].time);
    // free(messages[index]); // Не нужно освобождать сам указатель на сообщение

    // Сдвигаем элементы влево для замены удаленного элемента
    for (int i = index; i < messages_count[selected_user.index] - 1; ++i) {
        messages[selected_user.index][i] = messages[selected_user.index][i + 1];
    }

    // Уменьшаем количество сообщений
    messages_count[selected_user.index]--;

    // После удаления сообщения, последний элемент массива должен быть обнулен
    // Это важно, чтобы не возникло повторного освобождения памяти в будущем
    memset(&messages[selected_user.index][messages_count[selected_user.index]], 0, sizeof(t_message_s));

    refresh_scrollable_window2(scrollable_window2);
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
    g_print("gay2 is here-> %d\n", index);
    GtkWidget *menu;
    GtkWidget *menu_item;
    
    // Create a new menu
    menu = gtk_menu_new();

    // Create menu items (buttons)
    menu_item = gtk_menu_item_new_with_label("Edit");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    menu_item = gtk_menu_item_new_with_label("Delete");
    g_signal_connect(menu_item, "activate", G_CALLBACK(delete_message), GINT_TO_POINTER(index));
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    // menu_item = gtk_menu_item_new_with_label("Button 3");
    // gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    menu_item = gtk_menu_item_new_with_label("React");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    // Show the menu
    gtk_widget_show_all(menu);

    // Popup the menu at the event coordinates
    gtk_menu_popup_at_pointer(GTK_MENU(menu), event);
}
static void create_chatter_menu(GdkEvent *event) {
    GtkWidget *menu;
    GtkWidget *menu_item;
    
    // Create a new menu
    menu = gtk_menu_new();

    // Create menu items (buttons)
    menu_item = gtk_menu_item_new_with_label("Delete");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);

    // Show the menu
    gtk_widget_show_all(menu);

    // Popup the menu at the event coordinates
    gtk_menu_popup_at_pointer(GTK_MENU(menu), event);
}
gboolean user_box_clicked(GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
    int index = GPOINTER_TO_INT(user_data);
    g_print("clicked -> ");

    if (event->type == GDK_BUTTON_PRESS) {
        if (event->button == GDK_BUTTON_SECONDARY) {
            // Right-click event handling
            create_chatter_menu(event);
            return TRUE; // Prevent further processing of the event
        } else if (event->button == GDK_BUTTON_PRIMARY) {
            // Left-click event handling
            if (selected_user.box != NULL && selected_user.index != index) {
                gtk_widget_override_background_color(selected_user.box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHT_GRAY, LIGHT_GRAY, LIGHT_GRAY, 1.0}); 
            }
            // Change background color of the clicked user box to indicate selection
            gtk_widget_override_background_color(widget, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY, LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
            // Update the reference to the currently selected user box
            selected_user.box = widget;
            selected_user.index = index;

            gtk_widget_hide(empty_chat);
            refresh_user_box();
            refresh_scrollable_window2(scrollable_window2);
            gtk_widget_show(scrollable_window2);

            // refresh_
            gtk_widget_show_all(chat_box);
            
            g_print("%d\n", selected_user.index);

            // Returning FALSE allows the event to propagate further
            return FALSE;
        }
    }
    return FALSE;
}
static gboolean on_button_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    // int index = GPOINTER_TO_INT(((gpointer*)data)[0]);
    int index = GPOINTER_TO_INT(data);
    g_print("gay is here-> %d\n", index);
    GtkWidget *event_box = GTK_WIDGET(widget);
    GtkWidget *mess_box = gtk_bin_get_child(GTK_BIN(event_box));
    if (event->type == GDK_BUTTON_PRESS && event->button == GDK_BUTTON_SECONDARY) {
        GtkWindow *parent_window = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(widget)));
        if (parent_window != NULL) {
            gint x, y;
            gtk_window_get_position(parent_window, &x, &y);
            GtkAllocation allocation;
            gtk_widget_get_allocation(GTK_WIDGET(mess_box), &allocation);
            //ебаная проверка всё ломает, как же похуй на неё
    //             g_print("Event coordinates: (%f, %f)\n", event->x_root, event->y_root);
    //             g_print("Widget position: (%d, %d)\n", x, y);
    //             g_print("Widget size: (%d, %d)\n", allocation.width, allocation.height);
    //         if (event->x_root >= allocation.x && event->x_root <= allocation.x + allocation.width &&
    // event->y_root >= allocation.y && event->y_root <= allocation.y + allocation.height) {
    //             g_print("triple xui\n\n\n");
            create_tools_menu((GdkEvent *)event, index);
            return TRUE; // Prevent further processing of the event
            // }
        }
    }
    return FALSE;
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
    // Get widget allocation
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    int img_w = alloc.width;
    int img_h = alloc.height;

    double corner_radius = 12.0;
    double degrees = M_PI / 180.0;
    double radius = corner_radius;

    // Draw rounded rectangle
    cairo_move_to(cr, radius, 0);
    cairo_arc(cr, img_w - radius, radius, radius, -90 * degrees, 0 * degrees);
    cairo_arc(cr, img_w - radius, img_h - radius, radius, 0 * degrees, 90 * degrees);
    cairo_arc(cr, radius, img_h - radius, radius, 90 * degrees, 180 * degrees);
    cairo_arc(cr, radius, radius, radius, 180 * degrees, 270 * degrees);
    cairo_close_path(cr);

    // Set source pixbuf and fill
    gdk_cairo_set_source_pixbuf(cr, data, 0, 0);
    cairo_fill(cr);
}

void set_widget_height(GtkWidget *widget, int height) {
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    alloc.height = height;
    gtk_widget_set_size_request(widget, alloc.width, height);
}

GtkWidget *create_user_box(char* tag, char* last_msg, char* input_image_file) {
    // Create an event box to make the user box clickable
    GtkWidget *event_box = gtk_event_box_new();
    gtk_widget_set_name(event_box, "clickable-user-box");

    // Create the user box
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    // gtk_widget_set_name(box, "user-box");
    gtk_widget_set_margin_start(box, 10);

    //avatar
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(input_image_file, NULL);
    GdkPixbuf *prev_pixbuf = gdk_pixbuf_copy(pixbuf);
    // prev_pixbuf = resize_img(prev_pixbuf, 150, 150);

    GtkWidget *image = gtk_drawing_area_new();
    gtk_widget_set_halign(GTK_WIDGET(image), GTK_ALIGN_CENTER);
    gtk_widget_set_size_request(GTK_WIDGET(image), gdk_pixbuf_get_width(GDK_PIXBUF(prev_pixbuf)), gdk_pixbuf_get_height(GDK_PIXBUF(prev_pixbuf)));
    g_signal_connect(G_OBJECT(image), "draw", G_CALLBACK(draw_image), prev_pixbuf);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 15);

    // Create user info box
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    // User tag
    GtkWidget *user_tag = gtk_label_new(tag);
    gtk_widget_set_name(user_tag, "user-tag");

    // User name and surname
    GtkWidget *last_mess = gtk_label_new(last_msg);
    gtk_widget_set_name(last_mess, "last-message");

    gtk_box_pack_start(GTK_BOX(info_box), user_tag, FALSE, FALSE, 7);
    gtk_box_pack_start(GTK_BOX(info_box), last_mess, FALSE, FALSE, 0);

    // Pack avatar and info_box into the user box
    gtk_box_pack_start(GTK_BOX(box), info_box, FALSE, FALSE, 0);

    // Pack the user box into the event box to make it clickable
    gtk_container_add(GTK_CONTAINER(event_box), box);
// g_signal_connect(event_box, "button-press-event", G_CALLBACK(user_box_clicked), NULL);
    return event_box;
}

void user_populate_scrollable_window(GtkWidget *scrollable_window) {

    GtkWidget *user_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), user_list);
    
    if (chatters != NULL) {
        // g_print("%d\n", chatters_count);s
        for (int i = 0; i < chatters_count; i++) {
            GtkWidget *user_box = create_user_box(chatters[i].username, chatters[i].lastmsg, default_img);
            gtk_widget_set_name(user_box, "user-box");
            if(i == selected_user.index) {
                selected_user.box = user_box;
                gtk_widget_override_background_color(user_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY, LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
            }
            g_signal_connect(user_box, "button-press-event", G_CALLBACK(user_box_clicked), GINT_TO_POINTER(i));

            gtk_box_pack_start(GTK_BOX(user_list), user_box, FALSE, FALSE, 0);
        }
    }
}
void user_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter) {
    GtkWidget *user_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), user_list);
    
    if (chatters != NULL) {
        // g_print("%d\n", chatters_count);s
        for (int i = 0; i < chatters_count; i++) {
            if(is_in_format(chatters[i].username, filter) || is_in_format(chatters[i].name, filter)) {
                GtkWidget *user_box = create_user_box(chatters[i].username, chatters[i].lastmsg, default_img);
                gtk_widget_set_name(user_box, "user-box");
                if(i == selected_user.index) {
                    selected_user.box = user_box;
                    gtk_widget_override_background_color(user_box, GTK_STATE_FLAG_NORMAL, &(GdkRGBA){LIGHTER_GRAY, LIGHTER_GRAY, LIGHTER_GRAY, 1.0}); 
                }
                g_signal_connect(user_box, "button-press-event", G_CALLBACK(user_box_clicked), GINT_TO_POINTER(i));

                gtk_box_pack_start(GTK_BOX(user_list), user_box, FALSE, FALSE, 0);
            }
        }
    }
}

GtkWidget *create_message_box(t_message_s *message) {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_margin_start(box, 10);

    // Create user info box
    GtkWidget *message_box = gtk_grid_new(); // Use GtkGrid for more control over positioning
    gtk_grid_set_row_homogeneous(GTK_GRID(message_box), FALSE); // Allow rows to have different heights
    gtk_grid_set_column_homogeneous(GTK_GRID(message_box), FALSE); // Allow columns to have different widths
    gtk_widget_set_name(message_box, "message-text");

    if(mx_strcmp(message->text, ":ucode") == 0) {
        GdkPixbuf *ucode_pixbuf = gdk_pixbuf_new_from_file("client/img/ucode.png", NULL);
        if (ucode_pixbuf != NULL) {
            // Create an image widget
            GtkWidget *ucode_image = gtk_image_new_from_pixbuf(resize_img(ucode_pixbuf, 128, 128));
            gtk_grid_attach(GTK_GRID(message_box), ucode_image, 0, 0, 1, 1); // Attach image to grid
            g_object_unref(ucode_pixbuf); // Unreference the pixbuf
        } else {
            // Failed to load the image, display an error message
            GtkWidget *error_label = gtk_label_new("Error: Failed to load image");
            gtk_grid_attach(GTK_GRID(message_box), error_label, 0, 0, 1, 1); // Attach error label to grid
        }
    }
    else {
        // Add text label
        GtkWidget *text_label = gtk_label_new(message->text);
        gtk_grid_attach(GTK_GRID(message_box), text_label, 0, 0, 1, 1); // Attach text label to grid        
    }


    // Add time label
    GtkWidget *time_label = gtk_label_new(message->time);
    gtk_widget_set_name(time_label, "message-time");
    gtk_widget_set_halign(time_label, GTK_ALIGN_END); // Align time label to the end (right)
    gtk_widget_set_valign(time_label, GTK_ALIGN_END); // Align time label to the end (bottom)
    gtk_grid_attach(GTK_GRID(message_box), time_label, 1, 1, 1, 1); // Attach time label to grid

    // Add message box to main box
    if(message->is_user) {
        gtk_box_pack_end(GTK_BOX(box), message_box, FALSE, TRUE, 0);
    }
    else gtk_box_pack_start(GTK_BOX(box), message_box, FALSE, TRUE, 0);
    
    return box;
}


void message_populate_scrollable_window(GtkWidget *scrollable_window) {
    GtkWidget *mess_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), mess_list);

    // for(mes)
    if (messages != NULL) {
        // for (int i = 0; messages[s].text != NULL; i++) {
        for (int i = 0; i < messages_count[selected_user.index]; i++) {
            GtkWidget *event_box = gtk_event_box_new();
            gtk_container_add(GTK_CONTAINER(mess_list), event_box); // Здесь добавляется event_box в mess_list
            GtkWidget *mess_box = create_message_box(&messages[selected_user.index][i]);
            gtk_container_add(GTK_CONTAINER(event_box), mess_box);

            // gpointer user_data[] = {GINT_TO_POINTER(i)};
            // ButtonPressEventUserData *user_data = g_new(ButtonPressEventUserData, 1);
            int i_copy = i;
            // user_data->mess_box = mess_box;
            g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_button_press), GINT_TO_POINTER(i_copy));
        }
    }
}
void message_populate_scrollable_filtred_window(GtkWidget *scrollable_window, char* filter) {
    GtkWidget *mess_list = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(scrollable_window), mess_list);

    // for(mes)
    if (messages != NULL) {
        // for (int i = 0; messages[s].text != NULL; i++) {
        for (int i = 0; i < messages_count[selected_user.index]; i++) {
            if(is_in_format(messages[selected_user.index][i].text, filter)){
                GtkWidget *event_box = gtk_event_box_new();
                gtk_container_add(GTK_CONTAINER(mess_list), event_box); // Здесь добавляется event_box в mess_list
                GtkWidget *mess_box = create_message_box(&messages[selected_user.index][i]);
                gtk_container_add(GTK_CONTAINER(event_box), mess_box);
                g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_button_press), mess_box);
            }
        }
    }
}

char* format_last_msg(char* text) {
        int len = strlen(text);
    int max_length = 25; // Maximum length of the returned string (excluding "...")

    // Allocate memory for the formatted string
    char* formatted_text = (char*)malloc((max_length + 4) * sizeof(char)); // 4 extra chars for "..." and null terminator
    if (formatted_text == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    // Copy the first 25 characters of the input text
    strncpy(formatted_text, text, max_length);

    // Null-terminate the string
    formatted_text[max_length] = '\0';

    // Add "..." to indicate truncated text if the original text is longer than 25 characters
    if (len > max_length) {
        strcat(formatted_text, "...");
    }

    return formatted_text;
}

gboolean is_in_format(char* text, char* format) {
    size_t text_len = strlen(text);
    size_t format_len = strlen(format);
    
    if (format_len > text_len) {
        return FALSE; // Format is longer than text, so text can't match format
    }
    
    // Iterate through the text to find the first character of the format
    for (size_t i = 0; i <= text_len - format_len; ++i) {
        if (strncmp(text + i, format, format_len) == 0) {
            return TRUE; // Found the format in the text
        }
    }
    
    return FALSE; // Format not found in text
}